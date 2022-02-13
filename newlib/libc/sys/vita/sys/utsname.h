#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

#define __UTSNAMELEN 65 // Same as on linux

// Pure-POSIX version (without domainname)
struct utsname {
    char sysname[__UTSNAMELEN];
    char nodename[__UTSNAMELEN];
    char release[__UTSNAMELEN];
    char version[__UTSNAMELEN];
    char machine[__UTSNAMELEN];
};


int uname(struct utsname *name);

#endif
