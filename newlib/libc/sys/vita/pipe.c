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


#include <errno.h>
#include "vitadescriptor.h"

#include <psp2/kernel/threadmgr.h>

int pipe(int pipefd[2])
{
    SceUID pipeid = sceKernelCreateMsgPipe("", 0, 0, NULL, NULL);

    if (pipeid < 0)
    {
        errno = EFAULT;
        return -1;
    }

    int fd = __vita_acquire_descriptor();

    if (fd < 0)
    {
        sceKernelDeleteMsgPipe(pipeid);
        errno = EMFILE;
        return -1;
    }

    __vita_fdmap[fd]->sce_uid = pipeid;
    __vita_fdmap[fd]->type = VITA_DESCRIPTOR_PIPE;

    int fd2 = __vita_duplicate_descriptor(fd);

    if (fd2 < 0)
    {
        __vita_release_descriptor(fd);
        errno = EMFILE;
        return -1;
    }

    pipefd[0] = fd;
    pipefd[1] = fd2;
    return 0;
}

int __vita_glue_pipe_write(SceUID pipeid, const void *buf, size_t size)
{
    return sceKernelSendMsgPipe(pipeid, buf, size, 0, NULL, NULL);
}

int __vita_glue_pipe_read(SceUID pipeid, void *ptr, size_t len)
{
    return sceKernelReceiveMsgPipe(pipeid, ptr, len, 0, NULL, NULL);
}

int __vita_glue_pipe_close(SceUID pipeid)
{
    return sceKernelDeleteMsgPipe(pipeid);
}
