#include <mzx/aio/aio_operation.h>
#include <mzx/logger.h>

namespace mzx
{

AIOOperation::AIOOperation()
{
}

AIOOperation::~AIOOperation()
{
}

bool AIOOperation::Perform()
{
    return _Perform();
}

void AIOOperation::Complete()
{
    return _Complete();
}

AIOCompleteOperation::AIOCompleteOperation(CompleteFunc func)
    : func_(func)
{
    MZX_CHECK(func_ != nullptr);
}

void AIOCompleteOperation::_Complete()
{
    func_();
}

AIOReadOperation::AIOReadOperation(int fd, char *data, std::size_t size,
                                   CompleteFunc func)
    : fd_(fd)
    , data_(data)
    , size_(size)
    , func_(func)
{
    MZX_CHECK(fd_ >= 0 && data_ != nullptr && size_ > 0 && func_ != nullptr);
}

bool AIOReadOperation::_Perform()
{
    return true;
}

void AIOReadOperation::_Complete()
{
    func_(error_);
}

AIOWriteOperation::AIOWriteOperation(int fd, const char *data, std::size_t size,
                                     CompleteFunc func)
    : fd_(fd)
    , data_(data)
    , size_(size)
    , func_(func)
{
    MZX_CHECK(fd_ >= 0 && data_ != nullptr && size_ > 0 && func_ != nullptr);
}

bool AIOWriteOperation::_Perform()
{
    return true;
}

void AIOWriteOperation::_Complete()
{
    func_(error_);
}

} // namespace mzx
