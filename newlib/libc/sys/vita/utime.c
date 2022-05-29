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

#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <psp2/io/stat.h> 
#include <psp2/rtc.h>

#include "vitaerror.h"
#include "vitadescriptor.h"
#include "vitafs.h"

#define TRY(x) {rid=x; if (rid < 0) {errno = __vita_sce_errno_to_errno(rid, ERROR_GENERIC); return -1;}}

int
utimes (const char *filename, const struct timeval times[2])
{
    struct SceIoStat stat = {0};
    char* full_path;
    int rid;

    full_path = __realpath(filename);

    if(!full_path)
    {
        // errno is set by __realpath
        return -1;
    }

    int ret = sceIoGetstat(full_path, &stat);
    if (ret < 0)
    {
        free(full_path);
        errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
        return -1;
    }

    if (times) {
        TRY(sceRtcConvertTime_tToDateTime(times[0].tv_sec, &stat.st_atime));
        TRY(sceRtcConvertTime_tToDateTime(times[1].tv_sec, &stat.st_mtime));
        stat.st_atime.microsecond = times[0].tv_usec;
        stat.st_mtime.microsecond = times[1].tv_usec;
    }
    else {
        TRY(sceRtcGetCurrentClockUtc(&stat.st_atime));
        stat.st_mtime = stat.st_atime;
    }
    
    ret = sceIoChstat(full_path, &stat, SCE_CST_AT | SCE_CST_MT);
    if (ret < 0)
    {
        free(full_path);
        errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
        return -1;
    }
    free(full_path);
    return 0;
}

int
utime (const char *filename, const struct utimbuf *buf)
{
    if (buf != NULL)
    {
        struct timeval times[2];

        times[0].tv_sec = buf->actime;
        times[0].tv_usec = 0;
        times[1].tv_sec = buf->modtime;
        times[1].tv_usec = 0;
        return utimes (filename, times);
    }

    return utimes(filename, (struct timeval *) 0);
}

int 
futimens (int fd, const struct timespec times[2])
{
    struct SceIoStat stat = {0};
    unsigned int bits = 0;
    int ret = 0;
    int rid;

    DescriptorTranslation *fdmap = __vita_fd_grab(fd);

    if (!fdmap)
    {
        errno = EBADF;
        return -1;
    }

    switch (fdmap->type)
    {
        case VITA_DESCRIPTOR_TTY:
        case VITA_DESCRIPTOR_FILE:
        case VITA_DESCRIPTOR_DIRECTORY:
            ret = sceIoGetstatByFd(fdmap->sce_uid, &stat);
            break;
        case VITA_DESCRIPTOR_SOCKET:
        case VITA_DESCRIPTOR_PIPE:
            ret = __vita_make_sce_errno(EBADF);
            break;
    }

    if (ret < 0)
    {
        __vita_fd_drop(fdmap);
        errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
        return -1;
    }

    if (times) {
        SceDateTime dt[2];
        for (int i = 0; i < 2; i++) {
            if ((times[i].tv_nsec != UTIME_NOW) && (times[i].tv_nsec != UTIME_OMIT)) {
                TRY(sceRtcConvertTime_tToDateTime(times[i].tv_sec, &dt[i]));
                dt[i].microsecond = times[i].tv_nsec / 1000;
            } 
            else {
                TRY(sceRtcGetCurrentClockUtc(&dt[i]));
            }
        }
        stat.st_atime = dt[0];
        stat.st_mtime = dt[1];
    }
    else {
        TRY(sceRtcGetCurrentClockUtc(&stat.st_atime));
        stat.st_mtime = stat.st_atime;
    }

    if (!times || times[0].tv_nsec != UTIME_OMIT)
        bits |= SCE_CST_AT;
    if (!times || times[1].tv_nsec != UTIME_OMIT)
        bits |= SCE_CST_MT;

    ret = sceIoChstatByFd(fdmap->sce_uid, &stat, bits);

    __vita_fd_drop(fdmap);

    if (ret < 0)
    {
        errno = __vita_sce_errno_to_errno(ret, ERROR_GENERIC);
        return -1;
    }

    return 0;
}
