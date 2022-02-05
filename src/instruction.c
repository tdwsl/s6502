#include <stdlib.h>
#include <stdbool.h>
#include "s6502.h"
#include "instruction.h"
#include "byte.h"
#include "flags.h"
#include "opcodes.h"

void setFlags(byte b) {
	if(!b)
		r_F |= f_Z;
	else
		r_F &= !f_Z;
	r_F |= b & f_N;
}

void inc(byte *b) {
	(*b)++;
	setFlags(*b);
}

void dec(byte *b) {
	(*b)--;
	setFlags(*b);
}

void cmp(byte b, byte c) {
	if(b == c) {
		r_F &= !f_N;
		r_F |= f_Z;
	}
	else {
		r_F &= !f_Z;
		r_F &= b & f_N;
	}

	if(b >= c)
		r_F |= f_C;
	else
		r_F &= !f_C;
}

byte *m_abs(byte *ptr) {
	r_PC += 3;
	return &memory[getWord(ptr[2], ptr[1])];
}

byte *m_zpg(byte *ptr) {
	r_PC += 2;
	return &memory[ptr[1]];
}

byte *m_zpg_X(byte *ptr) {
	r_PC += 2;
	return &memory[(byte)(ptr[1]+r_X)];
}

byte *m_zpg_Y(byte *ptr) {
	r_PC += 2;
	return &memory[(byte)(ptr[1]+r_Y)];
}

byte *m_abs_Y(byte *ptr) {
	r_PC += 3;
	return &memory[getWord(ptr[2], ptr[1])+(word)r_Y];
}

byte *m_abs_X(byte *ptr) {
	r_PC += 3;
	return &memory[getWord(ptr[2], ptr[1])+(word)r_X];
}

byte *m_ind_X(byte *ptr) {
	r_PC += 2;
	return &memory[getWord(memory[ptr[1]+r_X+1], memory[ptr[1]+r_X])];
}

byte *m_ind_Y(byte *ptr) {
	r_PC += 2;
	return &memory[getWord(memory[ptr[1]+1], memory[ptr[1]]) + (word)r_Y];
}

void asl(byte *b) {
	if((*b) & 0x80)
		r_F |= f_C;
	else
		r_F &= !f_C;
	(*b) <<= 1;
	setFlags(*b);
}

void adc(byte b) {
	r_F &= !f_V;

	for(; b; b--) {
		r_A++;
		if(!r_A)
			r_F |= f_V;

	}

	if(r_F & f_C) {
		r_A++;
		if(!r_A)
			r_F |= f_V;
	}

	if(r_F & f_V)
		r_F |= f_C;
	else
		r_F &= !f_C;

	setFlags(r_A);
}

void push(byte b) {
	memory[0x0100 | (word)r_SP] = b;
	r_SP--;
}

byte pop() {
	r_SP++;
	byte b = memory[0x0100 | (word)r_SP];
	return b;
}

void pushPC() {
	byte h = r_PC >> 8;
	byte l = r_PC & 0x00FF;
	push(h);
	push(l);
}

void popPC() {
	byte l = pop();
	byte h = pop();
	r_PC = getWord(h, l);
}

void sbc(byte b) {
	r_F &= !f_V;

	for(; b; b--) {
		r_A--;
		if(r_A == 0xFF)
			r_F |= f_V;

	}

	if(r_F & f_C) {
		r_A--;
		if(r_A == 0xFF)
			r_F |= f_V;
	}

	if(r_F & f_V)
		r_F |= f_C;
	else
		r_F &= !f_C;

	setFlags(r_A);
}

void lsr(byte *b) {
	if((*b) & 0x01)
		r_F |= f_C;
	else
		r_F &= !f_C;
	(*b) >>= 1;
	setFlags(*b);
}

void rol(byte *b) {
	bool oc = r_F & f_C;
	asl(b);
	if(oc)
		(*b) |= 0x01;
}

void ror(byte *b) {
	bool oc = r_F & f_C;
	lsr(b);
	if(oc)
		(*b) |= 0x80;
}

void branch(byte b) {
	char m = 0;
	m |= b;
	r_PC += m;
}

bool doInstruction(byte *ptr) {
	switch(*ptr) {
	/* LDY */
	case LDY_imm:
		r_Y = ptr[1];
		setFlags(r_Y);
		r_PC += 2;
		break;
	case LDY_zpg:
		r_Y = *m_zpg(ptr);
		setFlags(r_Y);
		break;
	case LDY_zpg_X:
		r_Y = *m_zpg_X(ptr);
		setFlags(r_Y);
		break;
	case LDY_abs:
		r_Y = *m_abs(ptr);
		setFlags(r_Y);
		break;
	case LDY_abs_X:
		r_Y = *m_abs_X(ptr);
		setFlags(r_Y);
		break;

	/* LDA */
	case LDA_imm:
		r_A = ptr[1];
		setFlags(r_A);
		r_PC += 2;
		break;
	case LDA_zpg:
		r_A = *m_zpg(ptr);
		setFlags(r_A);
		break;
	case LDA_zpg_X:
		r_A = *m_zpg_X(ptr);
		setFlags(r_A);
		break;
	case LDA_abs:
		r_A = *m_abs(ptr);
		setFlags(r_A);
		break;
	case LDA_abs_X:
		r_A = *m_abs_X(ptr);
		setFlags(r_A);
		break;
	case LDA_abs_Y:
		r_A = *m_abs_Y(ptr);
		setFlags(r_A);
		break;
	case LDA_ind_X:
		r_A = *m_ind_X(ptr);
		setFlags(r_A);
		break;
	case LDA_ind_Y:
		r_A = *m_ind_Y(ptr);
		setFlags(r_A);
		break;

	/* LDX */
	case LDX_imm:
		r_X = ptr[1];
		setFlags(r_X);
		r_PC += 2;
		break;
	case LDX_zpg:
		r_X = *m_zpg(ptr);
		setFlags(r_X);
		break;
	case LDX_zpg_Y:
		r_X = *m_zpg_Y(ptr);
		setFlags(r_X);
		break;
	case LDX_abs:
		r_X = *m_abs(ptr);
		setFlags(r_X);
		break;
	case LDX_abs_Y:
		r_X = *m_abs_Y(ptr);
		setFlags(r_X);
		break;

	/* STY */
	case STY_zpg:
		*m_zpg(ptr) = r_Y;
		break;
	case STY_zpg_X:
		*m_zpg_X(ptr) = r_Y;
		break;
	case STY_abs:
		*m_abs(ptr) = r_Y;
		break;

	/* STA */
	case STA_zpg:
		*m_zpg(ptr) = r_A;
		break;
	case STA_zpg_X:
		*m_zpg_X(ptr) = r_A;
		break;
	case STA_abs:
		*m_abs(ptr) = r_A;
		break;
	case STA_abs_X:
		*m_abs_X(ptr) = r_A;
		break;
	case STA_abs_Y:
		*m_abs_Y(ptr) = r_A;
		break;
	case STA_ind_X:
		*m_ind_X(ptr) = r_A;
		break;
	case STA_ind_Y:
		*m_ind_Y(ptr) = r_A;
		break;

	/* STX */
	case STX_zpg:
		*m_zpg(ptr) = r_X;
		break;
	case STX_zpg_Y:
		*m_zpg_Y(ptr) = r_X;
		break;
	case STX_abs:
		*m_abs(ptr) = r_X;
		break;

	/* INC */
	case INC_zpg:
		inc(m_zpg(ptr));
		break;
	case INC_zpg_X:
		inc(m_zpg_X(ptr));
		break;
	case INC_abs:
		inc(m_abs(ptr));
		break;
	case INC_abs_X:
		inc(m_abs_X(ptr));
		break;

	/* DEC */
	case DEC_zpg:
		dec(m_zpg(ptr));
		break;
	case DEC_zpg_X:
		dec(m_zpg_X(ptr));
		break;
	case DEC_abs:
		dec(m_abs(ptr));
		break;
	case DEC_abs_X:
		dec(m_abs_X(ptr));
		break;

	/* CMP */
	case CMP_imm:
		cmp(r_A, ptr[1]);
		r_PC += 2;
		break;
	case CMP_zpg:
		cmp(r_A, *m_zpg(ptr));
		break;
	case CMP_zpg_X:
		cmp(r_A, *m_zpg_X(ptr));
		break;
	case CMP_abs:
		cmp(r_A, *m_abs(ptr));
		break;
	case CMP_abs_X:
		cmp(r_A, *m_abs_X(ptr));
		break;
	case CMP_abs_Y:
		cmp(r_A, *m_abs_Y(ptr));
		break;
	case CMP_ind_X:
		cmp(r_A, *m_ind_X(ptr));
		break;
	case CMP_ind_Y:
		cmp(r_A, *m_ind_Y(ptr));
		break;

	/* CPX */
	case CPX_imm:
		cmp(r_X, ptr[1]);
		r_PC += 2;
		break;
	case CPX_zpg:
		cmp(r_X, *m_zpg(ptr));
		break;
	case CPX_abs:
		cmp(r_X, *m_abs(ptr));
		break;

	/* CPY */
	case CPY_imm:
		cmp(r_Y, ptr[1]);
		r_PC += 2;
		break;
	case CPY_zpg:
		cmp(r_Y, *m_zpg(ptr));
		break;
	case CPY_abs:
		cmp(r_Y, *m_abs(ptr));
		break;

	/* AND */
	case AND_imm:
		r_A &= ptr[1];
		r_PC += 2;
		break;
	case AND_zpg:
		r_A &= *m_zpg(ptr);
		break;
	case AND_zpg_X:
		r_A &= *m_zpg_X(ptr);
		break;
	case AND_abs:
		r_A &= *m_abs(ptr);
		break;
	case AND_abs_X:
		r_A &= *m_abs_X(ptr);
		break;
	case AND_abs_Y:
		r_A &= *m_abs_Y(ptr);
		break;
	case AND_ind_X:
		r_A &= *m_ind_X(ptr);
		break;
	case AND_ind_Y:
		r_A &= *m_ind_Y(ptr);
		break;

	/* ORA */
	case ORA_imm:
		r_A |= ptr[1];
		setFlags(r_A);
		r_PC += 2;
		break;
	case ORA_zpg:
		r_A |= *m_zpg(ptr);
		setFlags(r_A);
		break;
	case ORA_zpg_X:
		r_A |= *m_zpg_X(ptr);
		setFlags(r_A);
		break;
	case ORA_abs:
		r_A |= *m_abs(ptr);
		setFlags(r_A);
		break;
	case ORA_abs_X:
		r_A |= *m_abs_X(ptr);
		setFlags(r_A);
		break;
	case ORA_abs_Y:
		r_A |= *m_abs_Y(ptr);
		setFlags(r_A);
		break;
	case ORA_ind_X:
		r_A |= *m_ind_X(ptr);
		setFlags(r_A);
		break;
	case ORA_ind_Y:
		r_A |= *m_ind_Y(ptr);
		setFlags(r_A);
		break;

	/* EOR */
	case EOR_imm:
		r_A ^= ptr[1];
		setFlags(r_A);
		r_PC += 2;
		break;
	case EOR_zpg:
		r_A ^= *m_zpg(ptr);
		setFlags(r_A);
		break;
	case EOR_zpg_X:
		r_A ^= *m_zpg_X(ptr);
		setFlags(r_A);
		break;
	case EOR_abs:
		r_A ^= *m_abs(ptr);
		setFlags(r_A);
		break;
	case EOR_abs_X:
		r_A ^= *m_abs_X(ptr);
		setFlags(r_A);
		break;
	case EOR_abs_Y:
		r_A ^= *m_abs_Y(ptr);
		setFlags(r_A);
		break;
	case EOR_ind_X:
		r_A ^= *m_ind_X(ptr);
		setFlags(r_A);
		break;
	case EOR_ind_Y:
		r_A ^= *m_ind_Y(ptr);
		setFlags(r_A);
		break;

	/* ROL */
	case ROL_A:
		rol(&r_A);
		r_PC++;
		break;
	case ROL_zpg:
		rol(m_zpg(ptr));
		break;
	case ROL_zpg_X:
		rol(m_zpg_X(ptr));
		break;
	case ROL_abs:
		rol(m_abs(ptr));
		break;
	case ROL_abs_X:
		rol(m_abs_X(ptr));
		break;

	/* ROR */
	case ROR_A:
		ror(&r_A);
		r_PC++;
		break;
	case ROR_zpg:
		ror(m_zpg(ptr));
		break;
	case ROR_zpg_X:
		ror(m_zpg_X(ptr));
		break;
	case ROR_abs:
		ror(m_abs(ptr));
		break;
	case ROR_abs_X:
		ror(m_abs_X(ptr));
		break;

	/* LSR */
	case LSR_A:
		lsr(&r_A);
		r_PC++;
		break;
	case LSR_zpg:
		lsr(m_zpg(ptr));
		break;
	case LSR_zpg_X:
		lsr(m_zpg_X(ptr));
		break;
	case LSR_abs:
		lsr(m_abs(ptr));
		break;
	case LSR_abs_X:
		lsr(m_abs_X(ptr));
		break;

	/* ASL */
	case ASL_A:
		asl(&r_A);
		r_PC++;
		break;
	case ASL_zpg:
		asl(m_zpg(ptr));
		break;
	case ASL_zpg_X:
		asl(m_zpg_X(ptr));
		break;
	case ASL_abs:
		asl(m_abs(ptr));
		break;
	case ASL_abs_X:
		asl(m_abs_X(ptr));
		break;

	/* ADC */
	case ADC_imm:
		adc(ptr[1]);
		r_PC += 2;
		break;
	case ADC_zpg:
		adc(*m_zpg(ptr));
		break;
	case ADC_zpg_X:
		adc(*m_zpg_X(ptr));
		break;
	case ADC_abs:
		adc(*m_abs(ptr));
		break;
	case ADC_abs_X:
		adc(*m_abs_X(ptr));
		break;
	case ADC_abs_Y:
		adc(*m_abs_Y(ptr));
		break;
	case ADC_ind_X:
		adc(*m_ind_X(ptr));
		break;
	case ADC_ind_Y:
		adc(*m_ind_Y(ptr));
		break;

	/* SBC */
	case SBC_imm:
		sbc(ptr[1]);
		r_PC += 2;
		break;
	case SBC_zpg:
		sbc(*m_zpg(ptr));
		break;
	case SBC_zpg_X:
		sbc(*m_zpg_X(ptr));
		break;
	case SBC_abs:
		sbc(*m_abs(ptr));
		break;
	case SBC_abs_X:
		sbc(*m_abs_X(ptr));
		break;
	case SBC_abs_Y:
		sbc(*m_abs_Y(ptr));
		break;
	case SBC_ind_X:
		sbc(*m_ind_X(ptr));
		break;
	case SBC_ind_Y:
		sbc(*m_ind_Y(ptr));
		break;

	/* JMP */
	case JMP_abs:
		r_PC += 2;
		pushPC();
		r_PC = getWord(ptr[2], ptr[1]);
		break;
	case JMP_ind:
		r_PC += 2;
		pushPC();
		r_PC = getWord(memory[ptr[1]+1], memory[ptr[1]]);
		break;

	/* transfer */
	case TAX:
		r_X = r_A;
		setFlags(r_X);
		r_PC++;
		break;
	case TSX:
		r_X = r_SP;
		setFlags(r_X);
		r_PC++;
		break;
	case TAY:
		r_Y = r_A;
		setFlags(r_Y);
		r_PC++;
		break;
	case TYA:
		r_A = r_Y;
		setFlags(r_A);
		r_PC++;
		break;
	case TXA:
		r_A = r_X;
		setFlags(r_A);
		r_PC++;
		break;
	case TXS:
		r_SP = r_X;
		setFlags(r_SP);
		r_PC++;
		break;

	/* INX, INY */
	case INX:
		inc(&r_X);
		r_PC++;
		break;
	case INY:
		inc(&r_Y);
		r_PC++;
		break;

	/* DEX, DEY */
	case DEX:
		dec(&r_X);
		r_PC++;
		break;
	case DEY:
		dec(&r_Y);
		r_PC++;
		break;

	/* push */
	case PHA:
		push(r_A);
		r_PC++;
		break;
	case PHP:
		push(r_F);
		r_PC++;
		break;

	/* pop */
	case PLA:
		r_A = pop();
		r_PC++;
		break;
	case PLP:
		r_F = pop();
		r_PC++;
		break;

	/* subroutine / interrupt */
	case JSR:
		r_PC += 2;
		pushPC();
		r_PC = getWord(ptr[2], ptr[1]);
		break;
	case RTS:
		popPC();
		r_PC++;
		break;
	case RTI:
		r_F = pop();
		popPC();
		r_PC++;
		break;

	/* branch */
	case BEQ:
		r_PC += 2;
		if(r_F & f_Z)
			branch(ptr[1]);
		break;
	case BNE:
		r_PC += 2;
		if(!(r_F & f_Z))
			branch(ptr[1]);
		break;
	case BCS:
		r_PC += 2;
		if(r_F & f_C)
			branch(ptr[1]);
		break;
	case BCC:
		r_PC += 2;
		if(!(r_F & f_C))
			branch(ptr[1]);
		break;
	case BVS:
		r_PC += 2;
		if(r_F & f_V)
			branch(ptr[1]);
		break;
	case BVC:
		r_PC += 2;
		if(!(r_F & f_V))
			branch(ptr[1]);
		break;
	case BMI:
		r_PC += 2;
		if(r_F & f_N)
			branch(ptr[1]);
		break;
	case BPL:
		r_PC += 2;
		if(!(r_F & f_N))
			branch(ptr[1]);
		break;

	/* clear */
	case CLC:
		r_F &= !f_C;
		r_PC++;
		break;
	case CLD:
		r_F &= !f_D;
		r_PC++;
		break;
	case CLI:
		r_F &= !f_I;
		r_PC++;
		break;
	case CLV:
		r_F &= !f_V;
		r_PC++;
		break;

	/* set */
	case SEC:
		r_F |= f_C;
		r_PC++;
		break;
	case SED:
		r_F |= f_D;
		r_PC++;
		break;
	case SEI:
		r_F |= f_I;
		r_PC++;
		break;

	case NOP:
		r_PC++;
		break;

	case BRK:
	default:
		r_PC += 2;
		pushPC();
		r_PC -= 2;
		push(r_F);
		r_F |= f_I;
		return false;
	}

	return true;
}
