/* s6502 - tdwsl 2022 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "instruction.h"
#include "byte.h"
#include "flags.h"

byte r_A, r_X, r_Y;
byte r_F=0, r_SP=0xFF;
word r_PC=0;

byte memory[0xFFFF+1] = {0};

void printRegisters() {
	printf("X: $%.2X  Y:$%.2X  a:$%.2X\n", r_X, r_Y, r_A);
	printf("SP: $%.2X PC: $%.4X\n", r_SP, r_PC);
	/* flags */
	printf("NV--DIZC\n");
	byte b = 0x80;
	for(int i = 0; i < 8; i++) {
		printf("%u", (bool)(b&r_F));
		b >>= 1;
	}
	printf("\n");
}

bool step() {
	return doInstruction(&memory[r_PC]);
}

void loadPrg(const char *filename) {
	FILE *fp = fopen(filename, "rb");
	if(!fp) {
		printf("Failed to load '%s'\n", filename);
		return;
	}
	byte h, l;
	fread(&l, 1, 1, fp);
	fread(&h, 1, 1, fp);
	r_PC = getWord(h, l);

	for(int i = 0; !feof(fp); i++)
		fread(&memory[r_PC+i], 1, 1, fp);

	fclose(fp);
}
