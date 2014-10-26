#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "gfx.h"


static const int 		PROGRAM_START_OFFSET = 0x200;

static unsigned char 	memory[4096] = { 0 };
static int 				I = 0;
static unsigned char 	regs[16] = { 0 };
static unsigned char 	keys[16] = { 0 };
static unsigned short 	PC = 0;
static unsigned short 	stack[16] = { 0 };
static unsigned char 	SP = 0;

static unsigned char DT = 0;
static unsigned char ST = 0;


typedef void (*op_handler)(int);

void op_00XX(int opperand)
{
	int op = opperand & 0xff;
	if(op == 0xE0)
	{
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
void op_1XXX(int opperand)
{
	PC = opperand-1;
}

/*
 Jump to address + v0
*/
void op_BXXX(int opperand)
{
	PC = regs[0] + (opperand&0x0fff)-1;
}

void op_CXXX(int operand)
{
	int targetRegister = (operand&0xf00)>>8;
	int mask = operand & 0xff;
	regs[targetRegister] = (rand() % 256) & mask;
}

void op_2XXX(int opperand)
{
	if(SP<16)
	{
		stack[SP] = PC;
		PC = opperand-1;
		SP++;
	}
	else
	{
		printf("Attempted to jump 16 times\n");
	}
}

void op_3XXX(int opperand)
{
	if(regs[(opperand & 0x0f00) >> 8] == (opperand & 0x000f))
	{
		PC++;
	}
}

void op_4XXX(int opperand)
{
	if(regs[(opperand&0x0f00)>>8] != (opperand & 0x00ff))
	{
		PC++;
	}
}

void op_5XXX(int opperand)
{
	if(regs[(opperand&0x0f00)>>8] == regs[(opperand&0x00f0)>>4])
	{
		PC++;
	}
}

void op_6XXX(int opperand)
{
	regs[(opperand&0x0f00)>>8] = (opperand&0x00ff);
}

void op_7XXX(int opperand)
{
	regs[(opperand&0x0f00)>>8] += (opperand&0x00ff);
}

void op_8XXX(int opperand)
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
		if(regs[lhs] > regs[rhs] > 255)
			regs[15] = 1;
		else
			regs[15] = 0;
		regs[lhs] -= regs[rhs];
	}
	else if(subOp ==6)
	{
		if((regs[lhs]&0x01) == 1)
			regs[15] = 1;
		else
			regs[15] = 0;
		regs[lhs] /= 2;
	}
	else if(subOp == 7)
	{
		if(regs[lhs] < regs[rhs] > 255)
			regs[15] = 1;
		else
			regs[15] = 0;
		regs[rhs] -= regs[lhs];
	}
	else if(subOp == 0xE)
	{
		if((regs[lhs]&0x01) == 1)
			regs[15] = 1;
		else
			regs[15] = 0;
		regs[lhs] *= 2;
	}
}

void op_9XXX(int opperand)
{
	if(regs[(opperand&0x0f00)>>8] != regs[(opperand&0x00f0)>>4])
	{
		PC++;
	}
}

void op_AXXX(int opperand)
{
	I = opperand & 0x0fff;
}

void op_DXXX(int opperand)
{
	gfx_draw( regs[(opperand&0xf00)>>8], regs[(opperand&0x00f0)>>4],memory+I, opperand & 0x0f);			
}

void op_EXXX(int opperand)
{
	if((opperand&0xff)==0x9E)
	{
		if(keys[regs[(opperand&0x0f00)>>8]]>0)
		{
			PC++;
		}
	}
	else if((opperand&0xff)==0xA1) // No need to check but still..
	{
		if(keys[regs[(opperand&0x0f00)>>8]]==0)
		{
			PC++;
		}
	}
}

void op_FXXX(int opperand)
{
	if((opperand&0xff)==0x07)
	{
		regs[(opperand&0x0f00)>>8] = DT;
	}
	else if((opperand&0xff)==0x0A)
	{
		SDL_Event ev;
		ev.type = 0;
		while(ev.type!=SDL_KEYDOWN)
		{
			SDL_WaitEvent(&ev);
			if(ev.type == SDL_KEYDOWN)
			{
				// just one key right now TODO
				regs[(opperand&0x0f00)>>8] = 1;
			}
		}
		
	}
	else if((opperand&0xff)==0x15)
	{
		DT = regs[(opperand&0x0f00)>>8];
	}
	else if((opperand&0xff)==0x18)
	{
		ST = regs[(opperand&0x0f00)>>8];
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
		printf("Huhwah?\n");
	}
	else if((opperand&0xff)==0x55)
	{
		unsigned char *start = memory+0x200+I;
		int i =0;
		for(;i<(opperand&0x0f00)>>8;i++)
			*(start+i) = regs[i];
	}
	else if((opperand&0xff)==0x65)
	{
		unsigned char *start = memory+0x200+I;
		int i =0;
		for(;i<(opperand&0x0f00)>>8;i++)
			regs[i] = *(start+i);
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

static init_memory()
{
	unsigned char font[] = 
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0,
		0x20, 0x60, 0x20, 0x20, 0x70,
		0xF0, 0x10, 0xF0, 0x80, 0xF0,
		0xF0, 0x10, 0xF0, 0x10, 0xF0,
		0x90, 0x90, 0xF0, 0x10, 0x10,
		0xF0, 0x80, 0xF0, 0x10, 0xF0,
		0x90, 0x90, 0xF0, 0x10, 0x10, 
		0xF0, 0x80, 0xF0, 0x10, 0xF0,
		0xF0, 0x80, 0xF0, 0x90, 0xF0,
		0xF0, 0x10, 0x20, 0x40, 0x40,
		0xF0, 0x90, 0xF0, 0x90, 0xF0,
		0xF0, 0x90, 0xF0, 0x10, 0xF0, 
		0xF0, 0x90, 0xF0, 0x90, 0x90,
		0xE0, 0x90, 0xE0, 0x90, 0xE0,
		0xF0, 0x80, 0x80, 0x80, 0xF0,
		0xE0, 0x90, 0x90, 0x90, 0xE0,
		0xF0, 0x80, 0xF0, 0x80, 0xF0,
		0xF0, 0x80, 0xF0, 0x80, 0x80 
	};
	memcpy(font, memory,80);
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
	
	fclose(game);
	
	PC = 0;
	init_memory();
	return 0;
}

int parse_op()
{
	unsigned char *opbytes = ((unsigned char*)memory+0x200+PC+2);
#ifndef FLIP
	unsigned short op = (opbytes[0])+(opbytes[1]<<8);
#else
	unsigned short op = (opbytes[1])+(opbytes[0]<<8);
#endif

	printf("PC: %04d SP: %02d PARSING: %02X%02X - %02x %03x REGS: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",PC,SP, (op&0xff00)>>8, op&0xff, (op&0xf000)>>12, op&0x0fff, regs[0], regs[1], regs[2], regs[3], regs[4], regs[5], regs[6], regs[7], regs[8], regs[9], regs[10], regs[11], regs[12], regs[13], regs[14], regs[15]);
	fflush(stdout);

	int operation = (op & 0xf000) >> 12;
	
	opHandlerTable[operation](op&0x0fff);			
		
	PC++;
	if(DT>0)
		DT--;
	if(ST>0)
		ST--;
	return 1;
}

