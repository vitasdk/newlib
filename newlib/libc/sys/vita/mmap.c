/*

Copyright (C) 2017, David "Davee" Morgan
Copyright (C) 2019, Sunguk Lee

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
#include <sys/mman.h>
#include <string.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <psp2/kernel/sysmem.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	// can't use file
	if (fd != -1) {
		errno = ENOSYS;
		return MAP_FAILED;
	}
	// can't use WX perm
	if ((prot & (PROT_EXEC | PROT_WRITE)) == (PROT_EXEC | PROT_WRITE)) {
		errno = EPERM;
		return MAP_FAILED;
	}

	if ((prot & (PROT_WRITE | PROT_READ)) != (PROT_WRITE | PROT_READ)) {
		errno = EPERM;
		return MAP_FAILED;
	}
	if ((flags & MAP_PRIVATE) != MAP_PRIVATE) {
		errno = ENOSYS;
		return MAP_FAILED;
	}

	// can't use shared mem
	if (flags & MAP_SHARED) {
		errno = ENOSYS;
		return MAP_FAILED;
	}

	SceUID blkid = sceKernelAllocMemBlock("mmap", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, length, 0);
	if (blkid < 0) {
		errno = ENOMEM;
		return MAP_FAILED;
	}

	void *block_addr = NULL;
	sceKernelGetMemBlockBase(blkid, &block_addr);

	if (flags & (MAP_ANON | MAP_ANONYMOUS))
		memset(block_addr, 0, length);

	return block_addr;
}

int munmap(void *addr, size_t length) {
	SceUID block = sceKernelFindMemBlockByAddr(addr, length);

	if (block < 0) {
		errno = EINVAL;
		return -1;
	}

	sceKernelFreeMemBlock(block);
	return 0;
}
