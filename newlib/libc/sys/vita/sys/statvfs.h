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

#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#include <sys/cdefs.h>
#include <sys/types.h>

#define ST_RDONLY      0x0001
#define ST_NOSUID      0x0002
#define ST_NODEV       0x0004
#define ST_NOEXEC      0x0008
#define ST_SYNCHRONOUS 0x0010
#define ST_MANDLOCK    0x0040
#define ST_NOATIME     0x0400
#define ST_NODIRATIME  0x0800
#define ST_RELATIME    0x1000

struct statvfs {
   unsigned long    f_bsize;
   unsigned long    f_frsize;
   fsblkcnt_t       f_blocks;
   fsblkcnt_t       f_bfree;
   fsblkcnt_t       f_bavail;
   fsfilcnt_t       f_files;
   fsfilcnt_t       f_ffree;
   fsfilcnt_t       f_favail;
   unsigned long    f_fsid;
   unsigned long    f_flag;
   unsigned long    f_namemax;
};

__BEGIN_DECLS

int statvfs(const char *__path, struct statvfs *__buf);
int fstatvfs(int __fd, struct statvfs *__buf);

#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
#define statvfs64 statvfs
#define fstatvfs64 fstatvfs
#endif

__END_DECLS

#endif
