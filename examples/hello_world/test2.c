#include <stdio.h>
#include "s6502.h"
#include "opcodes.h"

int main() {
	loadPrg("test2.bin");
	memory[0xF000] = RTS;
	while(step()) {
		switch(r_PC) {
		case 0xF000:
			printf("%c", r_A);
			break;
		}
	}
	return 0;
}
