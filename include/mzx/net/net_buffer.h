#ifndef __MZX_NET_BUFFER_H__
#define __MZX_NET_BUFFER_H__

#include <cstdint>

#include <mzx/logger.h>

namespace mzx
{

class NetBuffer final
{
public:
    NetBuffer(std::size_t size)
    {
        MZX_CHECK(size > 0);
        data_ = (char *)malloc(sizeof(char) * size);
        MZX_CHECK(data_ != nullptr);
        size_ = size;
    }
    ~NetBuffer()
    {
        if (data_)
        {
            free(data_);
            data_ = nullptr;
        }
    }
    NetBuffer(const NetBuffer &) = delete;
    NetBuffer &operator=(const NetBuffer &) = delete;

public:
    char *Data()
    {
        return data_;
    }
    const char *Data() const
    {
        return data_;
    }
    std::size_t Size() const
    {
        return size_;
    }
    const char *ReadBegin() const
    {
        return data_ + read_pos_;
    }
    char *WriteBegin()
    {
        return data_ + write_pos_;
    }
    std::size_t ReadAvailable() const
    {
        return write_pos_ - read_pos_;
    }
    std::size_t WriteAvailable() const
    {
        return size_ - write_pos_;
    }
    void Read(std::size_t size)
    {
        MZX_CHECK(read_pos_ + size <= write_pos_);
        read_pos_ += size;
    }
    void Write(std::size_t size)
    {
        MZX_CHECK(write_pos_ + size <= size);
    }

private:
    char *data_{nullptr};
    std::size_t size_{0};
    std::size_t read_pos_{0};
    std::size_t write_pos_{0};
};

} // namespace mzx

#endif
