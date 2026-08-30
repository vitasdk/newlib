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

/* memcmp for ARMv7-A.

   machine/arm had no memcmp at all, so Vita was running the generic C
   one: measured on hardware it does 232 MB/s at 4 KiB against 399 for
   SceLibKernel's.  Both are far under what the part can do -- memcpy
   reaches 1136 over the same buffers -- because the cost here is the
   compare and the branch, not the load.

   So the loop below spends one branch per 16 bytes instead of one per
   word: four words from each side, xor them pairwise, or the results
   together, and only when that is non-zero does it go back over those
   16 bytes a byte at a time to find which one differs and in which
   direction.  ldm needs word alignment, so a misaligned pair drops to a
   plain ldr loop, which ARMv7-A serves unaligned.  */

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

def_fn memcmp p2align=6
	cmp	r2, #16
	blo	.Lcmp_short
	orr	r3, r0, r1
	tst	r3, #3
	bne	.Lcmp_words		/* misaligned: ldm will not do */

	push	{r4, r5, r6, r7, r8, r9, r10, lr}
.Lcmp_l16:
	ldmia	r0!, {r3, r4, r5, r6}
	ldmia	r1!, {r7, r8, r9, r10}
	eor	lr, r3, r7
	eor	ip, r4, r8
	orr	lr, lr, ip
	eor	ip, r5, r9
	orr	lr, lr, ip
	eor	ip, r6, r10
	orrs	lr, lr, ip
	bne	.Lcmp_found16
	sub	r2, r2, #16
	cmp	r2, #16
	bhs	.Lcmp_l16
	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
	b	.Lcmp_short

.Lcmp_found16:
	/* The difference is inside the 16 bytes just consumed.  */
	sub	r0, r0, #16
	sub	r1, r1, #16
	mov	r2, #16
	pop	{r4, r5, r6, r7, r8, r9, r10, lr}
	b	.Lcmp_bytes

.Lcmp_short:
	cmp	r2, #4
	blo	.Lcmp_bytes

	/* No frame here: the small sizes go through this path and a
	   push/pop pair cost them a third of their throughput.  */
.Lcmp_words:
1:
	cmp	r2, #4
	blo	.Lcmp_bytes
	ldr	r3, [r0], #4
	ldr	ip, [r1], #4
	cmp	r3, ip
	bne	3f
	sub	r2, r2, #4
	b	1b
3:
	sub	r0, r0, #4
	sub	r1, r1, #4
	mov	r2, #4

.Lcmp_bytes:
	cmp	r2, #0
	beq	.Lcmp_equal
4:
	ldrb	r3, [r0], #1
	ldrb	ip, [r1], #1
	subs	r3, r3, ip
	bne	.Lcmp_diff
	subs	r2, r2, #1
	bne	4b

.Lcmp_equal:
	mov	r0, #0
	bx	lr

.Lcmp_diff:
	mov	r0, r3
	bx	lr

	.size	memcmp, . - memcmp
