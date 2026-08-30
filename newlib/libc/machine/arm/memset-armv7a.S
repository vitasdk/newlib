/* Copyright (c) 2026, VitaSDK contributors.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  */

/* memset for ARMv7-A, integer stores only.

   Measured on Cortex-A9, MB/s, 1 MiB blocks: a NEON version of this got
   962, sceClibMemset 2125, and this one 2114.  The store is the whole
   difference: the A9 has a 64-bit NEON datapath, so a 32-byte vst1.8 costs
   four cycles where an stm of four registers feeds the store buffer far
   better.  SceLibKernel's own memset loops on stm for that same reason.

   Against newlib's generic C memset it is 2.2x from 512 bytes up, and it
   stays within 2% of SceLibKernel's from 4 KiB up while winning at 64.

   Touching no FP register has a second effect worth keeping: unlike a NEON
   memset, this one is safe to back __aeabi_memset, which may only corrupt
   r0-r3, ip, lr and CPSR.  aeabi_memset.c still routes those to
   aeabi_memset-soft.S; nothing here depends on that staying so.  */

	.syntax unified
	.arm

	.macro def_fn f p2align=0
	.text
	.p2align \p2align
	.global \f
	.type \f, %function
\f:
	.endm

	.arch	armv7-a

def_fn memset p2align=6
	and	r1, r1, #255
	orr	r1, r1, r1, lsl #8
	orr	r1, r1, r1, lsl #16
	cmp	r2, #16
	blo	.Lset_small

	push	{r4, r5, lr}
	mov	ip, r0			/* cursor; r0 stays the return value */
	add	lr, r0, r2		/* end of buffer */

	/* At most three bytes to reach a word boundary; count >= 16 so
	   there is always room for them.  */
	tst	ip, #3
	beq	1f
0:
	strb	r1, [ip], #1
	tst	ip, #3
	bne	0b
1:
	mov	r3, r1
	mov	r4, r1
	mov	r5, r1

	sub	r2, lr, ip		/* bytes still to write */
	subs	r2, r2, #64
	blo	.Lset_tail

.Lset_loop64:
	stm	ip!, {r1, r3, r4, r5}
	stm	ip!, {r1, r3, r4, r5}
	stm	ip!, {r1, r3, r4, r5}
	stm	ip!, {r1, r3, r4, r5}
	subs	r2, r2, #64
	bhs	.Lset_loop64

.Lset_tail:
	adds	r2, r2, #64		/* 0..63 left */
2:
	cmp	r2, #16
	blo	3f
	stm	ip!, {r1, r3, r4, r5}
	sub	r2, r2, #16
	b	2b
3:
	cmp	r2, #8
	blo	4f
	str	r1, [ip], #4
	str	r1, [ip], #4
	sub	r2, r2, #8
4:
	cmp	r2, #4
	blo	5f
	str	r1, [ip], #4
	sub	r2, r2, #4
5:
	cmp	r2, #2
	blo	6f
	strh	r1, [ip], #2
	sub	r2, r2, #2
6:
	tst	r2, #1
	strbne	r1, [ip]
	pop	{r4, r5, pc}

	/* Under 16 bytes: unaligned integer stores, no frame.  */
.Lset_small:
	mov	r3, r0

	cmp	r2, #8
	blo	7f
	str	r1, [r3], #4
	str	r1, [r3], #4
	sub	r2, r2, #8
7:
	cmp	r2, #4
	blo	8f
	str	r1, [r3], #4
	sub	r2, r2, #4
8:
	cmp	r2, #2
	blo	9f
	strh	r1, [r3], #2
	sub	r2, r2, #2
9:
	tst	r2, #1
	strbne	r1, [r3]
	bx	lr

	.size	memset, . - memset
