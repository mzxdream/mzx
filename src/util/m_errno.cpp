#include <mzx/util/m_errno.h>

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
    case EINPROGRESS:
        return MError::InProgress;
    case ECONNRESET:
        return MError::ConnectReset;
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
    case EINPROGRESS:
        return "operation now in progress";
    case ECONNRESET:
        return "connect reset by peer";
    default:
        return "unknown error";
    }
}

std::string MGetErrorMsg(MError err)
{
    return MGetErrorMsg(statice_cast<int>(err));
}
