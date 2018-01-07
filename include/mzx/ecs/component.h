#ifndef __MZX_ECS_COMPONENT_H__
#define __MZX_ECS_COMPONENT_H__

#include <mzx/ecs/object.h>

namespace mzx {
namespace ecs {

class Component
    : public Object
{
public:
    Component();
    virtual ~Component();
private:

};

}
}

#endif
