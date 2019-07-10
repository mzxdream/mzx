#ifndef __MZX_AIO_OPERATION_H__
#define __MZX_AIO_OPERATION_H__

#include <functional>

#include <mzx/error.h>

namespace mzx
{

class AIOOperation
{

public:
    AIOOperation();
    virtual ~AIOOperation() = 0;

public:
    bool Perform();
    void Complete();

private:
    virtual bool _Perform();
    virtual void _Complete();
};

class AIOCompleteOperation : public AIOOperation
{
public:
    using CompleteFunc = std::function<void()>;

public:
    AIOCompleteOperation(CompleteFunc func);

private:
    virtual void _Complete() override;

private:
    CompleteFunc func_;
};

class AIOReadOperation : public AIOOperation
{
public:
    using CompleteFunc = std::function<void(const Error &)>;

public:
    AIOReadOperation(int fd, char *data, std::size_t size, CompleteFunc func);

private:
    virtual bool _Perform() override;
    virtual void _Complete() override;

private:
    Error error_;
    int fd_{-1};
    char *data_{nullptr};
    std::size_t size_{0};
    std::size_t read_size_{0};
    CompleteFunc func_;
};

class AIOWriteOperation : public AIOOperation
{
public:
    using CompleteFunc = std::function<void(const Error &)>;

public:
    AIOWriteOperation(int fd, const char *data, std::size_t size,
                      CompleteFunc func);

private:
    virtual bool _Perform() override;
    virtual void _Complete() override;

private:
    Error error_;
    int fd_{-1};
    const char *data_{nullptr};
    std::size_t size_{0};
    std::size_t write_size_{0};
    CompleteFunc func_;
};

class AIOConnectOperation : public AIOOperation
{
public:
    using CompleteFunc = std::function<void(const Error &)>;
public:
    AIOConnectOperation(int sock, const NetAddress &addr)
};

} // namespace mzx

#endif
