## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = cygnus

INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)

AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

SOCKET_OBJS = accept.o bind.o connect.o getpeername.o getsockname.o getsockopt.o listen.o recv.o recvfrom.o recvmsg.o send.o sendto.o sendmsg.o setsockopt.o shutdown.o socket.o socketpair.o
DIRENT_OBJS = dirfd.o closedir.o opendir.o readdir.o readdir_r.o rewinddir.o scandir.o seekdir.o telldir.o 

NET_SOURCES = net/gethostbyaddr.c \
	net/gethostbyname.c \
	net/getaddrinfo.c \
	net/freeaddrinfo.c \
	net/getservbyname.c \
	net/inet_ntop.c \
	net/inet_ntoa.c \
	net/inet_netof.c \
	net/inet_pton.c \
	net/inet_lnaof.c \
	net/inet_addr.c \
	net/inet_network.c \
	net/inet_net_ntop.c \
	net/inet_aton.c \
	net/inet_net_pton.c \
	net/inet_makeaddr.c \
	net/gethostname.c \
	vitanet.c

STUB_SOURCES = chroot.c groups.c

lib_a_SOURCES = syscalls.c access.c sbrk.c threading.c mlock.c io.c socket.c dup.c select.c error.c fcntl.c dirent.c \
	lcltime_r.c sleep.c usleep.c truncate.c fs.c clock.c pread.c pathconf.c fpathconf.c poll.c monetary.c uname.c getentropy.c \
	basename.c dirname.c resource.c sysconf.c ids.c secure_getenv.c pipe.c utime.c\
	${STUB_SOURCES} ${NET_SOURCES}
lib_a_LIBADD = ${SOCKET_OBJS} ${DIRENT_OBJS}
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

all-local: crt0.o

$(SOCKET_OBJS): socket.c
	$(COMPILE) -DF_$* $< -c -o $@

$(DIRENT_OBJS): dirent.c
	$(COMPILE) -DF_$* $< -c -o $@

ACLOCAL_AMFLAGS = -I ../../.. -I ../../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
