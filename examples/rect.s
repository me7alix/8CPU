scrs
scrf

; x1 = 3
imma 3
sta 0

; y1 = 4
imma 4
sta 1

; x2 = 7
imma 7
sta 2

; y2 = 10
imma 10
sta 3

; current y = y1
lda 1
sta 4

; rows = y1 * 16
imma 0
sta 6

lda 1
sta 7
lr:
	lda 6
		immb 16
		add
	sta 6

	lda 7
		immb 1
		sub
	sta 7
	jnz lr

ly:
	; current x = x1
	lda 0
	sta 5

lx:
	; ACC = current x
	lda 5
	movb

	; ACC = x + rows
	lda 6
	add

	; draw pixel
	scst 1
	scrf

	; current x++
	lda 5
		immb 1
		add
	sta 5

	; while current x != x2
	lda 2
	movb
	lda 5
	sub
	jnz lx

	; current y++
	lda 4
		immb 1
		add
	sta 4

	; rows += 16
	lda 6
		immb 16
		add
	sta 6

	; while current y != y2
	lda 3
	movb
	lda 4
	sub
	jnz ly
