#ifndef _M_ERRNO_H_
#define _M_ERRNO_H_

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

#endif
