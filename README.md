# 8CPU

<img width="942" height="510" alt="image" src="https://github.com/user-attachments/assets/620ae46c-2f94-48f6-afc7-2ee7c7f0beac" />

An accumulator based 8 bit CPU made in <a href="https://sebastian.itch.io/digital-logic-sim">DigLogSim</a>

## Registers
 - **A**: Accumulator
 - **B**: General-purpose register
 - **PC**: Program counter

## Set of instructions
 - `01` **add**:   `A = A + B`
 - `02` **sub**:   `A = A - B`
 - `03` **jmp**:   `PC = IMM`
 - `04` **jnz**:   `jmp if A != 0`
 - `05` **ldai**:  `A = RAM[B]`
 - `0F` **stai**:  `RAM[B] = A`
 - `07` **lda**:   `A = RAM[IMM]`
 - `08` **sta**:   `RAM[IMM] = A`
 - `06` **imma**:  `A = IMM`
 - `0A` **immb**:  `B = IMM`
 - `0B` **mvab**:  `B = A`
 - `09` **pra**:   `print A`
 - `0C` **scst**:  `VRAM[A] = IMM`
 - `0D` **scrf**:  `SCRN = VRAM`
 - `0E` **scrs**:  `VRAM = {0}`

## Example programs

Two numbers multiplication:

```asm
imma 8
sta 0

imma 7
sta 1

imma 0
sta 2

loop:
	lda 1
	mvab
	lda 2
	add
	sta 2

	lda 0
	immb 1
	sub
	sta 0

	jnz loop

lda 2
pra
```

Fibonacci sequence:

```asm
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
```
