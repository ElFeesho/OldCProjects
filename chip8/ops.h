#ifndef __OPS_H__
#define __OPS_H__

typedef struct chip8_cpu_t {
	unsigned short PC;
	unsigned char *memory;
	unsigned short I;
	unsigned char regs[16];
	unsigned short stack[16];
	unsigned char SP;
	unsigned char DT;
	unsigned char ST;
} chip8_cpu_t;

chip8_cpu_t *create_cpu();
int parse_op(chip8_cpu_t *cpu);
int load_game_from_file(chip8_cpu_t *cpu, char *fname);
int load_game(chip8_cpu_t *cpu, void *memory, int size);
void decrement_timers(chip8_cpu_t *cpu);
void destroy_cpu(chip8_cpu_t *cpu);

#endif
