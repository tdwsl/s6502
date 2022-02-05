#ifndef S6502_H
#define S6502_H

#include <stdbool.h>
#include "byte.h"

extern byte r_X, r_Y, r_A;
extern byte r_SP, r_F;
extern word r_PC;

extern byte memory[0xFFFF+1];

void printRegisters();
void loadPrg(const char *filename);
bool step();

#endif
