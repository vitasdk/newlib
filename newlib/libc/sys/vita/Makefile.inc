libc_a_SOURCES += \
        %D%/syscalls.c \
        %D%/access.c \
        %D%/sbrk.c \
        %D%/threading.c \
        %D%/mlock.c \
        %D%/io.c \
        %D%/socket.c \
        %D%/dup.c \
        %D%/select.c \
        %D%/error.c \
        %D%/fcntl.c \
        %D%/dirent.c \
        %D%/lcltime_r.c \
        %D%/sleep.c \
        %D%/usleep.c \
        %D%/truncate.c \
        %D%/fs.c \
        %D%/clock.c \
        %D%/pread.c \
        %D%/pathconf.c \
        %D%/fpathconf.c \
        %D%/poll.c \
        %D%/monetary.c \
        %D%/uname.c \
        %D%/getentropy.c \
        %D%/basename.c \
        %D%/dirname.c \
        %D%/resource.c \
        %D%/sysconf.c \
        %D%/ids.c \
        %D%/secure_getenv.c \
        %D%/pipe.c \
        %D%/utime.c \
        %D%/chroot.c \
        %D%/groups.c \
        %D%/net/gethostbyaddr.c \
        %D%/net/gethostbyname.c \
        %D%/net/getaddrinfo.c \
        %D%/net/freeaddrinfo.c \
        %D%/net/getservbyname.c \
        %D%/net/inet_ntop.c \
        %D%/net/inet_ntoa.c \
        %D%/net/inet_netof.c \
        %D%/net/inet_pton.c \
        %D%/net/inet_lnaof.c \
        %D%/net/inet_addr.c \
        %D%/net/inet_network.c \
        %D%/net/inet_net_ntop.c \
        %D%/net/inet_aton.c \
        %D%/net/inet_net_pton.c \
        %D%/net/inet_makeaddr.c \
        %D%/net/gethostname.c \
        %D%/net/getnameinfo.c \
        %D%/vitanet.c

libc_a_CPPFLAGS_%C%_socket.c = \
        -DF_accept -DF_bind -DF_connect -DF_getpeername \
        -DF_getsockname -DF_getsockopt -DF_listen -DF_recv \
        -DF_recvfrom -DF_recvmsg -DF_send -DF_sendto -DF_sendmsg \
        -DF_setsockopt -DF_shutdown -DF_socket
libc_a_CPPFLAGS_%C%_dirent.c = \
        -DF_closedir -DF_opendir -DF_readdir -DF_readdir_r \
        -DF_rewinddir -DF_scandir -DF_seekdir -DF_telldir -DF_dirfd