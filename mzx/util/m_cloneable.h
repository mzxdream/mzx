#ifndef _M_CLONEABLE_H_
#define _M_CLONEABLE_H_

class MCloneable
{
protected:
    MCloneable() = default;
    ~MCloneable() = default;
public:
    virtual MCloneable* Clone() = 0;
};

#endif
