imma 1
sta 0
pra

imma 2
sta 1

loop:
	lda 0
	pra

	lda 1
	sta 2

	mvab
	lda 0
	add
	sta 1

	lda 2
	sta 0

	jmp loop
