#ifndef __MZX_CLONEABLE_H__
#define __MZX_CLONEABLE_H__

namespace mzx {

class Cloneable
{
protected:
    Cloneable() = default;
    ~Cloneable() = default;
public:
    virtual Cloneable* Clone() = 0;
};

}

#endif
