#ifndef __MZX_SRSW_QUEUE_H__
#define __MZX_SRSW_QUEUE_H__

#include <atomic>
#include <cstdlib>

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
    bool Push(const T &data)
    {
        auto write_index = write_index_.load(std::memory_order_relaxed);
        if (++write_index >= max_size_)
        {
            write_index -= max_size_;
        }
        if (write_index == read_index_.load(std::memory_order_acquire))
        {
            return false;
        }
        new (Data(write_index)) T(data);
        write_index_.store(write_index, std::memory_order_release);
        return true;
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
        if (++read_index >= max_size_)
        {
            read_index -= max_size_;
        }
        read_index_.store(read_index, std::memory_order_release);
        return true;
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
