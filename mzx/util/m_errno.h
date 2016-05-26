#ifndef _M_ERRNO_H_
#define _M_ERRNO_H_

#include <errno.h>

enum class MError
{
    No,
    Unknown,
    ConnectRefused,
    Disconnect,
    NotSupport,
    InProgress,
    Again,
    INTR,
    Invalid,
    NoBufs,
};

#endif
