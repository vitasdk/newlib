/*

Copyright (C) 2021, vitasdk

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
#include <sys/types.h>
#include <unistd.h>

#include <psp2/kernel/rng.h>

#define MAX_ENTROPY 256

int getentropy(void *ptr, size_t n)
{
    size_t target = n;
    void* p = ptr;
    int ret;

    if (n > MAX_ENTROPY) {
        errno = EIO;
        return -1;
    }

    while (target > 64) {
        ret = sceKernelGetRandomNumber(ptr, 64);
        ptr += 64;
        target -= 64;
        if (ret < 0 ) {
            errno = EIO;
            return -1;
        }
    }

    ret = sceKernelGetRandomNumber(ptr, target);
    if (ret < 0 ) {
        errno = EIO;
        return -1;
    }
    return 0;
}

void _arc4random_getentropy_fail(void)
{

}
