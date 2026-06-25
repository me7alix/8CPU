#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

typedef struct {
	uint8_t A, B;
	uint8_t PR, PC;
	uint8_t PROG[512];
	uint8_t RAM[256];
	uint8_t SCRN[256];
	uint8_t VRAM[256];
} CPU;

void exec1(CPU *cpu) {
	int idx = (int)cpu->PC * 2;
	uint8_t opc = cpu->PROG[idx];
	uint8_t opr = cpu->PROG[idx+1];
	switch (opc) {
	case 1:  cpu->A += cpu->B;          break;
	case 2:  cpu->A -= cpu->B;          break;
	case 3:  cpu->PC = opr;             return;
	case 6:  cpu->A = opr;              break;
	case 9:  cpu->PR = cpu->A;          break;
	case 10: cpu->B = opr;              break;
	case 11: cpu->B = cpu->A;           break;
	case 12: cpu->VRAM[cpu->A] = opr;   break;
	case 5:  cpu->A = cpu->RAM[cpu->B]; break;
	case 15: cpu->RAM[cpu->B] = cpu->A; break;
	case 7:  cpu->A = cpu->RAM[opr];    break;
	case 8:  cpu->RAM[opr] = cpu->A;    break;
	case 4:
		if (cpu->A != 0) {
			cpu->PC = opr;
			return;
		} else break;
	case 13:
		memcpy(cpu->SCRN, cpu->VRAM, sizeof(cpu->SCRN));
		break;
	case 14:
		memset(cpu->VRAM, 0, sizeof(cpu->VRAM));
	}
	cpu->PC++;
}

int read_num(char *ln) {
	char num[32];
	size_t ncnt = 0;
	while (isdigit(*ln))
		num[ncnt++] = *ln++;
	num[ncnt] = '\0';
	return atoi(num);
}

void read_label(char *label, char *ln) {
	size_t cnt = 0;
	while (isalnum(*ln))
		label[cnt++] = *ln++;
	label[cnt] = '\0';
}

typedef struct {
	char id[32];
	size_t pos;
} Label;

size_t translate(uint8_t *code, FILE *f) {
	Label labels[256];
	Label jumps[256];
	size_t lcnt = 0, jcnt = 0;
	size_t count = 0, lines = 0;
	char buffer[1024], *ln;
	while ((ln = fgets(buffer, sizeof(buffer), f)) != NULL) {
		lines++;
		while (*ln == ' ' || *ln == '\t') ln++;
		if (strncmp(ln, "imma", 4) == 0) {
			ln += 4;
			while (*ln == ' ') ln++;
			code[count++] = 6;
			code[count++] = read_num(ln);
		} else if (strncmp(ln, "immb", 4) == 0) {
			ln += 4;
			while (*ln == ' ') ln++;
			code[count++] = 10;
			code[count++] = read_num(ln);
		} else if (strncmp(ln, "ldai", 4) == 0) {
			code[count++] = 5;
			code[count++] = 0;
		} else if (strncmp(ln, "stai", 4) == 0) {
			code[count++] = 15;
			code[count++] = 0;
		} else if (strncmp(ln, "scst", 4) == 0) {
			ln += 4;
			while (*ln == ' ') ln++;
			code[count++] = 12;
			code[count++] = read_num(ln);
		} else if (strncmp(ln, "lda", 3) == 0) {
			ln += 3;
			while (*ln == ' ') ln++;
			code[count++] = 7;
			code[count++] = read_num(ln);
		} else if (strncmp(ln, "sta", 3) == 0) {
			ln += 3;
			while (*ln == ' ') ln++;
			code[count++] = 8;
			code[count++] = read_num(ln);
		} else if (strncmp(ln, "add", 3) == 0) {
			code[count++] = 1;
			code[count++] = 0;
		} else if (strncmp(ln, "sub", 3) == 0) {
			code[count++] = 2;
			code[count++] = 0;
		} else if (strncmp(ln, "pra", 3) == 0) {
			code[count++] = 9;
			code[count++] = 0;
		} else if (strncmp(ln, "scrs", 4) == 0) {
			code[count++] = 14;
			code[count++] = 0;
		} else if (strncmp(ln, "scrf", 4) == 0) {
			code[count++] = 13;
			code[count++] = 0;
		} else if (strncmp(ln, "mvab", 4) == 0) {
			code[count++] = 11;
			code[count++] = 0;
		} else if (
			strncmp(ln, "jnz", 3) == 0 ||
			strncmp(ln, "jmp", 3) == 0
		) {
			bool jnz = strncmp(ln, "jnz", 3) == 0;
			int jk = jnz ? 4 : 3;
			code[count++] = jk;
			ln += 3; while (*ln == ' ') ln++;
			char label[32];
			read_label(label, ln);
			bool found = false;
			for (size_t i = 0; i < lcnt; i++) {
				if (strcmp(labels[i].id, label) == 0) {
					code[count++] = labels[i].pos;
					found = true;
					break;
				}
			}
			if (!found) {
				code[count++] = 0;
				memcpy(jumps[jcnt].id, label, sizeof(label));
				jumps[jcnt++].pos = count - 1;
			}
		} else if (*ln == ';' || *ln == '\n') {
			continue;
		} else {
			char *savdln = ln;
			bool found = false;
			while (*ln != '\0') {
				if (*ln++ == ':') {
					found = true;
					break;
				}
			}
			if (!found) {
				fprintf(stderr, "line %zu: wrong instruction\n", lines);
				return 1;
			}
			ln = savdln;
			char label[32];
			read_label(label, ln);
			memcpy(labels[lcnt].id, label, sizeof(label));
			labels[lcnt++].pos = count / 2;
			for (size_t i = 0; i < jcnt; i++) {
				if (strcmp(jumps[i].id, label) == 0) {
					code[jumps[i].pos] = count / 2;
				}
			}
		}
	}
	return count;
}

void generate(char *code, size_t count, FILE *f) {
	for (size_t i = 0; i < count; i+=2) {
		fprintf(f, "%02X%02X\n", (uint8_t)code[i], (uint8_t)code[i + 1]);
	}
}

void draw_state(CPU *cpu) {
	char buf[4096];
	FILE *fp = fmemopen(buf, sizeof(buf), "w");
	fprintf(fp, "%-32s    %s\n", "SCRN", "RAM");
	for (size_t i = 0; i < 16; i++) {
		for (size_t j = 0; j < 16; j++) {
			if (cpu->SCRN[(15 - i) * 16 + j] == 0) fprintf(fp, ". ");
			else fprintf(fp, "██");
		}
		fprintf(fp, "    ");
		for (size_t j = 0; j < 16; j++) {
			uint8_t v = cpu->RAM[i * 16 + j];
			fprintf(fp, "%02X ", v);
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "\nREGS\n");
	fprintf(fp, "A  = %u\n", cpu->A);
	fprintf(fp, "B  = %u\n", cpu->B);
	fprintf(fp, "PC = %u\n", cpu->PC);
	fprintf(fp, "PR = %u\n", cpu->PR);
	printf("\033[H");
	printf("%s", buf);
	fclose(fp);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: comp/sim ...\n");
		return 1;
	}
	uint8_t code[4096] = {0};
	size_t count = translate(code, fopen(argv[2], "r"));
	if (strcmp(argv[1], "comp") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Usage: [input.s] [output.b]\n");
			return 1;
		}
		generate(code, count, fopen(argv[3], "w"));
	} else if (strcmp(argv[1], "sim") == 0) {
		if (argc != 4) {
			fprintf(stderr, "Usage: [input.s] [nanos]\n");
			return 1;
		}
		CPU cpu;
		memcpy(cpu.PROG, code, sizeof(cpu.PROG));
		int slp = atoi(argv[3]);
		printf("\033[2J");
		while (cpu.PROG[(int)cpu.PC*2] != 0) {
			exec1(&cpu);
			draw_state(&cpu);
			if (slp != -1) usleep(slp);
			else getchar();
		}
	} else {
		fprintf(stderr, "Usage: comp/sim ...\n");
		return 1;
	}
	return 0;
}
