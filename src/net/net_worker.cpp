#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <mzx/logger.h>
#include <mzx/net/net_worker.h>

namespace mzx
{

NetWorker::NetWorker()
    : thread_(std::bind(&NetWorker::Run, this))
{
}

NetWorker::~NetWorker()
{
    Uninit();
}

bool NetWorker::InitIOServer()
{
    epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
    if (epoll_fd_ < 0)
    {
        MZX_ERR("create epoll fd failed");
        return false;
    }
    wakeup_fd_ = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (wakeup_fd_ < 0)
    {
        MZX_ERR("create wakeup fd failed");
        close(epoll_fd_);
        return false;
    }
    epoll_event ee;
    ee.events = EPOLLIN | EPOLLET;
    ee.data.ptr = this;
    int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, wakeup_fd_, &ee);
    if (ret < 0)
    {
        MZX_ERR("add wakeup fd:", wakeup_fd_, " in efd:", epoll_fd_, " failed");
        close(epoll_fd_);
        close(wakeup_fd_);
        return false;
    }
    return true;
}

bool NetWorker::Init(const NetConf &conf)
{
    MZX_CHECK(conf.connector_count > 0);
    MZX_CHECK(conf.input_event_count > 0);
    MZX_CHECK(conf.output_event_count > 0);
    MZX_CHECK(conf.input_buffer_count > 0 && conf.input_buffer_size > 0);
    MZX_CHECK(conf.output_buffer_count > 0 && conf.output_buffer_size > 0);
    if (!InitIOServer())
    {
        MZX_ERR("init io server failed");
        return false;
    }
    for (std::size_t i = 0; i < conf.connector_count; ++i)
    {
        auto *connector = new NetConnector();
        connector->id = InitNetConnectionID(NetConnectionType::kConnector,
                                            connector_list_.size());
        connector_list_.emplace_back(connector);
        connector_free_list_.PushBack(&connector->list_link);
    }
    input_event_list_ = new SPSCQueue<NetInputEvent>(conf.input_event_count);
    output_event_list_ = new SPSCQueue<NetOutputEvent>(conf.output_event_count);
    input_buffer_free_list_ =
        new SPSCQueue<NetBuffer *>(conf.input_buffer_count);
    for (std::size_t i = 0; i < conf.input_buffer_count; ++i)
    {
        auto *buffer = new NetBuffer(conf.input_buffer_size);
        input_buffer_list_.emplace_back(buffer);
        input_buffer_free_list_->Push(buffer);
    }
    output_buffer_free_list_ =
        new SPSCQueue<NetBuffer *>(conf.output_buffer_count);
    for (std::size_t i = 0; i < conf.output_buffer_count; ++i)
    {
        auto *buffer = new NetBuffer(conf.output_buffer_size);
        output_buffer_list_.emplace_back(buffer);
        output_buffer_free_list_->Push(buffer);
    }
    stop_flag_ = false;
    thread_.Start();
    return true;
}

void NetWorker::Uninit()
{
    stop_flag_.store(true, std::memory_order_relaxed);
    Wakeup();
    thread_.Join();
    if (epoll_fd_ > 0)
    {
        close(epoll_fd_);
        epoll_fd_ = -1;
    }
    if (wakeup_fd_ > 0)
    {
        close(wakeup_fd_);
        wakeup_fd_ = -1;
    }
    for (std::size_t i = 0; i < connector_list_.size(); ++i)
    {
        // TODO
    }
    // TODO
    delete input_event_list_;
    delete output_event_list_;
    delete input_buffer_free_list_;
    delete output_buffer_free_list_;
}

bool NetWorker::InputEventWriteAvailable() const
{
    return input_event_list_->WriteAvailable();
}

void NetWorker::Wakeup()
{
    uint64_t tmp = 1;
    write(wakeup_fd_, &tmp, sizeof(tmp));
}

void NetWorker::OnWakeup()
{
    uint64_t tmp = 0;
    read(wakeup_fd_, &tmp, sizeof(tmp));
}

void NetWorker::Run()
{
    epoll_event events[1024];
    while (!stop_flag_.load(std::memory_order_relaxed))
    {
        int timeout = -1;
        if (!active_handler_list_.Empty())
        {
            timeout = 0;
        }
        // TODO
        int nfds =
            epoll_wait(epoll_fd_, events, MZX_ARRAY_SIZE(events), timeout);
        if (nfds == -1)
        {
            if (errno != EINTR)
            {
                MZX_FATAL("epoll wait failed:", errno);
            }
        }
        for (int i = 0; i < nfds; ++i)
        {
            auto *p = events[i].data.ptr;
            if (p == this)
            {
                OnWakeup();
                continue;
            }
            auto *handler = static_cast<NetHandler *>(p);
            handler->active_events |= events[i].events;
            if (!handler->list_link.IsLinked())
            {
                active_handler_list_.PushBack(&handler->list_link);
            }
        }
        for (auto *node = active_handler_list_.Begin();
             node != active_handler_list_.End();)
        {
            auto *handler = MZX_CONTAINER_OF(node, NetHandler, list_link);
            MZX_CHECK(handler != nullptr && handler->event_cb != nullptr);
            if (handler->event_cb(this, handler))
            {
                node = active_handler_list_.Erase(node);
            }
            else
            {
                node = node->Next();
            }
        }
        if (!active_handler_list_.Empty())
        {
            auto *end_node = active_handler_list_.RBegin();
            ListNode *node = nullptr;
            while (true)
            {
                node = active_handler_list_.PopFront();
            }
        }
    }
}

void uv__io_poll(uv_loop_t *loop, int timeout)
{
    /* A bug in kernels < 2.6.37 makes timeouts larger than ~30 minutes
     * effectively infinite on 32 bits architectures.  To avoid blocking
     * indefinitely, we cap the timeout and poll again if necessary.
     *
     * Note that "30 minutes" is a simplification because it depends on
     * the value of CONFIG_HZ.  The magic constant assumes CONFIG_HZ=1200,
     * that being the largest value I have seen in the wild (and only once.)
     */
    static const int max_safe_timeout = 1789569;
    struct epoll_event events[1024];
    struct epoll_event *pe;
    struct epoll_event e;
    int real_timeout;
    QUEUE *q;
    uv__io_t *w;
    sigset_t sigset;
    sigset_t *psigset;
    uint64_t base;
    int have_signals;
    int nevents;
    int count;
    int nfds;
    int fd;
    int op;
    int i;

    if (loop->nfds == 0)
    {
        assert(QUEUE_EMPTY(&loop->watcher_queue));
        return;
    }

    memset(&e, 0, sizeof(e));

    while (!QUEUE_EMPTY(&loop->watcher_queue))
    {
        q = QUEUE_HEAD(&loop->watcher_queue);
        QUEUE_REMOVE(q);
        QUEUE_INIT(q);

        w = QUEUE_DATA(q, uv__io_t, watcher_queue);
        assert(w->pevents != 0);
        assert(w->fd >= 0);
        assert(w->fd < (int)loop->nwatchers);

        e.events = w->pevents;
        e.data.fd = w->fd;

        if (w->events == 0)
            op = EPOLL_CTL_ADD;
        else
            op = EPOLL_CTL_MOD;

        /* XXX Future optimization: do EPOLL_CTL_MOD lazily if we stop watching
         * events, skip the syscall and squelch the events after epoll_wait().
         */
        if (epoll_ctl(loop->backend_fd, op, w->fd, &e))
        {
            if (errno != EEXIST)
                abort();

            assert(op == EPOLL_CTL_ADD);

            /* We've reactivated a file descriptor that's been watched before.
             */
            if (epoll_ctl(loop->backend_fd, EPOLL_CTL_MOD, w->fd, &e))
                abort();
        }

        w->events = w->pevents;
    }

    psigset = NULL;
    if (loop->flags & UV_LOOP_BLOCK_SIGPROF)
    {
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGPROF);
        psigset = &sigset;
    }

    assert(timeout >= -1);
    base = loop->time;
    count = 48; /* Benchmarks suggest this gives the best throughput. */
    real_timeout = timeout;

    for (;;)
    {
        /* See the comment for max_safe_timeout for an explanation of why
         * this is necessary.  Executive summary: kernel bug workaround.
         */
        if (sizeof(int32_t) == sizeof(long) && timeout >= max_safe_timeout)
            timeout = max_safe_timeout;

        nfds = epoll_pwait(loop->backend_fd, events, ARRAY_SIZE(events),
                           timeout, psigset);

        /* Update loop->time unconditionally. It's tempting to skip the update
         * when timeout == 0 (i.e. non-blocking poll) but there is no guarantee
         * that the operating system didn't reschedule our process while in the
         * syscall.
         */
        SAVE_ERRNO(uv__update_time(loop));

        if (nfds == 0)
        {
            assert(timeout != -1);

            if (timeout == 0)
                return;

            /* We may have been inside the system call for longer than |timeout|
             * milliseconds so we need to update the timestamp to avoid drift.
             */
            goto update_timeout;
        }

        if (nfds == -1)
        {
            if (errno != EINTR)
                abort();

            if (timeout == -1)
                continue;

            if (timeout == 0)
                return;

            /* Interrupted by a signal. Update timeout and poll again. */
            goto update_timeout;
        }

        have_signals = 0;
        nevents = 0;

        assert(loop->watchers != NULL);
        loop->watchers[loop->nwatchers] = (void *)events;
        loop->watchers[loop->nwatchers + 1] = (void *)(uintptr_t)nfds;
        for (i = 0; i < nfds; i++)
        {
            pe = events + i;
            fd = pe->data.fd;

            /* Skip invalidated events, see uv__platform_invalidate_fd */
            if (fd == -1)
                continue;

            assert(fd >= 0);
            assert((unsigned)fd < loop->nwatchers);

            w = loop->watchers[fd];

            if (w == NULL)
            {
                /* File descriptor that we've stopped watching, disarm it.
                 *
                 * Ignore all errors because we may be racing with another
                 * thread when the file descriptor is closed.
                 */
                epoll_ctl(loop->backend_fd, EPOLL_CTL_DEL, fd, pe);
                continue;
            }

            /* Give users only events they're interested in. Prevents spurious
             * callbacks when previous callback invocation in this loop has
             * stopped the current watcher. Also, filters out events that users
             * has not requested us to watch.
             */
            pe->events &= w->pevents | POLLERR | POLLHUP;

            /* Work around an epoll quirk where it sometimes reports just the
             * EPOLLERR or EPOLLHUP event.  In order to force the event loop to
             * move forward, we merge in the read/write events that the watcher
             * is interested in; uv__read() and uv__write() will then deal with
             * the error or hangup in the usual fashion.
             *
             * Note to self: happens when epoll reports EPOLLIN|EPOLLHUP, the
             * user reads the available data, calls uv_read_stop(), then
             * sometime later calls uv_read_start() again.  By then, libuv has
             * forgotten about the hangup and the kernel won't report EPOLLIN
             * again because there's nothing left to read.  If anything, libuv
             * is to blame here.  The current hack is just a quick bandaid; to
             * properly fix it, libuv needs to remember the error/hangup event.
             * We should get that for free when we switch over to edge-triggered
             * I/O.
             */
            if (pe->events == POLLERR || pe->events == POLLHUP)
                pe->events |= w->pevents &
                              (POLLIN | POLLOUT | UV__POLLRDHUP | UV__POLLPRI);

            if (pe->events != 0)
            {
                /* Run signal watchers last.  This also affects child process
                 * watchers because those are implemented in terms of signal
                 * watchers.
                 */
                if (w == &loop->signal_io_watcher)
                    have_signals = 1;
                else
                    w->cb(loop, w, pe->events);

                nevents++;
            }
        }

        if (have_signals != 0)
            loop->signal_io_watcher.cb(loop, &loop->signal_io_watcher, POLLIN);

        loop->watchers[loop->nwatchers] = NULL;
        loop->watchers[loop->nwatchers + 1] = NULL;

        if (have_signals != 0)
            return; /* Event loop should cycle now so don't poll again. */

        if (nevents != 0)
        {
            if (nfds == ARRAY_SIZE(events) && --count != 0)
            {
                /* Poll for more events but don't block this time. */
                timeout = 0;
                continue;
            }
            return;
        }

        if (timeout == 0)
            return;

        if (timeout == -1)
            continue;

    update_timeout:
        assert(timeout > 0);

        real_timeout -= (loop->time - base);
        if (real_timeout <= 0)
            return;

        timeout = real_timeout;
    }
}

} // namespace mzx
