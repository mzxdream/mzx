#ifndef __MZX_SPSC_QUEUE_H__
#define __MZX_SPSC_QUEUE_H__

#include <atomic>
#include <cstdlib>
#include <functional>
#include <type_traits>

#include <mzx/logger.h>

namespace mzx
{

template <typename T>
class SPSCQueue final
{
public:
    explicit SPSCQueue(std::size_t max_size)
        : max_size_(max_size + 1)
    {
        MZX_CHECK(max_size_ > 1);
        data_ = static_cast<char *>(malloc(sizeof(T) * max_size_));
    }

    ~SPSCQueue()
    {
        T out;
        while (Pop(&out))
        {
        }
        free(data_);
    }

    SPSCQueue(const SPSCQueue &) = delete;
    SPSCQueue &operator=(const SPSCQueue &) = delete;

public:
    std::size_t ReadAvailable() const
    {
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(std::memory_order_relaxed);
        return ReadAvailable(write_index, read_index, max_size_);
    }

    std::size_t WriteAvailable() const
    {
        auto write_index = write_index_.load(std::memory_order_relaxed);
        auto read_index = read_index_.load(std::memory_order_acquire);
        return WriteAvailable(write_index, read_index, max_size_);
    }

    bool MaxSize() const
    {
        return max_size_;
    }

    std::size_t NextIndex(std::size_t index)
    {
        ++index;
        while (index >= max_size_)
        {
            index -= max_size_;
        }
        return index;
    }

    bool Push(const T &data)
    {
        auto write_index = write_index_.load(std::memory_order_relaxed);
        auto next_index = NextIndex(write_index);
        if (next_index == read_index_.load(std::memory_order_acquire))
        {
            return false;
        }
        new (Data() + write_index) T(data);
        write_index_.store(next_index, std::memory_order_release);
        return true;
    }

    std::size_t Product(std::function<void(T &)> cb,
                        std::size_t count = (std::size_t)-1)
    {
        MZX_CHECK(cb != nullptr && count > 0);
        auto write_index = write_index_.load(std::memory_order_relaxed);
        auto read_index = read_index_.load(std::memory_order_acquire);
        auto write_avail = WriteAvailable(write_index, read_index, max_size_);
        if (write_avail == 0)
        {
            return 0;
        }
        count = std::min(count, write_avail);
        for (std::size_t i = 0; i < count; ++i)
        {
            auto &tmp = Data()[write_index];
            tmp.T();
            cb(tmp);
            write_index = NextIndex(write_index);
        }
        write_index_.store(write_index, std::memory_order_release);
        return count;
    }

    bool Pop(T *data)
    {
        MZX_CHECK(data != nullptr);
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(std::memory_order_relaxed);
        if (write_index == read_index)
        {
            return false;
        }
        auto &tmp = Data()[read_index];
        *data = *tmp;
        tmp.~T();
        auto next_index = NextIndex(read_index);
        read_index_.store(next_index, std::memory_order_release);
        return true;
    }

    std::size_t Consume(std::function<void(const T &)> cb,
                        std::size_t count = (std::size_t)-1)
    {
        MZX_CHECK(cb != nullptr && count > 0);
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(std::memory_order_relaxed);
        auto read_avail = ReadAvailable(write_index, read_index);
        if (read_avail == 0)
        {
            return 0;
        }
        count = std::min(count, read_avail);
        for (std::size_t i = 0; i < count; ++i)
        {
            auto &tmp = Data()[read_index];
            cb(tmp);
            tmp.~T();
            read_index = NextIndex(read_index);
        }
        read_index_.store(read_index, std::memory_order_release);
        return count;
    }

private:
    T *Data()
    {
        return static_cast<T *>(data_);
    }

    const T *Data() const
    {
        return static_cast<const T *>(data_);
    }

    static std::size_t ReadAvailable(std::size_t write_index,
                                     std::size_t read_index,
                                     std::size_t max_size)
    {
        if (write_index >= read_index)
        {
            return write_index - read_index;
        }
        return write_index + max_size - read_index;
    }

    static std::size_t WriteAvailable(std::size_t write_index,
                                      std::size_t read_index,
                                      std::size_t max_size)
    {
        if (write_index >= read_index)
        {
            return max_size - write_index + read_index - 1;
        }
        return read_index - write_index - 1;
    }

private:
    char *data_{nullptr};
    std::size_t max_size_{0};
    std::atomic<std::size_t> read_index_{0};
    std::atomic<std::size_t> write_index_{0};
};

} // namespace mzx

#endif
