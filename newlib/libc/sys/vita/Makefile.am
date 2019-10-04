## Process this file with automake to generate Makefile.in

AUTOMAKE_OPTIONS = cygnus

INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)

AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

SOCKET_OBJS = accept.o bind.o connect.o getpeername.o getsockname.o getsockopt.o listen.o recv.o recvfrom.o recvmsg.o send.o sendto.o sendmsg.o setsockopt.o shutdown.o socket.o error.o
NET_OBJS = gethostbyname.o getaddrinfo.o freeaddrinfo.o getservbyname.o inet_ntop.o inet_ntoa.o inet_netof.o inet_pton.o inet_lnaof.o inet_addr.o inet_network.o inet_net_ntop.o inet_aton.o inet_net_pton.o inet_makeaddr.o
DIRENT_OBJS = closedir.o opendir.o readdir.o readdir_r.o rewinddir.o seekdir.o telldir.o

NET_SOURCES = net/gethostbyname.c net/getaddrinfo.c net/freeaddrinfo.c net/getservbyname.c net/inet_ntop.c net/inet_ntoa.c net/inet_netof.c net/inet_pton.c net/inet_lnaof.c net/inet_addr.c net/inet_network.c net/inet_net_ntop.c net/inet_aton.c net/inet_net_pton.c net/inet_makeaddr.c
lib_a_SOURCES = syscalls.c sbrk.c threading.c mmap.c mlock.c io.c socket.c dup.c select.c error.c dirent.c lcltime_r.c ${NET_SOURCES}
lib_a_LIBADD = ${SOCKET_OBJS} ${NET_OBJS} ${DIRENT_OBJS}
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

all-local: crt0.o

$(SOCKET_OBJS): socket.c error.c
	$(COMPILE) -DF_$* $< -c -o $@

$(NET_OBJS): net/gethostbyname.c net/getaddrinfo.c net/freeaddrinfo.c net/getservbyname.c net/inet_ntop.c net/inet_ntoa.c net/inet_netof.c net/inet_pton.c net/inet_lnaof.c net/inet_addr.c net/inet_network.c net/inet_net_ntop.c net/inet_aton.c net/inet_net_pton.c net/inet_makeaddr.c
	$(COMPILE) -DF_$* $< -c -o $@

$(DIRENT_OBJS): dirent.c
	$(COMPILE) -DF_$* $< -c -o $@

ACLOCAL_AMFLAGS = -I ../../.. -I ../../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
