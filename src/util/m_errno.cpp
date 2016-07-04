#include <mzx/util/m_errno.h>
#include <errno.h>

MError MGetLastError()
{
    switch (errno)
    {
    case 0:
        return MError::No;
    case EPERM:
        return MError::OperationNotPermitted;
    case ENOENT:
        return MError::NoSuchFileOrDirectory;
    case ESRCH:
        return MError::NoSuchProcess;
    case EINTR:
        return MError::InterruptedSystemCall;
    case EIO:
        return MError::IOError;
    case ENXIO:
        return MError::NoSuchDeviceOrAddress;
    case E2BIG:
        return MError::ArgumentListTooLong;
    case ENOEXEC:
        return MError::ExecFormatError;
    case EBADF:
        return MError::BadFileNumber;
    case ECHILD:
        return MError::NoChildProcesses;
    case EAGAIN:
        return MError::TryAgain;
    case ENOMEM:
        return MError::OutOfMemory;
    case EACCES:
        return MError::PermissionDenied;
    case EBUSY:
        return MError::DeviceOrResourceBusy;
    case EINVAL:
        return MError::InvalidArgument;
    case ENFILE:
        return MError::FileTableOverflow;
    case EMFILE:
        return MError::TooManyOpenFiles;
    case EWOULDBLOCK:
        return MError::TryAgain;
    case ENODATA:
        return MError::NoData;
    case ETIME:
        return MError::TimerExpired;
    case EADDRINUSE:
        return MError::AddressInUse;
    case ENOBUFS:
        return MError::NoBufferSpace;
    case ECONNREFUSED:
        return MError::ConnectionRefused;
    default:
        return MError::Unknown;
    }
}

std::string MGetErrorMsg(MError err)
{
    switch (err)
    {
    case MError::No:
        return "no error";
    case MError::OperationNotPermitted:
        return "operation not permitted";
    case MError::NoSuchFileOrDirectory:
        return "NoSuchFileOrDirectory";
    case MError::NoSuchProcess:
        return "NoSuchProcess";
    case MError::InterruptedSystemCall:
        return "InterruptedSystemCall";
    case MError::IOError:
        return "IOError";
    case ENXIO:
        return MError::NoSuchDeviceOrAddress;
    case E2BIG:
        return MError::ArgumentListTooLong;
    case ENOEXEC:
        return MError::ExecFormatError;
    case EBADF:
        return MError::BadFileNumber;
    case ECHILD:
        return MError::NoChildProcesses;
    case EAGAIN:
        return MError::TryAgain;
    case ENOMEM:
        return MError::OutOfMemory;
    case EACCES:
        return MError::PermissionDenied;
    case EBUSY:
        return MError::DeviceOrResourceBusy;
    case EINVAL:
        return MError::InvalidArgument;
    case ENFILE:
        return MError::FileTableOverflow;
    case EMFILE:
        return MError::TooManyOpenFiles;
    case EWOULDBLOCK:
        return MError::TryAgain;
    case ENODATA:
        return MError::NoData;
    case ETIME:
        return MError::TimerExpired;
    case EADDRINUSE:
        return MError::AddressInUse;
    case ENOBUFS:
        return MError::NoBufferSpace;
    case ECONNREFUSED:
        return MError::ConnectionRefused;
    default:
        return MError::Unknown;
    }
}
