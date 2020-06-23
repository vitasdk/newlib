/*

Copyright (C) 2017, David "Davee" Morgan
Copyright (C) 2018, Sunguk Lee

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/
#include <errno.h>

#include <psp2/net/net.h>

#include "vitaerror.h"

int __vita_sce_errno_to_errno(int sce_errno)
{
	switch (sce_errno)
	{
		case SCE_NET_ERROR_EPERM:
			return EPERM;
		case SCE_NET_ERROR_ENOENT:
			return ENOENT;
		case SCE_NET_ERROR_ESRCH:
			return ESRCH;
		case SCE_NET_ERROR_EINTR:
			return EINTR;
		case SCE_NET_ERROR_EIO:
			return EIO;
		case SCE_NET_ERROR_ENXIO:
			return ENXIO;
		case SCE_NET_ERROR_E2BIG:
			return E2BIG;
		case SCE_NET_ERROR_ENOEXEC:
			return ENOEXEC;
		case SCE_NET_ERROR_EBADF:
			return EBADF;
		case SCE_NET_ERROR_ECHILD:
			return ECHILD;
		case SCE_NET_ERROR_EDEADLK:
			return EDEADLK;
		case SCE_NET_ERROR_ENOMEM:
			return ENOMEM;
		case SCE_NET_ERROR_EACCES:
			return EACCES;
		case SCE_NET_ERROR_EFAULT:
			return EFAULT;
		case SCE_NET_ERROR_ENOTBLK:
			return ENOTBLK;
		case SCE_NET_ERROR_EBUSY:
			return EBUSY;
		case SCE_NET_ERROR_EEXIST:
			return EEXIST;
		case SCE_NET_ERROR_EXDEV:
			return EXDEV;
		case SCE_NET_ERROR_ENODEV:
			return ENODEV;
		case SCE_NET_ERROR_ENOTDIR:
			return ENOTDIR;
		case SCE_NET_ERROR_EISDIR:
			return EISDIR;
		case SCE_NET_ERROR_EINVAL:
			return EINVAL;
		case SCE_NET_ERROR_ENFILE:
			return ENFILE;
		case SCE_NET_ERROR_EMFILE:
			return EMFILE;
		case SCE_NET_ERROR_ENOTTY:
			return ENOTTY;
		case SCE_NET_ERROR_ETXTBSY:
			return ETXTBSY;
		case SCE_NET_ERROR_EFBIG:
			return EFBIG;
		case SCE_NET_ERROR_ENOSPC:
			return ENOSPC;
		case SCE_NET_ERROR_ESPIPE:
			return ESPIPE;
		case SCE_NET_ERROR_EROFS:
			return EROFS;
		case SCE_NET_ERROR_EMLINK:
			return EMLINK;
		case SCE_NET_ERROR_EPIPE:
			return EPIPE;
		case SCE_NET_ERROR_EDOM:
			return EDOM;
		case SCE_NET_ERROR_ERANGE:
			return ERANGE;
		case SCE_NET_ERROR_EAGAIN:
			return EAGAIN;
// these values are the same as EAGAIN
//		case SCE_NET_ERROR_EWOULDBLOCK:
//			return EWOULDBLOCK;
		case SCE_NET_ERROR_EINPROGRESS:
			return EINPROGRESS;
		case SCE_NET_ERROR_EALREADY:
			return EALREADY;
		case SCE_NET_ERROR_ENOTSOCK:
			return ENOTSOCK;
		case SCE_NET_ERROR_EDESTADDRREQ:
			return EDESTADDRREQ;
		case SCE_NET_ERROR_EMSGSIZE:
			return EMSGSIZE;
		case SCE_NET_ERROR_EPROTOTYPE:
			return EPROTOTYPE;
		case SCE_NET_ERROR_ENOPROTOOPT:
			return ENOPROTOOPT;
		case SCE_NET_ERROR_EPROTONOSUPPORT:
			return EPROTONOSUPPORT;
		case SCE_NET_ERROR_ESOCKTNOSUPPORT:
			return ESOCKTNOSUPPORT;
		case SCE_NET_ERROR_EOPNOTSUPP:
			return EOPNOTSUPP;
		case SCE_NET_ERROR_EPFNOSUPPORT:
			return EPFNOSUPPORT;
		case SCE_NET_ERROR_EAFNOSUPPORT:
			return EAFNOSUPPORT;
		case SCE_NET_ERROR_EADDRINUSE:
			return EADDRINUSE;
		case SCE_NET_ERROR_EADDRNOTAVAIL:
			return EADDRNOTAVAIL;
		case SCE_NET_ERROR_ENETDOWN:
			return ENETDOWN;
		case SCE_NET_ERROR_ENETUNREACH:
			return ENETUNREACH;
		case SCE_NET_ERROR_ENETRESET:
			return ENETRESET;
		case SCE_NET_ERROR_ECONNABORTED:
			return ECONNABORTED;
		case SCE_NET_ERROR_ECONNRESET:
			return ECONNRESET;
		case SCE_NET_ERROR_ENOBUFS:
			return ENOBUFS;
		case SCE_NET_ERROR_EISCONN:
			return EISCONN;
		case SCE_NET_ERROR_ENOTCONN:
			return ENOTCONN;
		case SCE_NET_ERROR_ESHUTDOWN:
			return ESHUTDOWN;
		case SCE_NET_ERROR_ETOOMANYREFS:
			return ETOOMANYREFS;
		case SCE_NET_ERROR_ETIMEDOUT:
			return ETIMEDOUT;
		case SCE_NET_ERROR_ECONNREFUSED:
			return ECONNREFUSED;
		case SCE_NET_ERROR_ELOOP:
			return ELOOP;
		case SCE_NET_ERROR_ENAMETOOLONG:
			return ENAMETOOLONG;
		case SCE_NET_ERROR_EHOSTDOWN:
			return EHOSTDOWN;
		case SCE_NET_ERROR_EHOSTUNREACH:
			return EHOSTUNREACH;
		case SCE_NET_ERROR_ENOTEMPTY:
			return ENOTEMPTY;
		case SCE_NET_ERROR_EUSERS:
			return EUSERS;
		case SCE_NET_ERROR_EDQUOT:
			return EDQUOT;
		case SCE_NET_ERROR_ESTALE:
			return ESTALE;
		case SCE_NET_ERROR_EREMOTE:
			return EREMOTE;
		case SCE_NET_ERROR_ENOLCK:
			return ENOLCK;
		case SCE_NET_ERROR_ENOSYS:
			return ENOSYS;
		case SCE_NET_ERROR_EIDRM:
			return EIDRM;
		case SCE_NET_ERROR_EOVERFLOW:
			return EOVERFLOW;
		case SCE_NET_ERROR_EILSEQ:
			return EILSEQ;
		case SCE_NET_ERROR_ENOTSUP:
			return ENOTSUP;
		case SCE_NET_ERROR_ECANCELED:
			return ECANCELED;
		case SCE_NET_ERROR_EBADMSG:
			return EBADMSG;
		case SCE_NET_ERROR_ENODATA:
			return ENODATA;
		case SCE_NET_ERROR_ENOSR:
			return ENOSR;
		case SCE_NET_ERROR_ENOSTR:
			return ENOSTR;
		case SCE_NET_ERROR_ETIME:
			return ETIME;
		default:
			return sce_errno;
	}
}
