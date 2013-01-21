#ifndef __OPS_H__
#define __OPS_H__

extern unsigned char memory[4096];
extern unsigned short I;
extern unsigned char regs[16];

extern unsigned short stack[16];

extern int parse_op();

extern int load_game(char *fname);

#endif
