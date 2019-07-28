#ifndef __MZX_SRSW_QUEUE_H__
#define __MZX_SRSW_QUEUE_H__

#include <atomic>

namespace mzx
{

template <typename T, std::size_t max_size>
class SRSWQueue final
{
public:
    SRSWQueue();
    ~SRSWQueue();
    SRSWQueue(const SRSWQueue &) = delete;
    SRSWQueue &operator=(const SRSWQueue &) = delete;

public:
    bool Push(const T &data)
    {
        return true;
    }
private:
    T data_[max_size + 1];
    std::atomic<std::size_t> read_index_;
    std::atomic<std::size_t> write_index_;
};

} // namespace mzx

#endif
