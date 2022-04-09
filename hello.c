#include <stdio.h>
#include "s6502.h"

int main() {
	loadPrg("hello.bin");
	memory[0xF000] = RTS;
	while(step()) {
		switch(r_PC) {
		case 0xF000:
			printf("%c", r_A);
			break;
		}
	}
	printRegisters();
	return 0;
}
