#include <mzx/util/m_errno.h>
#include <errno.h>

MError MGetLastError()
{
    return MMakeError(errno);
}

std::string MGetLastErrorMsg()
{
    return MGetErrorMsg(errno);
}

MError MMakeError(int err)
{
    switch (err)
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

std::string MGetErrorMsg(int err)
{
    switch (err)
    {
    case 0:
        return "no error";
    case EPERM:
        return "operation not permitted";
    case ENOENT:
        return "no such file or directory";
    case ESRCH:
        return "no such process";
    case EINTR:
        return "interrupted system call";
    case EIO:
        return "io error";
    case ENXIO:
        return "no such device or address";
    case E2BIG:
        return "argument list too long";
    case ENOEXEC:
        return "exec format error";
    case EBADF:
        return "bad file number";
    case ECHILD:
        return "no child processes";
    case EAGAIN:
        return "try again";
    case ENOMEM:
        return "out of memory";
    case EACCES:
        return "permission denied";
    case EBUSY:
        return "device or resource busy";
    case EINVAL:
        return "invalid argument";
    case ENFILE:
        return "file table overflow";
    case EMFILE:
        return "too many open files";
    case ENODATA:
        return "no data";
    case ETIME:
        return "timer expired";
    case EADDRINUSE:
        return "address in use";
    case ENOBUFS:
        return "no buffer space";
    case ECONNREFUSED:
        return "connection refused";
    default:
        return "unknown error";
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
        return "no such file or directory";
    case MError::NoSuchProcess:
        return "no such process";
    case MError::InterruptedSystemCall:
        return "interrupted system call";
    case MError::IOError:
        return "io error";
    case MError::NoSuchDeviceOrAddress:
        return "no such device or address";
    case MError::ArgumentListTooLong:
        return "argument list too long";
    case MError::ExecFormatError:
        return "exec format error";
    case MError::BadFileNumber:
        return "bad file number";
    case MError::NoChildProcesses:
        return "no child processes";
    case MError::TryAgain:
        return "try again";
    case MError::OutOfMemory:
        return "out of memory";
    case MError::PermissionDenied:
        return "permission denied";
    case MError::DeviceOrResourceBusy:
        return "device or resource busy";
    case MError::InvalidArgument:
        return "invalid argument";
    case MError::FileTableOverflow:
        return "file table overflow";
    case MError::TooManyOpenFiles:
        return "too many open files";
    case MError::NoData:
        return "no data";
    case MError::TimerExpired:
        return "timer expired";
    case MError::AddressInUse:
        return "address in use";
    case MError::NoBufferSpace:
        return "no buffer space";
    case MError::ConnectionRefused:
        return "connection refused";
    default:
        return "unknown error";
    }
}
