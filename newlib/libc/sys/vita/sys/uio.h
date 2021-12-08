/* 
 * Copyright (C) 1991-2018 Free Software Foundation, Inc.
 * This file is part of the GNU C Library.
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, see
 * <http://www.gnu.org/licenses/>.  
 */

#ifndef _SYS_UIO_H_
#define _SYS_UIO_H_

#include <sys/types.h>
#include <string.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#ifndef __GNU_VISIBLE
void* mempcpy(void *<[out]>, const void *<[in]>, size_t <[n]>);
#endif

struct iovec {
	void   *iov_base;
	size_t  iov_len;
};

ssize_t readv(int, const struct iovec *, int);
ssize_t writev(int, const struct iovec *, int);

__END_DECLS

#endif /* sys/uio.h */
