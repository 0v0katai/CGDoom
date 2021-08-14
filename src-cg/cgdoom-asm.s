.global _CGD_sector_memcmp
.align 4

# A pretty fast memcmp for 512-byte sectors, with equal(0)-different(1) output
# r4: 32-aligned pointer to sector in RAM (preferably 1-cycle operand bus RAM)
# r5: 32-aligned pointer to sector in ROM
# r6: 512 (ignored; for compatibility with memcmp prototype)
#
# There are two main ideas in this code:
#
# * Read with words, since such is the affinity of the ROM. (I don't know why.)
#   I tested with longwords, the performance is much worse; bytes are somewhere
#   in-between, which tormented me as I wondered why the most trivial memcmp()
#   with poor assembler from libfxcg was faster than my hand-written function.
#
# * Weave iterations with smart register allocation to exploit superscalar
#   parallelism. We read to r0/r1 while comparing r2/r3, then vice-versa. The
#   two mov.w (LS) for one comparison execute in parallel with the cmp (EX) and
#   bf (BR) of the previous comparison, so overall one comparison takes 2
#   cycles (plus any extra cycles in ROM reads if the cache isn't hit or
#   doesn't respond immediately, and some loop overhead).
#
_CGD_sector_memcmp:
	# For the first 32 bytes, compare as fast as possible to exit early
	# when the sectors don't match (this saves a little bit).
	mov	#16, r7
1:	mov.w	@r5+, r0
	mov.w	@r4+, r1
	cmp/eq	r0, r1
	bf	.fail
	dt	r7
	bf	1b

	mov	#30, r7

.line:
	# There is a 2-cycle delay for the RAW dependency between each mov.b
	# and the corresponding use. Here the delay is honored so there are no
	# cycles lost to RAW dependencies.

	mov.w	@r5+, r0
	nop

	mov.w	@r4+, r1
	nop

	mov.w	@r5+, r2
	nop

	mov.w	@r4+, r3
	cmp/eq	r0, r1

	mov.w	@r5+, r0
	bf	.fail

	mov.w	@r4+, r1
	cmp/eq	r2, r3

	mov.w	@r5+, r2
	bf	.fail

	mov.w	@r4+, r3
	cmp/eq	r0, r1

	mov.w	@r5+, r0
	bf	.fail

	mov.w	@r4+, r1
	cmp/eq	r2, r3

	mov.w	@r5+, r2
	bf	.fail

	mov.w	@r4+, r3
	cmp/eq	r0, r1

	mov.w	@r5+, r0
	bf	.fail

	mov.w	@r4+, r1
	cmp/eq	r2, r3

	mov.w	@r5+, r2
	bf	.fail

	mov.w	@r4+, r3
	cmp/eq	r0, r1

	# These two can run in parallel (BR/EX)
	bf	.fail
	cmp/eq	r2, r3

	bf	.fail

	dt	r7
	bf	.line

.success:
	rts
	mov	#0, r0

.fail:
	# We don't specify an order
	rts
	mov	#1, r0
