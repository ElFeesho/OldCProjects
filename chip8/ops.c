#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "ops.h"
#include "gfx.h"
#include "input.h"
#include "sound.h"

static const int 		PROGRAM_START_OFFSET = 0x200;

typedef void (*op_handler)(chip8_cpu_t *, short);

void op_00XX(chip8_cpu_t *cpu, short opperand)
{
	int op = opperand & 0xff;
	if(op == 0xE0)
	{
		printf("CLEARING SCREEN\n");
		gfx_cls();
	}
	else if(op == 0xEE)
	{
		int sp = cpu->SP;
		cpu->SP -= 1;
		cpu->PC = cpu->stack[sp];
		printf("POPPED STACK: %d\n", cpu->SP);
	}
}

/*
 Jump to address
*/
void op_1XXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->PC = opperand;
}

/*
 Jump to address + v0
*/
void op_BXXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->PC = cpu->regs[0] + (opperand&0x0fff);
}

void op_CXXX(chip8_cpu_t *cpu, short operand)
{
	int targetRegister = (operand&0xf00)>>8;
	int mask = operand & 0xff;
	cpu->regs[targetRegister] = (rand() % 256) & mask;
}

void op_2XXX(chip8_cpu_t *cpu, short opperand)
{
	printf("Attempting to jump: %d\n", cpu->SP);
	if(cpu->SP<16)
	{
		cpu->stack[cpu->SP] = cpu->PC;
		cpu->PC = opperand;
		cpu->SP++;

		printf("PUSHED STACK: %d\n", cpu->SP);
	}
	else
	{
		printf("Attempted to jump 16 times\n");
	}
}

void op_3XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[(opperand & 0x0f00) >> 8] == (opperand & 0x000f))
	{
		cpu->PC += 2;
	}
}

void op_4XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[(opperand&0x0f00)>>8] != (opperand & 0x00ff))
	{
		cpu->PC += 2;
	}
}

void op_5XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[(opperand&0x0f00)>>8] == cpu->regs[(opperand&0x00f0)>>4])
	{
		cpu->PC += 2;
	}
}

void op_6XXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[(opperand&0x0f00)>>8] = (opperand&0x00ff);
}

void op_7XXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[(opperand&0x0f00)>>8] += (opperand&0x00ff);
}

void op_8XXX(chip8_cpu_t *cpu, short opperand)
{
	int subOp = opperand & 0x0f;
	int lhs = (opperand & 0x0f00)>>8;
	int rhs = (opperand & 0x00f0)>>4;

	if(subOp == 0)
	{
		cpu->regs[lhs] = cpu->regs[rhs];
	}
	else if(subOp == 1)
	{
		cpu->regs[lhs] |= cpu->regs[rhs];
	}
	else if(subOp == 2)
	{
		cpu->regs[lhs] &= cpu->regs[rhs];
	}
	else if(subOp == 3)
	{
		cpu->regs[lhs] ^= cpu->regs[rhs];
	}
	else if(subOp == 4)
	{
		if(cpu->regs[lhs] + cpu->regs[rhs] > 255)
		{
			cpu->regs[15] = 1;
		}
		else
		{
			cpu->regs[15] = 0;
		}

		cpu->regs[lhs] += cpu->regs[rhs];
	}
	else if(subOp == 5)
	{
		if(cpu->regs[rhs] > cpu->regs[lhs])
		{
			cpu->regs[15] = 0;
		}
		else
		{
			cpu->regs[15] = 1;
		}

		cpu->regs[lhs] -= cpu->regs[rhs];
	}
	else if(subOp == 6)
	{
		cpu->regs[15] = cpu->regs[lhs]&0x01;

		cpu->regs[lhs] = cpu->regs[rhs] >> 1;
	}
	else if(subOp == 7)
	{
		if(cpu->regs[lhs] > cpu->regs[rhs])
		{
			cpu->regs[15] = 0;
		}
		else
		{
			cpu->regs[15] = 1;
		}

		cpu->regs[lhs] = cpu->regs[rhs] - cpu->regs[lhs];
	}
	else if(subOp == 0xE)
	{

		cpu->regs[15] = (cpu->regs[lhs]&0x80) >> 7;

		cpu->regs[lhs] = cpu->regs[rhs] << 1;
	}
}

void op_9XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[(opperand&0x0f00)>>8] != cpu->regs[(opperand&0x00f0)>>4])
	{
		cpu->PC += 2;
	}
}

void op_AXXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->I = opperand & 0x0fff;
}

void op_DXXX(chip8_cpu_t *cpu, short opperand)
{
	int x = cpu->regs[(opperand & 0xf00) >> 8];
	int y = cpu->regs[(opperand & 0x00f0) >> 4];
	int count = opperand & 0x0f;
	cpu->regs[15] = gfx_draw(x, y, cpu->memory+cpu->I, count)?1:0;
}

void op_EXXX(chip8_cpu_t *cpu, short opperand)
{
	int targetKey = cpu->regs[(opperand&0x0f00)>>8];
	int keydown = input_keydown(targetKey);
	if((opperand&0xff)==0x9E)
	{
		if(keydown)
		{
			cpu->PC += 2;
		}
	}
	else if((opperand&0xff)==0xA1)
	{
		if(!keydown)
		{
			cpu->PC += 2;
		}
	}
}

void op_FXXX(chip8_cpu_t *cpu, short opperand)
{
	if((opperand&0xff)==0x07)
	{
		cpu->regs[(opperand&0x0f00)>>8] = cpu->DT;
	}
	else if((opperand&0xff)==0x0A)
	{
		int key = (opperand & 0x0f00) >> 8;
		SDL_Flip(SDL_GetVideoSurface());
		cpu->regs[key] = input_readkey();
	}
	else if((opperand&0xff)==0x15)
	{
		cpu->DT = cpu->regs[(opperand&0x0f00)>>8];
	}
	else if((opperand&0xff)==0x18)
	{
		cpu->ST = cpu->regs[(opperand&0x0f00)>>8];
		sound_start();
	}
	else if((opperand&0xff)==0x1E)
	{
		cpu->I += cpu->regs[(opperand&0x0f00)>>8];
	}
	else if((opperand&0xff)==0x29)
	{
		// Sprite stuff
		cpu->I = 5*(cpu->regs[(opperand&0x0f00)>>8]);
	}
	else if((opperand&0xff)==0x33)
	{
		//BCD...
		int lhs = (opperand & 0x0f00) >> 8;
		int target = cpu->regs[lhs];
		int hundreds = target / 100;
		target -= hundreds * 100;
		int tens = target / 10;
		target -= tens * 10;
		int units = target;
		cpu->memory[cpu->I] = (char)hundreds;
		cpu->memory[cpu->I+1] = (char)tens;
		cpu->memory[cpu->I+2] = (char)units;

		printf("BCD %d to %d %d %d\n", cpu->regs[lhs], hundreds, tens, units);
	}
	else if((opperand&0xff)==0x55)
	{
		unsigned char *start = cpu->memory + cpu->I;
		int i = 0;
		for(;i<=(opperand&0x0f00)>>8;i++)
		{
			*(start+i) = cpu->regs[i];
		}

		cpu->I += ((opperand&0x0f00)>>8) + 1;
	}
	else if((opperand&0xff)==0x65)
	{
		unsigned char *start = cpu->memory+cpu->I;
		int i = 0;
		for(;i<=(opperand&0x0f00)>>8;i++)
		{
			cpu->regs[i] = *(start+i);
		}

		cpu->I += ((opperand&0x0f00)>>8) + 1;
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

static void init_memory(chip8_cpu_t *cpu)
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
	memcpy(cpu->memory, font, 80);
}

chip8_cpu_t *create_cpu()
{
	chip8_cpu_t *cpu = malloc(sizeof(chip8_cpu_t));
	memset(cpu, 0, sizeof(chip8_cpu_t));
	cpu->memory = malloc(4096);
	cpu->SP = 0;
	cpu->DT = 0;
	cpu->ST = 0;
	cpu->I = 0;

	for(int i = 0; i < 16; i++)
	{
		cpu->regs[i] = 0;
		cpu->stack[i] = 0;
	}

	cpu->PC = PROGRAM_START_OFFSET;
	init_memory(cpu);

	return cpu;
}

void destroy_cpu(chip8_cpu_t *cpu)
{
	free(cpu->memory);
	free(cpu);
}

int load_game_from_file(chip8_cpu_t *cpu, char *fname)
{
	FILE *game = fopen(fname, "rb");
	if(!game)
	{
		return -1;
	}

	size_t bytes_read = 0;

	while(!feof(game))
	{
		bytes_read += fread(cpu->memory + bytes_read + PROGRAM_START_OFFSET, 1, 1024, game);
	}

	printf("Read: %u bytes\n", (unsigned int)bytes_read);

	int i = 0;
	for(i = 0; i < bytes_read; i++)
	{
		printf("%02x ", cpu->memory[PROGRAM_START_OFFSET + i]);
	}
	printf("\n");
	fclose(game);
	return 0;
}

int load_game(chip8_cpu_t *cpu, void *game, int size)
{
	memcpy(cpu->memory+PROGRAM_START_OFFSET, game, size);
	return 0;
}

unsigned short get_op(chip8_cpu_t *cpu)
{
	unsigned char opL = *((unsigned char*)cpu->memory+(cpu->PC));
	unsigned char opR = *((unsigned char*)cpu->memory+(cpu->PC+1));
	cpu->PC += 2;
	return ((short)opL<<8) + opR;
}

void decrement_timers(chip8_cpu_t *cpu)
{
	if(cpu->DT > 0)
	{
		cpu->DT--;
	}

	if(cpu->ST > 0)
	{
		cpu->ST--;
	}
	else
	{
		sound_stop();
	}
}

int parse_op(chip8_cpu_t *cpu)
{
	unsigned short op = get_op(cpu);

	int operation = (op & 0xf000) >> 12;

	opHandlerTable[operation](cpu, op&0x0fff);

	return 1;
}
