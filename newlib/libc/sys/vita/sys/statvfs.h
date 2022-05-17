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
   fsblkcnt_t	    f_blocks;
   fsblkcnt_t	    f_bfree;
   fsblkcnt_t	    f_bavail;
   fsfilcnt_t	    f_files;
   fsfilcnt_t	    f_ffree;
   fsfilcnt_t	    f_favail;
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
