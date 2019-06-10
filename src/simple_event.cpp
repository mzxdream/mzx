#include <mzx/simple_event.h>

namespace mzx
{

SimpleEventBase::ClassIndexType SimpleEventBase::class_index_counter_ = 0;

SimpleEventBase::SimpleEventBase()
{
}

SimpleEventBase::~SimpleEventBase()
{
}

SimpleEventBase::ClassIndexType SimpleEventBase::ClassIndexCount()
{
    return class_index_counter_;
}

} // namespace mzx
