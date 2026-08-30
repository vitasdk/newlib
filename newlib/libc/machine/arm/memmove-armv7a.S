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

/* memmove for ARMv7-A.

   When a forward copy is safe -- which is nearly always -- this branches
   straight into memcpy and never comes back.  SceLibKernel's memmove does
   the same thing, and it is what newlib's generic C memmove is missing:
   that one carries its own word-at-a-time loops and never reaches the
   tuned memcpy sitting next to it.

   (unsigned)(dst - src) >= n covers both safe cases at once: dst far
   enough ahead of src, and dst below src, which wraps to a huge unsigned.
   Only dst above src by less than n needs the backward copy below.

   The backward loop uses ldmdb/stmdb rather than NEON, for the reason
   measured in memset-armv7a.S.  LDM/STM need word alignment, so the
   cursor is aligned first and a source that stays unaligned falls back to
   word-at-a-time ldr/str, which ARMv7-A serves unaligned.

   Beware if this ever backs __aeabi_memmove: the forward path lands in
   memcpy, which uses FP registers, and that helper may only corrupt
   r0-r3, ip, lr and CPSR.  aeabi_memmove.c keeps it on the integer
   routine in aeabi_memmove-soft.S.  */

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

def_fn memmove p2align=6
	subs	r3, r0, r1
	beq	.Lmov_ret		/* dst == src */
	cmp	r3, r2
	bhs	memcpy			/* copiar hacia delante es seguro */

	cmp	r2, #0
	beq	.Lmov_ret

	push	{r4, r5, lr}
	add	ip, r0, r2		/* final del destino */
	add	r1, r1, r2		/* final del origen */

	/* Alinear el cursor de destino a palabra, como mucho 3 bytes.  */
	tst	ip, #3
	beq	1f
0:
	ldrb	r3, [r1, #-1]!
	strb	r3, [ip, #-1]!
	subs	r2, r2, #1
	beq	.Lmov_pop
	tst	ip, #3
	bne	0b
1:
	tst	r1, #3
	bne	.Lmov_words		/* origen desalineado: ldm no vale */

	subs	r2, r2, #32
	blo	.Lmov_tail
.Lmov_loop32:
	ldmdb	r1!, {r3, r4, r5, lr}
	stmdb	ip!, {r3, r4, r5, lr}
	ldmdb	r1!, {r3, r4, r5, lr}
	stmdb	ip!, {r3, r4, r5, lr}
	subs	r2, r2, #32
	bhs	.Lmov_loop32
.Lmov_tail:
	adds	r2, r2, #32		/* 0..31 */

.Lmov_words:
2:
	cmp	r2, #4
	blo	3f
	ldr	r3, [r1, #-4]!
	str	r3, [ip, #-4]!
	sub	r2, r2, #4
	b	2b
3:
	cmp	r2, #0
	beq	.Lmov_pop
4:
	ldrb	r3, [r1, #-1]!
	strb	r3, [ip, #-1]!
	subs	r2, r2, #1
	bne	4b

.Lmov_pop:
	pop	{r4, r5, pc}

.Lmov_ret:
	bx	lr

	.size	memmove, . - memmove
