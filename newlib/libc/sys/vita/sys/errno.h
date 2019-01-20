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

#define EPERM		1	/* Not owner */
#define ENOENT		2	/* No such file or directory */
#define ESRCH		3	/* No such process */
#define EINTR		4	/* Interrupted system call */
#define EIO			5	/* I/O error */
#define ENXIO		6	/* No such device or address */
#define E2BIG		7	/* Arg list too long */
#define ENOEXEC		8	/* Exec format error */
#define EBADF		9	/* Bad file number */
#define ECHILD		10	/* No children */
#define EAGAIN		11	/* No more processes */
#define ENOMEM		12	/* Not enough space */
#define EACCES		13	/* Permission denied */
#define EFAULT		14	/* Bad address */
#define ENOTBLK		15	/* Block device required */
#define EBUSY		16	/* Device or resource busy */
#define EEXIST		17	/* File exists */
#define EXDEV		18	/* Cross-device link */
#define ENODEV		19	/* No such device */
#define ENOTDIR		20	/* Not a directory */
#define EISDIR		21	/* Is a directory */
#define EINVAL		22	/* Invalid argument */
#define ENFILE		23	/* Too many open files in system */
#define EMFILE		24	/* File descriptor value too large */
#define ENOTTY		25	/* Not a character device */
#define ETXTBSY		26	/* Text file busy */
#define EFBIG		27	/* File too large */
#define ENOSPC		28	/* No space left on device */
#define ESPIPE		29	/* Illegal seek */
#define EROFS		30	/* Read-only file system */
#define EMLINK		31	/* Too many links */
#define EPIPE		32	/* Broken pipe */
#define EDOM		33	/* Mathematics argument out of domain of function */
#define ERANGE		34	/* Result too large */
#define ENOMSG		35	/* No message of desired type */
#define EIDRM		36	/* Identifier removed */
#define EDEADLK		45	/* deadlock */
#define ENOLCK		46	/* no lock */
#define ENOSTR		60	/* Not a stream */
#define ENODATA		61	/* No data (for no delay io) */
#define ETIME		62	/* Stream ioctl timeout */
#define ENOSR		63	/* No stream resources */
#define EREMOTE		66	/* The object is remote */
#define ENOLINK		67	/* Virtual circuit is gone */
#define EPROTO		71	/* Protocol error */
#define EMULTIHOP	74	/* Multihop attempted */
#define EBADMSG		77	/* Bad message */
#define EFTYPE		79	/* Inappropriate file type or format */
#define ENOSYS		88	/* Function not implemented */
#define ENOTEMPTY	90	/* Directory not empty */
#define ENAMETOOLONG	91	/* File or path name too long */
#define ELOOP		92	/* Too many symbolic links */
#define EOPNOTSUPP	95	/* Operation not supported on socket */
#define EPFNOSUPPORT	96	/* Protocol family not supported */
#define ECONNRESET	104	/* Connection reset by peer */
#define ENOBUFS		105	/* No buffer space available */
#define EAFNOSUPPORT	106	/* Address family not supported by protocol family */
#define EPROTOTYPE	107	/* Protocol wrong type for socket */
#define ENOTSOCK	108	/* Socket operation on non-socket */
#define ENOPROTOOPT	109	/* Protocol not available */
#define ESHUTDOWN	110	/* Can't send after socket shutdown */
#define ECONNREFUSED	111	/* Connection refused */
#define EADDRINUSE	112	/* Address already in use */
#define ECONNABORTED	113	/* Software caused connection abort */
#define ENETUNREACH	114	/* Network is unreachable */
#define ENETDOWN	115	/* Network interface is not configured */
#define ETIMEDOUT	116	/* Connection timed out */
#define EHOSTDOWN	117	/* Host is down */
#define EHOSTUNREACH	118	/* Host is unreachable */
#define EINPROGRESS	119	/* Connection already in progress */
#define EALREADY	120	/* Socket already connected */
#define EDESTADDRREQ	121	/* Destination address required */
#define EMSGSIZE	122	/* Message too long */
#define EPROTONOSUPPORT	123	/* Unknown protocol */
#define ESOCKTNOSUPPORT	124	/* Socket type not supported */
#define EADDRNOTAVAIL	125	/* Address not available */
#define ENETRESET	126	/* Connection aborted by network */
#define EISCONN		127	/* Socket is already connected */
#define ENOTCONN	128	/* Socket is not connected */
#define ETOOMANYREFS	129
#define EUSERS		131
#define EDQUOT		132
#define ESTALE		133
#define ENOTSUP		134	/* Not supported */
#define EILSEQ		138	/* Illegal byte sequence */
#define EOVERFLOW	139	/* Value too large for defined data type */
#define ECANCELED	140	/* Operation canceled */
#define ENOTRECOVERABLE	141	/* State not recoverable */
#define EOWNERDEAD	142	/* Previous owner died */
#define EWOULDBLOCK	EAGAIN	/* Operation would block */

#define __ELASTERROR	2000	/* Users can add values starting here */

#ifdef __cplusplus
}
#endif
#endif /* _SYS_ERRNO_H */
