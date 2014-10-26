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

void op_1XXX(int opperand)
{
	PC = opperand-1;
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

op_handler opHandlerTable[] = {
	&op_00XX,
	&op_1XXX,
	&op_2XXX,
	&op_3XXX
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

	//printf("PC: %04d SP: %02d PARSING: %02X%02X - %02x %03x REGS: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",PC,SP, (op&0xff00)>>8, op&0xff, (op&0xf000)>>12, op&0x0fff, regs[0], regs[1], regs[2], regs[3], regs[4], regs[5], regs[6], regs[7], regs[8], regs[9], regs[10], regs[11], regs[12], regs[13], regs[14], regs[15]);
	fflush(stdout);
	int operation = (op & 0xf000) >> 12;

	switch(operation)
	{
		case 0: 
		case 1:
		case 2:
		case 3:
			opHandlerTable[operation](op&0x0fff);
			break;
		case 4:
			if(regs[(op&0x0f00)>>8] != (op & 0x00ff))
				PC++;
			break;
		case 5:
			if((op&0x000f)==0)
			{
				if(regs[(op&0x0f00)>>8] == regs[(op&0x00f0)>>4])
					PC++;
			}
			else
			{
				printf("Unhandled opcode variation\n");
				return 1;
			}
			break;
		case 6:
			regs[(op&0x0f00)>>8]=(op&0x00ff);
			break;
		case 7:
			regs[(op&0x0f00)>>8]+=(op&0x00ff);
			break;
		case 8:
			if((op&0x0f)==0)
			{
				regs[(op&0x0f00)>>8] = regs[(op&0x00f0)>>4];
			}
			else if((op&0x0f)==1)
			{
				regs[(op&0x0f00)>>8] |= regs[(op&0x00f0)>>4];
			}
			else if((op&0x0f)==2)
			{
				regs[(op&0x0f00)>>8] &= regs[(op&0x00f0)>>4];
			}
			else if((op&0x0f)==3)
			{
				regs[(op&0x0f00)>>8] ^= regs[(op&0x00f0)>>4];
			}
			else if((op&0x0f)==4)
			{
				if(regs[(op&0x0f00)>>8] + regs[(op&0x00f0)>>4] > 255)
					regs[15] = 1;
				else
					regs[15] = 0;
				regs[(op&0x0f00)>>8] += regs[(op&0x00f0)>>4];
			}
			else if((op&0x0f)==5)
			{
				if(regs[(op&0x0f00)>>8] > regs[(op&0x00f0)>>4] > 255)
					regs[15] = 1;
				else
					regs[15] = 0;
				regs[(op&0x0f00)>>8] -= regs[(op&0x00f0)>>4];
			}
			else if((op&0x0f)==6)
			{
				if((regs[(op&0x0f00)>>8]&0x01) == 1)
					regs[15] = 1;
				else
					regs[15] = 0;
				regs[(op&0x0f00)>>8] /= 2;
			}
			else if((op&0x0f)==7)
			{
				if(regs[(op&0x0f00)>>8] < regs[(op&0x00f0)>>4] > 255)
					regs[15] = 1;
				else
					regs[15] = 0;
				regs[(op&0x00f0)>>4] -= regs[(op&0x0f00)>>8];
			}
			else if((op&0x0f)==0xE)
			{
				if((regs[(op&0x0f00)>>8]&0x01) == 1)
					regs[15] = 1;
				else
					regs[15] = 0;
				regs[(op&0x0f00)>>8] *= 2;
			}
			break;
		case 9:
			if(regs[(op&0x0f00)>>8] != regs[(op&0x00f0)>>4])
				PC++;
			break;
		case 0xA: // Set I
			I = op&0x0fff;
			break;
		case 0xB:
			PC = regs[0]+(op&0x0fff)-1; // -1 for the PC++ at the end of the function
			break;
		case 0xC:
			regs[(op&0xf00)>>8] = (rand()%256)&(op&0x00ff);
			break;
		case 0xD:
			gfx_draw(regs[(op&0xf00)>>8],regs[(op&0x00f0)>>4],memory+I,op&0x0f);
			break;
		// INPUT
		case 0xE:
			if((op&0xff)==0x9E)
			{
				if(keys[regs[(op&0x0f00)>>8]]>0)
					PC++;
			}
			else if((op&0xff)==0xA1) // No need to check but still..
			{
				if(keys[regs[(op&0x0f00)>>8]]==0)
					PC++;
			}
			break;
		case 0xF: 
			if((op&0xff)==0x07)
			{
				regs[(op&0x0f00)>>8] = DT;
			}
			if((op&0xff)==0x0A)
			{
				SDL_Event ev;
				ev.type = 0;
				while(ev.type!=SDL_KEYDOWN)
				{
					SDL_WaitEvent(&ev);
					if(ev.type == SDL_KEYDOWN)
					{
						// just one key right now TODO
						regs[(op&0x0f00)>>8] = 1;
					}
				}
				
			}
			if((op&0xff)==0x15)
			{
				DT = regs[(op&0x0f00)>>8];
			}
			if((op&0xff)==0x18)
			{
				ST = regs[(op&0x0f00)>>8];
			}
			else if((op&0xff)==0x1E)
			{
				I += regs[(op&0x0f00)>>8];
			}
			else if((op&0xff)==0x29)
			{
				// Sprite stuff
				I = 5*(regs[(op&0x0f00)>>8]);
			}
			else if((op&0xff)==0x33)
			{
				//BCD...
				printf("Huhwah?\n");
			}
			else if((op&0xff)==0x55)
			{
				unsigned char *start = memory+0x200+I;
				int i =0;
				for(;i<(op&0x0f00)>>8;i++)
					*(start+i) = regs[i];
			}
			else if((op&0xff)==0x65)
			{
				unsigned char *start = memory+0x200+I;
				int i =0;
				for(;i<(op&0x0f00)>>8;i++)
					regs[i] = *(start+i);
			}
			break;
		default:
			printf("Unhandled op code: %X\n", op>>12);
			return 0;
			break;
	}
	PC++;
	if(DT>0)
		DT--;
	if(ST>0)
		ST--;
	return 1;
}

