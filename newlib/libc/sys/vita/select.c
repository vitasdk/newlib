/*

Copyright (c) 2016, 2017, 2018 vitasdk

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
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"

#define SCE_ERRNO_MASK 0xFF

#define MAX_EVENTS 255

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
		   struct timeval *timeout)
{
	if (nfds < 0 || nfds >= MAX_OPEN_FILES) {
		errno = EINVAL;
		return -1;
	}

	uint64_t wait = timeout->tv_sec * 1000000 + timeout->tv_usec;

	if (nfds == 0) {
		sceKernelDelayThread(wait);
		return 0;
	}

	int i;

	int eid = sceNetEpollCreate("", 0);
	if (eid < 0) {
		errno = eid & SCE_ERRNO_MASK;
		return -1;
	}

	DescriptorTranslation *lock[MAX_OPEN_FILES] = {0};

	for (i = 0; i < nfds; i++) {
		SceNetEpollEvent ev = {0};
		ev.data.fd = i;
		if (readfds && FD_ISSET(i, readfds)) {
			ev.events |= SCE_NET_EPOLLIN;
		}
		if (writefds && FD_ISSET(i, writefds)) {
			ev.events |= SCE_NET_EPOLLOUT;
		}
		if (exceptfds && FD_ISSET(i, exceptfds)) {
			ev.events |= SCE_NET_EPOLLERR;
		}
		if (ev.events == 0) {
			continue;
		}

		DescriptorTranslation *fdmap = __vita_fd_grab(i);
		if (fdmap == NULL) {
			continue;
		}
		if (fdmap->type != VITA_DESCRIPTOR_SOCKET) {
			__vita_fd_drop(fdmap);
			continue;
		}
		lock[i] = fdmap;

		sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, fdmap->sce_uid, &ev);
	}

	SceNetEpollEvent events[MAX_EVENTS] = {0};
	int nev = sceNetEpollWait(eid, events, MAX_EVENTS, wait);
	int res = 0;

	if (nev < 0) {
		errno = nev & SCE_ERRNO_MASK;
		res = -1;
		goto exit;
	}

	if (readfds) {
		FD_ZERO(readfds);
	}
	if (writefds) {
		FD_ZERO(writefds);
	}
	if (exceptfds) {
		FD_ZERO(exceptfds);
	}

	for (i = 0; i < nev; i++) {
		if (events[i].events) {
			if (events[i].events & SCE_NET_EPOLLIN) {
				FD_SET(events[i].data.fd, readfds);
				res++;
			}
			if (events[i].events & SCE_NET_EPOLLOUT) {
				FD_SET(events[i].data.fd, writefds);
				res++;
			}
			if (events[i].events & SCE_NET_EPOLLERR) {
				FD_SET(events[i].data.fd, exceptfds);
				res++;
			}
		}
	}

exit:
	sceNetEpollDestroy(eid);

	for (i = 0; i < nfds; i++) {
		if (lock[i] != NULL) {
			__vita_fd_drop(lock[i]);
		}
	}

	return res;
}
