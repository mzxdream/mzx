#ifndef __MZX_NONCOPYABLE_H__
#define __MZX_NONCOPYABLE_H__

namespace mzx
{

class Noncopyable
{
protected:
    Noncopyable() = default;
    ~Noncopyable() = default;

public:
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;
};

} // namespace mzx

#endif
