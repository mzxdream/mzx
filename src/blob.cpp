#include <mzx/blob.h>

namespace mzx {

    Blob::Blob()
    {
    }

    Blob::~Blob()
    {
    }

    const char * Blob::GetData() const
    {
        return &data_[0];
    }

    char * Blob::GetData()
    {
        return const_cast<char*>(static_cast<const Blob&>(*this).GetData());
    }

    std::size_t Blob::GetSize() const
    {
        return data_.size();
    }

    void Blob::Resize(std::size_t size)
    {
        data_.resize(size);
    }

}
