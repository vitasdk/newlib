/*

Copyright (c) 2021 vitasdk

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

#include <sys/poll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/types.h>

#include "vitadescriptor.h"
#include "vitaerror.h"

// max events = max pollfd elements = max open files
#define MAX_EVENTS MAX_OPEN_FILES

int poll(struct pollfd _pfd[], nfds_t _nfds, int _timeout)
{
	if (_nfds >= MAX_OPEN_FILES)
	{
		errno = EINVAL;
		return -1;
	}

	uint64_t wait = _timeout * 1000;

	if (_nfds == 0)
	{
		sceKernelDelayThread(wait);
		return 0;
	}

	int i;

	int eid = sceNetEpollCreate("poll", 0);
	if (eid < 0) {
		errno = __vita_scenet_errno_to_errno(eid);
		return -1;
	}

	DescriptorTranslation *lock[MAX_OPEN_FILES] = {0};
	int res = 0;


	for (i = 0; i < _nfds; i++)
	{
		SceNetEpollEvent ev = {0};

		_pfd[i].revents = 0;

		if (_pfd[i].fd < 0)
		{
			continue;
		}

		ev.data.fd = i;

		if (_pfd[i].events & POLLIN)
		{
			ev.events |= SCE_NET_EPOLLIN;
		}
		if (_pfd[i].events & POLLOUT)
		{
			ev.events |= SCE_NET_EPOLLOUT;
		}
		if (ev.events == 0)
		{
			continue;
		}

		DescriptorTranslation *fdmap = __vita_fd_grab(_pfd[i].fd);
		if (fdmap == NULL)
		{
			continue;
		}
		if (fdmap->type != VITA_DESCRIPTOR_SOCKET)
		{
			if (fdmap->type == VITA_DESCRIPTOR_FILE)
			{
				// regular files always poll true for in/out
				if (_pfd[i].events & POLLIN)
					_pfd[i].revents | POLLIN;
				if (_pfd[i].events & POLLOUT)
					_pfd[i].revents | POLLOUT;
				if ((_pfd[i].events & POLLIN) || (_pfd[i].events & POLLOUT))
					res++;
			}
			__vita_fd_drop(fdmap);
			continue;
		}
		lock[i] = fdmap;

		sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, fdmap->sce_uid, &ev);
	}

	SceNetEpollEvent events[MAX_EVENTS] = {0};
	int nev = sceNetEpollWait(eid, events, MAX_EVENTS, wait);

	if (nev < 0)
	{
		errno = __vita_scenet_errno_to_errno(nev);
		res = -1;
		goto exit;
	}

	// append number of socket fds with events to number of regular file fds
	res += nev;

	for (i = 0; i < nev; i++)
	{
		if (events[i].events)
		{
			if (events[i].events & SCE_NET_EPOLLIN)
			{
				_pfd[events[i].data.fd].revents |= POLLIN;
			}
			if (events[i].events & SCE_NET_EPOLLOUT)
			{
				_pfd[events[i].data.fd].revents |= POLLOUT;
			}
			if (events[i].events & SCE_NET_EPOLLERR)
			{
				_pfd[events[i].data.fd].revents |= POLLERR;
			}
			if (events[i].events & SCE_NET_EPOLLHUP)
			{
				_pfd[events[i].data.fd].revents |= POLLHUP;
			}
		}
	}

exit:
	sceNetEpollDestroy(eid);

	for (i = 0; i < _nfds; i++)
	{
		if (lock[i] != NULL)
		{
			__vita_fd_drop(lock[i]);
		}
	}

	return res;
}
