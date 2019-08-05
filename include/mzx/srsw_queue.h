#ifndef __MZX_SRSW_QUEUE_H__
#define __MZX_SRSW_QUEUE_H__

#include <atomic>
#include <cstdlib>
#include <functional>

#include <mzx/logger.h>

namespace mzx
{

template <typename T>
class SRSWQueue final
{
public:
    explicit SRSWQueue(std::size_t max_size)
        : max_size_(max_size + 1)
    {
        MZX_CHECK(max_size_ > 1);
        data_ = static_cast<char *>(malloc(sizeof(T) * max_size_));
    }
    ~SRSWQueue()
    {
        T out;
        while (Pop(&out))
        {
        }
        free(data_);
        data_ = nullptr;
    }
    SRSWQueue(const SRSWQueue &) = delete;
    SRSWQueue &operator=(const SRSWQueue &) = delete;

public:
    std::size_t ReadAvailable(std::size_t write_index,
                              std::size_t read_index) const
    {
        if (write_index >= read_index)
        {
            return write_index - read_index;
        }
        return write_index + max_size_ - read_index;
    }

    std::size_t ReadAvailable() const
    {
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(std::memory_order_relaxed);
        return ReadAvailable(write_index, read_index);
    }

    std::size_t WriteAvailable(std::size_t write_index,
                               std::size_t read_index) const
    {
        if (write_index >= read_index)
        {
            return max_size_ - 1 - write_index + read_index;
        }
        return read_index - 1 - write_index;
    }

    std::size_t WriteAvailable() const
    {
        auto write_index = write_index_.load(std::memory_order_relaxed);
        auto read_index = read_index_.load(std::memory_order_acquire);
        return WriteAvailable(write_index, read_index);
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
        new (Data(write_index)) T(data);
        write_index_.store(next_index, std::memory_order_release);
        return true;
    }

    std::size_t Write(std::function<void(T &)> cb, std::size_t max_size = -1)
    {
        MZX_CHECK(cb != nullptr && max_size != 0);
        auto write_index = write_index_.load(std::memory_order_relaxed);
        auto read_index = read_index_.load(std::memory_order_acquire);
        auto write_avail = WriteAvailable(write_index, read_index);
        if (write_avail == 0)
        {
            return 0;
        }
        if (max_size > write_avail)
        {
            max_size = write_avail;
        }
        for (std::size_t i = 0; i < max_size; ++i)
        {
            auto *data = Data(write_index);
            data->T();
            cb(*data);
            write_index = NextIndex(write_index);
        }
        write_index_.store(write_index, std::memory_order_release);
        return max_size;
    }

    bool Pop(T *data)
    {
        MZX_CHECK(data != nullptr);
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(memory_order_relaxed);
        if (write_index == read_index)
        {
            return false;
        }
        auto &tmp = Data(read_index);
        *data = tmp;
        tmp.~T();
        read_index_.store(NextIndex(read_index), std::memory_order_release);
        return true;
    }

    std::size_t Read(std::function<void(const T &)> cb,
                     std::size_t max_size = -1)
    {
        MZX_CHECK(cb != nullptr && max_size != 0);
        auto write_index = write_index_.load(std::memory_order_acquire);
        auto read_index = read_index_.load(memory_order_relaxed);
        auto read_avail = ReadAvailable(write_index, read_index);
        if (read_avail == 0)
        {
            return 0;
        }
        if (max_size > read_avail)
        {
            max_size = read_avail;
        }
        for (std::size_t i = 0; i < max_size; ++i)
        {
            auto *data = Data(read_index);
            cb(*data);
            data->~T();
            read_index = NextIndex(read_index);
        }
        read_index_.store(read_index, std::memory_order_release);
        return read_avail;
    }

private:
    T *Data(std::size_t index)
    {
        return static_cast<T *>(data_)[index];
    }

private:
    char *data_{nullptr};
    std::size_t max_size_{0};
    std::atomic<std::size_t> read_index_{0};
    std::atomic<std::size_t> write_index_{0};
};

} // namespace mzx

#endif
