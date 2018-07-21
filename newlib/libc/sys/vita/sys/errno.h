/* errno is not a global variable, because that would make using it
   non-reentrant.  Instead, its address is returned by the function
   __errno.  */

#ifndef _SYS_ERRNO_H_
#ifdef __cplusplus
extern "C" {
#endif
#define _SYS_ERRNO_H_

#include <sys/reent.h>

#ifndef _REENT_ONLY
#define errno (*__errno())
extern int *__errno _PARAMS ((void));
#endif

/* Please don't use these variables directly.
   Use strerror instead. */
extern __IMPORT _CONST char * _CONST _sys_errlist[];
extern __IMPORT int _sys_nerr;

#define __errno_r(ptr) ((ptr)->_errno)

#define EPERM           1   /* Not owner */
#define ENOENT          2   /* No such file or directory */
#define ESRCH           3   /* No such process */
#define EINTR           4   /* Interrupted system call */
#define EIO             5   /* I/O error */
#define ENXIO           6   /* No such device or address */
#define E2BIG           7   /* Arg list too long */
#define ENOEXEC         8   /* Exec format error */
#define EBADF           9   /* Bad file number */
#define ECHILD          10  /* No children */
#define EAGAIN          35  /* No more processes */
#define ENOMEM          12  /* Not enough space */
#define EACCES          13  /* Permission denied */
#define EFAULT          14  /* Bad address */
#define ENOTBLK         15  /* Block device required */
#define EBUSY           16  /* Device or resource busy */
#define EEXIST          17  /* File exists */
#define EXDEV           18  /* Cross-device link */
#define ENODEV          19  /* No such device */
#define ENOTDIR         20  /* Not a directory */
#define EISDIR          21  /* Is a directory */
#define EINVAL          22  /* Invalid argument */
#define ENFILE          23  /* Too many open files in system */
#define EMFILE          24  /* File descriptor value too large */
#define ENOTTY          25  /* Not a character device */
#define ETXTBSY         26  /* Text file busy */
#define EFBIG           27  /* File too large */
#define ENOSPC          28  /* No space left on device */
#define ESPIPE          29  /* Illegal seek */
#define EROFS           30  /* Read-only file system */
#define EMLINK          31  /* Too many links */
#define EPIPE           32  /* Broken pipe */
#define EDOM            33  /* Mathematics argument out of domain of function */
#define ERANGE          34  /* Result too large */
#define ENOMSG          83  /* No message of desired type */
#define EIDRM           82  /* Identifier removed */
#define EDEADLK         11  /* Deadlock */
#define ENOLCK          77  /* No lock */
#define ENOSTR          91  /* Not a stream */
#define ENODATA         89  /* No data (for no delay io) */
#define ETIME           92  /* Stream ioctl timeout */
#define ENOSR           90  /* No stream resources */
#define EREMOTE         71  /* The object is remote */
// TODO: check value
#define ENOLINK         91  /* Virtual circuit is gone */
// TODO: check value
#define EPROTO          92  /* Protocol error */
// TODO: check value
#define EMULTIHOP       90  /* Multihop attempted */
#define EBADMSG         88  /* Bad message */
#define EFTYPE          79  /* Inappropriate file type or format */
#define ENOSYS          78  /* Function not implemented */
#define ENOTEMPTY       66  /* Directory not empty */
// TODO: check value
#define ENAMETOOLONG    63  /* File or path name too long */
// TODO: check value
#define ELOOP           62  /* Too many symbolic links */
#define EOPNOTSUPP      45  /* Operation not supported on socket */
#define EPFNOSUPPORT    46  /* Protocol family not supported */
#define ECONNRESET      54  /* Connection reset by peer */
#define ENOBUFS         55  /* No buffer space available */
#define EAFNOSUPPORT    47  /* Address family not supported by protocol family */
#define EPROTOTYPE      41  /* Protocol wrong type for socket */
#define ENOTSOCK        38  /* Socket operation on non-socket */
#define ENOPROTOOPT     42  /* Protocol not available */
#define ESHUTDOWN       58  /* Can't send after socket shutdown */
#define ECONNREFUSED    61  /* Connection refused */
#define EADDRINUSE      48  /* Address already in use */
#define ECONNABORTED    53  /* Software caused connection abort */
#define ENETUNREACH     51  /* Network is unreachable */
#define ENETDOWN        50  /* Network interface is not configured */
#define ETIMEDOUT       60  /* Connection timed out */
#define EHOSTDOWN       64  /* Host is down */
#define EHOSTUNREACH    65  /* Host is unreachable */
#define EINPROGRESS     36  /* Connection already in progress */
#define EALREADY        37  /* Socket already connected */
#define EDESTADDRREQ    39  /* Destination address required */
#define EMSGSIZE        40  /* Message too long */
#define EPROTONOSUPPORT 43  /* Unknown protocol */
#define ESOCKTNOSUPPORT 44  /* Socket type not supported */
#define EADDRNOTAVAIL   49  /* Address not available */
#define ENETRESET       52  /* Connection aborted by network */
#define EISCONN         56  /* Socket is already connected */
#define ENOTCONN        57  /* Socket is not connected */
#define ETOOMANYREFS    59
#define EPROCLIM        67
#define EUSERS          68
#define EDQUOT          69
#define ESTALE          70
#define ENOTSUP         86  /* Not supported */
#define EILSEQ          85  /* Illegal byte sequence */
#define EOVERFLOW       84  /* Value too large for defined data type */
#define ECANCELED       87  /* Operation canceled */
// TODO: check value
#define ENOTRECOVERABLE 95  /* State not recoverable */
// TODO: check value
#define EOWNERDEAD      96  /* Previous owner died */

#define EBADRPC         72
#define ERPCMISMATCH    73
#define EPROGUNAVAIL    74
#define EPROGMISMATCH   75
#define EPROCUNAVAIL    76
#define EAUTH           80
#define ENEEDAUTH       81
#define EADHOC          160
#define EDISABLEDIF     161
#define ERESUME         162
#define EWOULDBLOCK     EAGAIN /* Operation would block */

#define __ELASTERROR    2000 /* Users can add values starting here */

#ifdef __cplusplus
}
#endif
#endif /* _SYS_ERRNO_H */
