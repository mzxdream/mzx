#ifndef _M_ERRNO_H_
#define _M_ERRNO_H_

#include <string>

enum class MError
{
    No,
    Unknown,
    OperationNotPermitted,
    NoSuchFileOrDirectory,
    NoSuchProcess,
    InterruptedSystemCall,
    IOError,
    NoSuchDeviceOrAddress,
    ArgumentListTooLong,
    ExecFormatError,
    BadFileNumber,
    NoChildProcesses,
    TryAgain,
    OutOfMemory,
    PermissionDenied,
    DeviceOrResourceBusy,
    InvalidArgument,
    FileTableOverflow,
    TooManyOpenFiles,
    NoData,
    TimerExpired,
    AddressInUse,
    NoBufferSpace,
    ConnectionRefused,
};

MError MGetLastError();
std::string MGetLastErrorMsg();
MError MMakeError(int err);
std::string MGetErrorMsg(int err);
std::string MGetErrorMsg(MError err);

#endif
