#ifndef _M_ERRNO_H_
#define _M_ERRNO_H_

#include <errno.h>
#include <string>

enum class MError:int
{
    No = 0,
    Unknown = -1,
    OperationNotPermitted = EPERM,
    NoSuchFileOrDirectory = ENOENT,
    NoSuchProcess = ESRCH,
    InterruptedSystemCall = EINTR,
    IOError = EIO,
    NoSuchDeviceOrAddress = ENXIO,
    ArgumentListTooLong = E2BIG,
    ExecFormatError = ENOEXEC,
    BadFileNumber = EBADF,
    NoChildProcesses = ECHILD,
    TryAgain = EAGAIN,
    OutOfMemory = ENOMEM,
    PermissionDenied = EACCES,
    DeviceOrResourceBusy = EBUSY,
    InvalidArgument = EINVAL,
    FileTableOverflow = ENFILE,
    TooManyOpenFiles = EMFILE,
    NoData = ENODATA,
    TimerExpired = ETIME,
    AddressInUse = EADDRINUSE,
    NoBufferSpace = ENOBUFS,
    ConnectionRefused = ECONNREFUSED,
    InProgress = EINPROGRESS,
    ConnectReset = ECONNRESET,
};

MError MGetLastError();
std::string MGetLastErrorMsg();
MError MMakeError(int err);
std::string MGetErrorMsg(int err);
std::string MGetErrorMsg(MError err);

#endif
