imma 8
sta 0

imma 7
sta 1

imma 0
sta 2

loop:
	lda 1
	movb
	lda 2
	add
	sta 2

	lda 0
	immb 1
	sub
	sta 0

	jnz loop

lda 2
prnta
