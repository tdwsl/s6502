#include <stdio.h>
#include "s6502.h"

int main(int argc, char **args) {
	if(argc != 2) {
		printf("usage:\n");
		printf("  trace <file.bin>\n");
		return 0;
	}
	loadPrg(args[1]);

	do {
		printRegisters();
	} while(step());
	printRegisters();

	return 0;
}
