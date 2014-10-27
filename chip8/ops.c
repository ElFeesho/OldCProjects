#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"
#include "input.h"
#include "sound.h"

static const int 		PROGRAM_START_OFFSET = 0x200;

static unsigned char 	memory[4096] = { 0 };
static int 				I = 0;
static unsigned char 	regs[16] = { 0 };
static unsigned char 	keys[16] = { 0 };
static unsigned short 	PC = 0x200;
static unsigned short 	stack[16] = { 0 };
static unsigned char 	SP = 0;

static unsigned char DT = 0;
static unsigned char ST = 0;


typedef void (*op_handler)(short);

void op_00XX(short opperand)
{
	int op = opperand & 0xff;
	if(op == 0xE0)
	{
		printf("CLEARING SCREEN\n");
		gfx_cls();
	}
	else if(op == 0xEE)
	{
		PC = stack[--SP];
	}
}

/*
 Jump to address
*/
void op_1XXX(short opperand)
{
	PC = opperand;
}

/*
 Jump to address + v0
*/
void op_BXXX(short opperand)
{
	PC = regs[0] + (opperand&0x0fff);
}

void op_CXXX(short operand)
{
	int targetRegister = (operand&0xf00)>>8;
	int mask = operand & 0xff;
	regs[targetRegister] = (rand() % 256) & mask;
}

void op_2XXX(short opperand)
{
	if(SP<16)
	{
		stack[SP] = PC;
		PC = opperand;
		SP++;
	}
	else
	{
		printf("Attempted to jump 16 times\n");
	}
}

void op_3XXX(short opperand)
{
	if(regs[(opperand & 0x0f00) >> 8] == (opperand & 0x000f))
	{
		PC += 2;
	}
}

void op_4XXX(short opperand)
{
	if(regs[(opperand&0x0f00)>>8] != (opperand & 0x00ff))
	{
		PC += 2;
	}
}

void op_5XXX(short opperand)
{
	if(regs[(opperand&0x0f00)>>8] == regs[(opperand&0x00f0)>>4])
	{
		PC += 2;
	}
}

void op_6XXX(short opperand)
{
	regs[(opperand&0x0f00)>>8] = (opperand&0x00ff);
}

void op_7XXX(short opperand)
{
	regs[(opperand&0x0f00)>>8] += (opperand&0x00ff);
}

void op_8XXX(short opperand)
{
	int subOp = opperand & 0x0f;
	int lhs = (opperand & 0x0f00)>>8;
	int rhs = (opperand & 0x00f0)>>4;
	if(subOp == 0)
	{
		regs[lhs] = regs[rhs];
	}
	else if(subOp == 1)
	{
		regs[lhs] |= regs[rhs];
	}
	else if(subOp == 2)
	{
		regs[lhs] &= regs[rhs];
	}
	else if(subOp == 3)
	{
		regs[lhs] ^= regs[rhs];
	}
	else if(subOp == 4)
	{
		if(regs[lhs] + regs[rhs] > 255)
			regs[15] = 1;
		else
			regs[15] = 0;
		regs[lhs] += regs[rhs];
	}
	else if(subOp == 5)
	{
		if(regs[rhs] > regs[lhs])
			regs[15] = 0;
		else
			regs[15] = 1;
		regs[lhs] -= regs[rhs];
	}
	else if(subOp == 6)
	{
		regs[15] = regs[lhs]&0x01;

		regs[lhs] = regs[rhs] >> 1;
	}
	else if(subOp == 7)
	{
		if(regs[lhs] > regs[rhs])
			regs[15] = 0;
		else
			regs[15] = 1;
		regs[lhs] = regs[rhs] - regs[lhs];
	}
	else if(subOp == 0xE)
	{

		regs[15] = (regs[lhs]&0x80) >> 7;

		regs[lhs] = regs[rhs] << 1;
	}
}

void op_9XXX(short opperand)
{
	if(regs[(opperand&0x0f00)>>8] != regs[(opperand&0x00f0)>>4])
	{
		PC += 2;
	}
}

void op_AXXX(short opperand)
{
	I = opperand & 0x0fff;
}

void op_DXXX(short opperand)
{
	int x = regs[(opperand & 0xf00) >> 8];
	int y = regs[(opperand & 0x00f0) >> 4];
	int count = opperand & 0x0f;
	regs[15] = gfx_draw(x, y, memory+I, count)?1:0;
}

void op_EXXX(short opperand)
{
	int targetKey = regs[(opperand&0x0f00)>>8];
	int keydown = input_keydown(targetKey);
	if((opperand&0xff)==0x9E)
	{
		if(keydown)
		{
			PC += 2;
		}
	}
	else if((opperand&0xff)==0xA1)
	{
		if(!keydown)
		{
			PC += 2;
		}
	}
}

void op_FXXX(short opperand)
{
	if((opperand&0xff)==0x07)
	{
		regs[(opperand&0x0f00)>>8] = DT;
	}
	else if((opperand&0xff)==0x0A)
	{
		int key = (opperand & 0x0f00) >> 8;
		regs[key] = input_readkey();
	}
	else if((opperand&0xff)==0x15)
	{
		DT = regs[(opperand&0x0f00)>>8];
	}
	else if((opperand&0xff)==0x18)
	{
		ST = regs[(opperand&0x0f00)>>8];
		sound_start();
	}
	else if((opperand&0xff)==0x1E)
	{
		I += regs[(opperand&0x0f00)>>8];
	}
	else if((opperand&0xff)==0x29)
	{
		// Sprite stuff
		I = 5*(regs[(opperand&0x0f00)>>8]);
	}
	else if((opperand&0xff)==0x33)
	{
		//BCD...
		int lhs = (opperand & 0x0f00) >> 8;
		int target = regs[lhs];
		int hundreds = target / 100;
		target -= hundreds * 100;
		int tens = target / 10;
		target -= tens * 10;
		int units = target;
		memory[I] = (char)hundreds;
		memory[I+1] = (char)tens;
		memory[I+2] = (char)units;

		printf("BCD %d to %d %d %d\n", regs[lhs], hundreds, tens, units);
	}
	else if((opperand&0xff)==0x55)
	{
		unsigned char *start = memory+I;
		int i = 0;
		for(;i<=(opperand&0x0f00)>>8;i++)
			*(start+i) = regs[i];

		I += ((opperand&0x0f00)>>8) + 1;
	}
	else if((opperand&0xff)==0x65)
	{
		unsigned char *start = memory+I;
		int i = 0;
		for(;i<=(opperand&0x0f00)>>8;i++)
			regs[i] = *(start+i);

		I += ((opperand&0x0f00)>>8) + 1;
	}
}

op_handler opHandlerTable[] = {
	&op_00XX,
	&op_1XXX,
	&op_2XXX,
	&op_3XXX,
	&op_4XXX,
	&op_5XXX,
	&op_6XXX,
	&op_7XXX,
	&op_8XXX,
	&op_9XXX,
	&op_AXXX,
	&op_BXXX,
	&op_CXXX,
	&op_DXXX,
	&op_EXXX,
	&op_FXXX
};

static int ops_get_pc()
{
	return PC;
}

static void init_memory()
{
	unsigned char font[] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	memcpy(memory, font, 80);
}

int load_game(char *fname)
{
	FILE *game = fopen(fname, "rb");
	if(!game)
	{
		return -1;
	}

	size_t bytes_read = 0;

	while(!feof(game))
	{
		bytes_read += fread(memory + bytes_read + PROGRAM_START_OFFSET, 1, 1024, game);
	}

	printf("Read: %u bytes\n", (unsigned int)bytes_read);

	int i = 0;
	for(i = 0; i < bytes_read; i++)
	{
		printf("%02x ", memory[PROGRAM_START_OFFSET + i]);
	}
	printf("\n");
	fclose(game);

	PC = 0x200;
	init_memory();
	return 0;
}

unsigned short get_op()
{
	unsigned char opL = *((unsigned char*)memory+(PC));
	unsigned char opR = *((unsigned char*)memory+(PC+1));
	PC += 2;
	return ((short)opL<<8) + opR;
}

void print_cpu_state()
{
	printf("PC: %04X SP: %02d I: %X REGS: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n", PC, SP, I, regs[0], regs[1], regs[2], regs[3], regs[4], regs[5], regs[6], regs[7], regs[8], regs[9], regs[10], regs[11], regs[12], regs[13], regs[14], regs[15]);
	fflush(stdout);
}

void decrement_timers()
{
	if(DT>0)
	{
		DT--;
	}

	if(ST>0)
	{
		ST--;
	}
	else
	{
		sound_stop();
	}
}

int parse_op()
{
	unsigned short op = get_op();

	int operation = (op & 0xf000) >> 12;

	opHandlerTable[operation](op&0x0fff);

	

	return 1;
}

 int draw_flag_set()
 {
 	int flagset = regs[15];
 	regs[15] = 0;
 	return flagset;
 }