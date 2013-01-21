#ifndef __ASM_F_H__
#define __ASM_F_H__

void asm_reset_vm(void);
void asm_set_prog(char **aprog, unsigned int lines);
int asm_step();
void asm_get_regs(unsigned short *reg_cont);

/*
	Variable linked list system
*/

typedef struct
{
	char *name;
	char type;
	unsigned short size;
	void *ptr;
	int sptr;
	void *next;
} asm_var;


asm_var *asm_get_var_list();

#endif
