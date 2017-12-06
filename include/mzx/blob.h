#ifndef __MZX_BLOB_H__
#define __MZX_BLOB_H__

#include <string>

namespace mzx {

class Blob
{
public:
    Blob() = default;
    ~Blob() = default;
    Blob(const Blob &) = default;
    Blob& operator=(const Blob &) = default;
public:
    const char* GetData() const
    {
        return &data_[0];
    }
    char* GetData()
    {
        return const_cast<char*>(static_cast<const Blob&>(*this).GetData());
    }
    size_t GetSize() const
    {
        return data_.size();
    }
    void Resize(size_t size)
    {
        data_.resize(size);
    }
private:
    std::string data_;
};

}

#endif
