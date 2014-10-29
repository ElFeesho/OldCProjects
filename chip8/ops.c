#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "ops.h"
#include "gfx.h"
#include "input.h"
#include "sound.h"

typedef void (*op_handler)(chip8_cpu_t *, short);

static const int PROGRAM_START_OFFSET = 0x200;
static const int CARRY_FLAG = 15;

static inline unsigned char nybbleOne(unsigned short value)
{
	return (value & 0xf000)>>12;
} 

static inline unsigned char nybbleTwo(unsigned short value)
{
	return (value & 0x0f00)>>8;
} 

static inline unsigned char nybbleThree(unsigned short value)
{
	return (value & 0x00f0)>>4;
} 

static inline unsigned char nybbleFour(unsigned short value)
{
	return (value & 0x000f);
} 

static inline unsigned char byteOne(unsigned short value)
{
	return (value & 0xff00) >> 8;
}

static inline unsigned char byteTwo(unsigned short value)
{
	return value & 0x00ff;
}

static inline void increment_pc(chip8_cpu_t *cpu)
{
	cpu->PC += 2;
}

void op_00XX(chip8_cpu_t *cpu, short opperand);
void op_1XXX(chip8_cpu_t *cpu, short opperand);
void op_2XXX(chip8_cpu_t *cpu, short opperand);
void op_3XXX(chip8_cpu_t *cpu, short opperand);
void op_4XXX(chip8_cpu_t *cpu, short opperand);
void op_5XXX(chip8_cpu_t *cpu, short opperand);
void op_6XXX(chip8_cpu_t *cpu, short opperand);
void op_7XXX(chip8_cpu_t *cpu, short opperand);
void op_8XXX(chip8_cpu_t *cpu, short opperand);
void op_8XX0(chip8_cpu_t *cpu, short opperand);
void op_8XX1(chip8_cpu_t *cpu, short opperand);
void op_8XX2(chip8_cpu_t *cpu, short opperand);
void op_8XX3(chip8_cpu_t *cpu, short opperand);
void op_8XX4(chip8_cpu_t *cpu, short opperand);
void op_8XX5(chip8_cpu_t *cpu, short opperand);
void op_8XX6(chip8_cpu_t *cpu, short opperand);
void op_8XX7(chip8_cpu_t *cpu, short opperand);
void op_8XXE(chip8_cpu_t *cpu, short opperand);

void op_9XXX(chip8_cpu_t *cpu, short opperand);
void op_AXXX(chip8_cpu_t *cpu, short opperand);
void op_BXXX(chip8_cpu_t *cpu, short opperand);
void op_CXXX(chip8_cpu_t *cpu, short opperand);
void op_DXXX(chip8_cpu_t *cpu, short opperand);
void op_EXXX(chip8_cpu_t *cpu, short opperand);
void op_FXXX(chip8_cpu_t *cpu, short opperand);
void unhandledOpcode(chip8_cpu_t *cpu, short opperand);

static op_handler baseOpHandlerTable[] = {
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

static op_handler arithmeticTable[] = {
	&op_8XX0,
	&op_8XX1,
	&op_8XX2,
	&op_8XX3,
	&op_8XX4,
	&op_8XX5,
	&op_8XX6,
	&op_8XX7,
	&unhandledOpcode,
	&unhandledOpcode,
	&unhandledOpcode,
	&unhandledOpcode,
	&unhandledOpcode,
	&unhandledOpcode,
	&op_8XXE,
	&unhandledOpcode
};

void op_8XX0(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[nybbleTwo(opperand)] = cpu->regs[nybbleThree(opperand)];
}

void op_8XX1(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[nybbleTwo(opperand)] |= cpu->regs[nybbleThree(opperand)];
}

void op_8XX2(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[nybbleTwo(opperand)] &= cpu->regs[nybbleThree(opperand)];
}

void op_8XX3(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[nybbleTwo(opperand)] ^= cpu->regs[nybbleThree(opperand)];
}

void op_8XX4(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[CARRY_FLAG] = (cpu->regs[nybbleTwo(opperand)] + cpu->regs[nybbleThree(opperand)] > 255);
	cpu->regs[nybbleTwo(opperand)] += cpu->regs[nybbleThree(opperand)];
}

void op_8XX5(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[CARRY_FLAG] = !(cpu->regs[nybbleThree(opperand)] > cpu->regs[nybbleTwo(opperand)]);
	cpu->regs[nybbleTwo(opperand)] -= cpu->regs[nybbleThree(opperand)];
}

void op_8XX6(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[CARRY_FLAG] = cpu->regs[nybbleThree(opperand)]&0x01;
	cpu->regs[nybbleTwo(opperand)] = cpu->regs[nybbleThree(opperand)] >> 1;
}

void op_8XX7(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[CARRY_FLAG] = !(cpu->regs[nybbleTwo(opperand)] > cpu->regs[nybbleThree(opperand)]);
	cpu->regs[nybbleTwo(opperand)] = cpu->regs[nybbleThree(opperand)] - cpu->regs[nybbleTwo(opperand)];
}

void op_8XXE(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[CARRY_FLAG] = (cpu->regs[nybbleThree(opperand)]&0x80) >> 7;
	cpu->regs[nybbleTwo(opperand)] = cpu->regs[nybbleThree(opperand)] << 1;
}

void unhandledOpcode(chip8_cpu_t *cpu, short opperand) 
{
	fprintf(stderr, "UNHANDLEDOPCODE: %X\n", opperand);
}

void op_00XX(chip8_cpu_t *cpu, short opperand)
{
	int op = opperand & 0xff;
	if(op == 0xE0)
	{
		gfx_cls();
	}
	else if(op == 0xEE)
	{
		cpu->SP--;
		cpu->PC = cpu->stack[cpu->SP];
	}
}

/*
 Jump to address
*/
void op_1XXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->PC = opperand & 0x0fff;
}

void op_2XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->SP<16)
	{
		cpu->stack[cpu->SP] = cpu->PC;
		cpu->PC = opperand;
		cpu->SP++;
	}
	else
	{
		printf("Attempted to jump 16 times\n");
	}
}

void op_3XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[nybbleTwo(opperand)] == byteTwo(opperand))
	{
		increment_pc(cpu);
	}
}

void op_4XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[nybbleTwo(opperand)] != byteTwo(opperand))
	{
		increment_pc(cpu);
	}
}

void op_5XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[nybbleTwo(opperand)] == cpu->regs[nybbleThree(opperand)])
	{
		increment_pc(cpu);
	}
}

void op_6XXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[nybbleTwo(opperand)] = byteTwo(opperand);
}

void op_7XXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->regs[nybbleTwo(opperand)] += byteTwo(opperand);
}

void op_8XXX(chip8_cpu_t *cpu, short opperand)
{
	arithmeticTable[nybbleFour(opperand)](cpu, opperand);
}

void op_9XXX(chip8_cpu_t *cpu, short opperand)
{
	if(cpu->regs[nybbleTwo(opperand)] != cpu->regs[nybbleThree(opperand)])
	{
		increment_pc(cpu);
	}
}

void op_AXXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->I = opperand & 0x0fff;
}

void op_BXXX(chip8_cpu_t *cpu, short opperand)
{
	cpu->PC = cpu->regs[0] + (opperand & 0x0fff);
}

void op_CXXX(chip8_cpu_t *cpu, short opperand)
{
	int targetRegister = nybbleTwo(opperand);
	int mask = byteTwo(opperand);
	cpu->regs[targetRegister] = (rand() % 256) & mask;
}

void op_DXXX(chip8_cpu_t *cpu, short opperand)
{
	int x = cpu->regs[nybbleTwo(opperand)];
	int y = cpu->regs[nybbleThree(opperand)];
	int count = nybbleFour(opperand);

	cpu->regs[CARRY_FLAG] = gfx_draw(x-1, y, cpu->memory+cpu->I, count)?1:0;
}

void op_EXXX(chip8_cpu_t *cpu, short opperand)
{
	int targetKey = cpu->regs[nybbleTwo(opperand)];
	int keydown = input_keydown(targetKey);

	if(byteTwo(opperand) == 0x9E && keydown)
	{
		increment_pc(cpu);
	}
	else if(byteTwo(opperand) == 0xA1 && !keydown)
	{
		increment_pc(cpu);
	}
}

void op_FXXX(chip8_cpu_t *cpu, short opperand)
{
	unsigned char subCommand = byteTwo(opperand);
	if(subCommand == 0x07)
	{
		cpu->regs[nybbleTwo(opperand)] = cpu->DT;
	}
	else if(subCommand == 0x0A)
	{
		gfx_flip();
		cpu->regs[nybbleTwo(opperand)] = input_readkey();
	}
	else if(subCommand == 0x15)
	{
		cpu->DT = cpu->regs[nybbleTwo(opperand)];
	}
	else if(subCommand == 0x18)
	{
		cpu->ST = cpu->regs[nybbleTwo(opperand)];
		sound_start();
	}
	else if(subCommand == 0x1E)
	{
		cpu->I += cpu->regs[nybbleTwo(opperand)];
	}
	else if(subCommand == 0x29)
	{
		cpu->I = 5 * (cpu->regs[nybbleTwo(opperand)]);
	}
	else if(subCommand == 0x33)
	{
		//BCD...
		int lhs = nybbleTwo(opperand);
		int target = cpu->regs[lhs];
		int hundreds = target / 100;
		target -= hundreds * 100;
		int tens = target / 10;
		target -= tens * 10;
		int units = target;
		cpu->memory[cpu->I] = (char)hundreds;
		cpu->memory[cpu->I+1] = (char)tens;
		cpu->memory[cpu->I+2] = (char)units;
	}
	else if(subCommand == 0x55)
	{
		for(int i = 0; i <= nybbleTwo(opperand);i++)
		{
			cpu->memory[cpu->I++] = cpu->regs[i];
		}
	}
	else if(subCommand == 0x65)
	{
		for(int i = 0; i<= nybbleTwo(opperand); i++)
		{
			cpu->regs[i] = cpu->memory[cpu->I++];
		}
	}
}


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
	memset(cpu->memory, 0, 4096);

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
	unsigned char opR = *((unsigned char*)cpu->memory+(cpu->PC + 1));
	increment_pc(cpu);
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
	int operation = nybbleOne(op);
	baseOpHandlerTable[operation](cpu, op & 0x0fff);

	return 1;
}
