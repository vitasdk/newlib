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
#include <string.h>

#include <psp2/types.h>
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

    int fd2 = __vita_duplicate_descriptor(fd);
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