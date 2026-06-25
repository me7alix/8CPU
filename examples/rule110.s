scrs
scrf

; [0, 16) - main
; [16, 32) - buf

imma 31
sta 40
lf:
	lda 40
	mvab
	imma 0
	stai
	lda 40
	immb 1
	sub
	sta 40
	jnz lf

imma 1
sta 14

imma 0
sta 45 ; ROWS

lm:
	imma 15
	sta 40 ; CNT

	ld:
		lda 40
		mvab
		ldai
		; if [CNT] == 0
		jnz ifz
		; draw black
		lda 40
		mvab
		lda 45
		add ; CNT + ROWS
		scst 0
		jmp elz
	ifz:
		; draw white
		lda 40
		mvab
		lda 45
		add ; CNT + ROWS
		scst 1
	elz:
		; CNT -= 1
		lda 40
			scrf
			immb 1
			sub
		sta 40
		jnz ld

	imma 1
	sta 40 ; CNT
	lt:
		imma 0
		sta 41 ; NG

		; [X, 0, 0]
		lda 40
		immb 1
		sub
		mvab
		ldai
		immb 1
		sub
		jnz sk1
		lda 41
		immb 4
		add
		sta 41
	sk1:

		; [0, 0, X]
		lda 40
		immb 1
		add
		mvab
		ldai
		immb 1
		sub
		jnz sk2
		lda 41
		immb 1
		add
		sta 41
	sk2:

		; [0, X, 0]
		lda 40
		mvab
		ldai
		immb 1
		sub
		jnz sk3
		lda 41
		immb 2
		add
		sta 41
	sk3:

		; RES = [42]
		; NG = abc

		; if NG == 111: RES = 0
		lda 41
		immb 7
		sub
		jnz r1
		imma 0
		sta 42
	r1:

		; if NG == 110: RES = 1
		lda 41
		immb 6
		sub
		jnz r2
		imma 1
		sta 42
	r2:

		; if NG == 101: RES = 1
		lda 41
		immb 5
		sub
		jnz r3
		imma 1
		sta 42
	r3:

		; if NG == 100: RES = 0
		lda 41
		immb 4
		sub
		jnz r4
		imma 0
		sta 42
	r4:

		; if NG == 011: RES = 1
		lda 41
		immb 3
		sub
		jnz r5
		imma 1
		sta 42
	r5:

		; if NG == 010: RES = 1
		lda 41
		immb 2
		sub
		jnz r6
		imma 1
		sta 42
	r6:

		; if NG == 001: RES = 1
		lda 41
		immb 1
		sub
		jnz r7
		imma 1
		sta 42
	r7:

		; if NG == 000: RES = 0
		lda 41
		jnz r8
		imma 0
		sta 42
	r8:

		; [CNT+16] = RES
		lda 40
		immb 16
		add
		mvab
		lda 42
		stai

		; CNT += 1
		lda 40
			immb 1
			add
		sta 40

		; if CNT == 15: jnz lt
		lda 40
		immb 15
		sub
		jnz lt

	imma 32
	sta 40 ; CNT

	lcp:
		; CNT -= 1
		lda 40
			immb 1
			sub
		sta 40

		; [41] = [CNT]
		lda 40
		mvab
		ldai
		sta 41

		; [CNT-16] = [41]
		lda 40
		immb 16
		sub
		mvab
		lda 41
		stai

		; if CNT != 16: jmp lcp
		lda 40
		immb 16
		sub
		jnz lcp

	; ROWS += 16
	lda 45
		immb 16
		add
	sta 45

	jmp lm
