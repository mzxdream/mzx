#ifndef __MZX_BLOB_H__
#define __MZX_BLOB_H__

#include <string>

namespace mzx {

class Blob
{
public:
    Blob();
    ~Blob();
    Blob(const Blob &) = default;
    Blob& operator=(const Blob &) = default;
public:
    const char * GetData() const;
    char * GetData();
    std::size_t GetSize() const;
    void Resize(std::size_t size);
private:
    std::string data_;
};

}

#endif
