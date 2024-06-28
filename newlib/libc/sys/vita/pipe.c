/*

Copyright (C) 2022, vitasdk

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

#include <reent.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>
#include <sys/socket.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <psp2/types.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>

#include "vitaerror.h"
#include "vitadescriptor.h"

#define MSGPIPE_MEMTYPE_USER_MAIN 0x40
#define MSGPIPE_THREAD_ATTR_PRIO (0x8 | 0x4)


int pipe(int pipefd[2])
{
    int ret;
    ret = sceKernelCreateMsgPipe("newlib pipe", MSGPIPE_MEMTYPE_USER_MAIN, MSGPIPE_THREAD_ATTR_PRIO, 4 * 4096, NULL);

    if (ret < 0)
    {
        errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
        return -1;
    }


    int fd = __vita_acquire_descriptor();

    if (fd < 0)
    {
        sceKernelDeleteMsgPipe(ret);
        errno = EMFILE;
        return -1;
    }

    __vita_fdmap[fd]->sce_uid = ret;
    __vita_fdmap[fd]->type = VITA_DESCRIPTOR_PIPE;

    int fd2 = __vita_duplicate_descriptor(fd, 0);
    if (fd2 < 0)
    {
        sceKernelDeleteMsgPipe(ret);
        errno = EMFILE;
        return -1;
    }

    pipefd[0] = fd;
    pipefd[1] = fd2;

    return 0;
}


int pipe2(int pipefd[2], int flags) {
    // FIXME this action is simulated async io using the socket.
    // this implementation has limitation that this cannot use on every environment
    // such as adhoc network mode
    // implementation have to be changed to async sce*MsgPiple
    //
    // https://github.com/vitasdk/newlib/pull/101
    if (socketpair(AF_INET, SOCK_STREAM, 0, pipefd) == -1) {
        return -1;
    }

    for (int i=0; i<2; i++) {
        // Free associated resources as soon as close is called
        SceNetLinger val = { .l_onoff = 1, .l_linger = 0, };
        if (setsockopt(pipefd[i], SOL_SOCKET, SO_LINGER, &val, sizeof(val)) == -1) {
            goto failure;
        }
    }

    if (flags & O_NONBLOCK) {
        for (int i=0; i<2; i++) {
            int val = 1;
            if (setsockopt(pipefd[i], SOL_SOCKET, SO_NONBLOCK, &val, sizeof(val)) == -1) {
                goto failure;
            }
        }
    }

    return 0;
failure: ;
    int save_errno = errno;
    close(pipefd[0]);
    close(pipefd[1]);
    errno = save_errno;
    return -1;
}
