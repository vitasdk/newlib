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

/* Large-copy paths for memcpy on Cortex-A9.

   memcpy-armv7a.S is tuned for copies that stay in cache and falls off a
   cliff once they do not: measured on hardware in MB/s, it does 1286 at
   4 KiB but only 551 at 64 KiB and 334 at 1 MiB, where SceLibKernel's
   holds 1136 / 896 / 597.  The cause is in its own comment at the long
   copy: it deliberately avoids PLD and fakes prefetching with a dummy
   load five cache lines ahead.  That trades a real load slot per line for
   a hint the A9 would have given for free.

   So above the threshold this takes over.  With both pointers word
   aligned, an ldm/stm loop with a real pld reaches 837 / 597 -- +52% and
   +79% over Linaro's, level with SceLibKernel's.  ldm needs alignment, so
   a source that stays unaligned goes through vld1.8 instead, which does
   not: without that path an unaligned megabyte drops to 255 against
   SceLibKernel's 538.  */

	.syntax unified
	.arm
	.fpu	neon
	.arch	armv7-a

#ifdef BROKEN_ASM_NEON_ALIGN
#define ALIGN(addr, align) addr,:align
#else
#define ALIGN(addr, align) addr:align
#endif

	.text
	.p2align 6
	.global	__vita_memcpy_large
	.type	__vita_memcpy_large, %function

/* r0 = dst (returned untouched), r1 = src, r2 = n >= VITA_LARGE_COPY.  */
__vita_memcpy_large:
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
	mov	ip, r0			/* cursor; r0 is the return value */

	/* Head: bring the destination cursor to a 16-byte boundary with
	   byte stores.  n is large, so there is always room.  */
	tst	ip, #15
	beq	1f
0:
	ldrb	r3, [r1], #1
	strb	r3, [ip], #1
	subs	r2, r2, #1
	tst	ip, #15
	bne	0b
1:
	tst	r1, #3
	bne	.Lbig_neon		/* source unaligned: ldm will not do */

	subs	r2, r2, #32
	blo	.Lbig_tail
.Lbig_int:
	pld	[r1, #192]
	ldmia	r1!, {r3, r4, r5, r6, r7, r8, r9, r10}
	stmia	ip!, {r3, r4, r5, r6, r7, r8, r9, r10}
	subs	r2, r2, #32
	bhs	.Lbig_int
	b	.Lbig_tail

.Lbig_neon:
	subs	r2, r2, #32
	blo	.Lbig_tail
2:
	pld	[r1, #192]
	vld1.8	{d0, d1, d2, d3}, [r1]!
	vst1.8	{d0, d1, d2, d3}, [ALIGN(ip, 128)]!
	subs	r2, r2, #32
	bhs	2b

.Lbig_tail:
	adds	r2, r2, #32		/* 0..31 left */
	beq	.Lbig_done
3:
	cmp	r2, #4
	blo	4f
	ldr	r3, [r1], #4
	str	r3, [ip], #4
	sub	r2, r2, #4
	b	3b
4:
	cmp	r2, #0
	beq	.Lbig_done
5:
	ldrb	r3, [r1], #1
	strb	r3, [ip], #1
	subs	r2, r2, #1
	bne	5b

.Lbig_done:
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}

	.size	__vita_memcpy_large, . - __vita_memcpy_large
