/*

Copyright (C) 2016, David "Davee" Morgan

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

#include <sys/socket.h>
#include <errno.h>

#include <psp2/net/net.h>

#include "vitadescriptor.h"

#define SCE_ERRNO_MASK 0xFF

static inline int is_socket_valid(int s)
{
	return (is_fd_valid(s) && (__vita_fdmap[s]->type == VITA_DESCRIPTOR_SOCKET));
}

#ifdef F_accept
int accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetAccept(__vita_fdmap[s]->sce_uid, (SceNetSockaddr *)addr, (unsigned int *)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	int s2 =__vita_acquire_descriptor();

	if (s2 < 0)
	{
		errno = EMFILE;
		return -1;
	}

	__vita_fdmap[s2]->sce_uid = res;
	__vita_fdmap[s2]->type = VITA_DESCRIPTOR_SOCKET;
	return s2;
}
#endif

#ifdef F_bind
int	bind(int s, const struct sockaddr *addr, socklen_t addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetBind(__vita_fdmap[s]->sce_uid, (SceNetSockaddr *)addr, (unsigned int)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_connect
int	connect(int s, const struct sockaddr *addr, socklen_t addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetConnect(__vita_fdmap[s]->sce_uid, (SceNetSockaddr *)addr, (unsigned int)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_getpeername
int	getpeername(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetGetpeername(__vita_fdmap[s]->sce_uid, (SceNetSockaddr *)addr, (unsigned int *)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_getsockname
int	getsockname(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetGetsockname(__vita_fdmap[s]->sce_uid, (SceNetSockaddr *)addr, (unsigned int *)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_getsockopt
int	getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetGetsockopt(__vita_fdmap[s]->sce_uid, level, optname, optval, (unsigned int *)optlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_listen
int	listen(int s, int backlog)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetListen(__vita_fdmap[s]->sce_uid, backlog);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_recv
ssize_t	recv(int s, void *buf, size_t len, int flags)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetRecv(__vita_fdmap[s]->sce_uid, buf, len, flags);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return res;
}
#endif

#ifdef F_recvfrom
ssize_t	recvfrom(int s, void *buf, size_t len, int flags,
		struct sockaddr *src_addr, socklen_t *addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetRecvfrom(__vita_fdmap[s]->sce_uid, buf, len, flags, (SceNetSockaddr *)src_addr, (unsigned int *)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return res;
}
#endif

#ifdef F_recvmsg
ssize_t recvmsg(int s, struct msghdr *msg, int flags)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetRecvmsg(__vita_fdmap[s]->sce_uid, (SceNetMsghdr *)msg, flags);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return res;
}
#endif

#ifdef F_send
ssize_t	send(int s, const void *buf, size_t len, int flags)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetSend(__vita_fdmap[s]->sce_uid, buf, len, flags);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return res;

}
#endif

#ifdef F_sendto
ssize_t	sendto(int s, const void *buf,
		size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetSendto(__vita_fdmap[s]->sce_uid, buf, len, flags, (SceNetSockaddr *)dest_addr, (unsigned int)addrlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return res;
}
#endif

#ifdef F_sendmsg
ssize_t sendmsg(int s, const struct msghdr *msg, int flags)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetSendmsg(__vita_fdmap[s]->sce_uid, (const SceNetMsghdr *)msg, flags);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return res;
}
#endif

#ifdef F_setsockopt
int	setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetSetsockopt(__vita_fdmap[s]->sce_uid, level, optname, optval, optlen);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_shutdown
int	shutdown(int s, int how)
{
	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int res = sceNetShutdown(__vita_fdmap[s]->sce_uid, how);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}
#endif

#ifdef F_socket
int	socket(int domain, int type, int protocol)
{
	int res = sceNetSocket("", domain, type, protocol);

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	int s = __vita_acquire_descriptor();

	if (s < 0)
	{
		errno = EMFILE;
		return -1;
	}

	__vita_fdmap[s]->sce_uid = res;
	__vita_fdmap[s]->type = VITA_DESCRIPTOR_SOCKET;
	return s;
}

int __vita_glue_socket_close(int s)
{
	int res = 0;

	if (!is_socket_valid(s))
	{
		errno = EBADF;
		return -1;
	}

	int sce_uid = __vita_fdmap[s]->sce_uid;
	int ref_count = __vita_release_descriptor(s);

	if (ref_count < 0)
	{
		errno = EBADF;
		return -1;
	}

	if (ref_count == 0)
	{
		res = sceNetSocketClose(sce_uid);
	}

	if (res < 0)
	{
		errno = res & SCE_ERRNO_MASK;
		return -1;
	}

	return 0;
}

ssize_t __vita_glue_socket_recv(int s, void *buf, size_t len, int flags)
{
	return recv(s, buf, len, flags);
}

ssize_t	__vita_glue_socket_send(int s, const void *buf, size_t len, int flags)
{
	return send(s, buf, len, flags);
}
#endif
