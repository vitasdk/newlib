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

/* NEON memset for ARMv7-A with unaligned access.

   Uses q0/q1 and no callee-saved register, so it needs no stack frame.
   Clobbering them is what AAPCS allows a normal call to do, which is why
   this must never back __aeabi_memset: that helper may only corrupt
   r0-r3, ip, lr and CPSR.  aeabi_memset.c keeps those on the integer
   routine in aeabi_memset-soft.S; do not point them here.

   Invariant behind the overlapping tail store: at every point below,
   cursor + remaining == dstin + count.  With count >= 16 the last
   16-byte store therefore lands on [dstin+count-16, dstin+count),
   which is inside the buffer.  */

	.syntax unified
	.arm

#ifdef BROKEN_ASM_NEON_ALIGN
#define ALIGN(addr, align) addr,:align
#else
#define ALIGN(addr, align) addr:align
#endif

	.macro def_fn f p2align=0
	.text
	.p2align \p2align
	.global \f
	.type \f, %function
\f:
	.endm

	.fpu	neon
	.arch	armv7-a

def_fn memset p2align=6
	and	r1, r1, #255
	cmp	r2, #16
	blo	.Lset_small

	vdup.8	q0, r1
	vmov	q1, q0			/* two copies: 32 bytes per store */
	add	ip, r0, r2		/* ip = end of buffer */

	/* Head: cover [dstin, dstin+16), then round the cursor up so every
	   later store is 16-byte aligned.  */
	vst1.8	{d0, d1}, [r0]
	add	r3, r0, #16
	bic	r3, r3, #15

	sub	r2, ip, r3		/* bytes still to write from cursor */
	subs	r2, r2, #64
	blo	.Lset_tail

.Lset_loop64:
	vst1.8	{d0, d1, d2, d3}, [ALIGN(r3, 128)]!
	vst1.8	{d0, d1, d2, d3}, [ALIGN(r3, 128)]!
	subs	r2, r2, #64
	bhs	.Lset_loop64

.Lset_tail:
	adds	r2, r2, #64		/* 0..63 left */
	beq	.Lset_done

	cmp	r2, #32
	blo	1f
	vst1.8	{d0, d1, d2, d3}, [ALIGN(r3, 128)]!
	subs	r2, r2, #32
	beq	.Lset_done
1:
	cmp	r2, #16
	blo	2f
	vst1.8	{d0, d1}, [ALIGN(r3, 128)]!
	subs	r2, r2, #16
	beq	.Lset_done
2:
	sub	ip, ip, #16
	vst1.8	{d0, d1}, [ip]

.Lset_done:
	bx	lr

	/* Under 16 bytes: integer stores only, unaligned.  */
.Lset_small:
	orr	r1, r1, r1, lsl #8
	orr	r1, r1, r1, lsl #16
	mov	r3, r0

	cmp	r2, #8
	blo	3f
	str	r1, [r3], #4
	str	r1, [r3], #4
	sub	r2, r2, #8
3:
	cmp	r2, #4
	blo	4f
	str	r1, [r3], #4
	sub	r2, r2, #4
4:
	cmp	r2, #2
	blo	5f
	strh	r1, [r3], #2
	sub	r2, r2, #2
5:
	tst	r2, #1
	strbne	r1, [r3]
	bx	lr

	.size	memset, . - memset
