#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui.h"

#include "asm.h"

static asm_var *variable_list;

static short ax, bx, cx, dx, di, bp, si, sp;

static short stack[0xfffe];

/* 16 Kilobytes of top-notch memory! */
static unsigned char memory[16384]; 

/* Memory offset, incremented each time a variable is made */
static unsigned int mem_offset = 0; 

static unsigned int pc = 0;

static char **prog = NULL;
static unsigned int prog_length = 0;

/* Helper funcs */

static void memory_add_var(char type, char *name, unsigned short size, void *data)
{
	if(variable_list == NULL)
	{
		variable_list = (asm_var*)g_malloc0(sizeof(variable_list));
		variable_list->name = (char*)g_strdup(name);
		variable_list->type = type;
		variable_list->size = size;
		variable_list->ptr = (void*)(memory+mem_offset);
		variable_list->sptr = mem_offset;
		variable_list->next = 0;
		/* Byte is only 1 well, byte, so only need to copy one, size can be ignored right now */
		memcpy(memory+mem_offset,data,size);
		mem_offset+=size;
	}
	else
	{
		asm_var *tmp = (asm_var*)g_malloc0(sizeof(variable_list));
		tmp->name = (char*)g_strdup(name);
		tmp->type = type;
		tmp->size = size;
		tmp->ptr = (void*)(memory+mem_offset);
		tmp->sptr = mem_offset;
		tmp->next = variable_list;
		variable_list = tmp;
		/* Copy the data to the memory */
		memcpy(memory+mem_offset,data,size);
		mem_offset+=size;
	}
}

static short asm_convert_number(char *string, int *not_num)
{
	short return_val;
	printf("End char is %c\n",string[strlen(string)-1]);
	if(isalpha(string[strlen(string)-1]))
	{
		int base = 0;
		switch(string[strlen(string)-1])
		{
			case 'b':
			case 'B':
				base = 2;
				break;
			case 'h':
			case 'H':
				base = 16;
				break;
			default:
				base = 10;
				break;
		}
		printf("Converting: %s to a number\n",string);
		char *endptr;
		string[strlen(string)]='\0';
		return_val = strtol(string,&endptr,base);
		if(endptr==string)
		{
			fprintf(stderr, "Failed parsing number\n");
			*not_num = 1;
		}
	}
	else
	{
		if(string[strlen(string)-1]=='$')
		{
			*not_num = 1;
			return 0;
		}
		return_val = atoi(string);
	}
	return return_val;
}

static char *asm_get_operand(char *string)
{
	char *operand = malloc(strlen(string)+1);
	memset(operand, 0, strlen(string)+1);
	int i;
	int started = 0;
	int j = 0;
	int is_string = 0;
	for(i = 0; i<strlen(string);i++)
	{
		/* Miss any starting spaces or commas */
		if((isspace(string[i]) || string[i] == ',') && !started)
			continue;
		
		if(!is_string)
		{
			if(!isspace(string[i]) && string[i]!=',')
			{
				started = 1;
				if(string[i]=='\'')
				{
					is_string = 1;
					continue;
				}
				operand[j++] = string[i];
			}
		}
		else
		{
			if(string[i] == '\'' && string[i-1]!='\\')
				break;
			operand[j++] = string[i];
			continue;
		}
		
		/* If we hit a space or a comma break the loop! */
		if(isspace(string[i]) || string[i] == ',' && started)
			break;
	}
	return operand;
}

void asm_get_regs(unsigned short *reg_cont)
{
	reg_cont[0] = ax;
	reg_cont[1] = bx;
	reg_cont[2] = cx;
	reg_cont[3] = dx;
	reg_cont[4] = di;
	reg_cont[5] = bp;
	reg_cont[6] = si;
	reg_cont[7] = sp;
}

asm_var *asm_get_var_list(void)
{
	return variable_list;
}


void asm_reset_vm(void)
{
	ax = 0;
	bx = 0;
	cx = 0;
	dx = 0;
	di = 0;
	bp = 0;
	si = 0;
	sp = 0xfffe;
	pc = 0;
	gui_update();
}

static void asm_var_list_clear()
{
	/* It sucks I know :( */
	variable_list = NULL;
	mem_offset = 0;
}

void asm_set_prog(char **aprog, unsigned int lines)
{
	/* Wipe the Assembler's memory clear */
	memset(memory, '0', 16384);
	
	/* Remove the variables in the list */
	asm_var_list_clear();
	if(prog!=NULL)
	{
		/* Free the last asm program */
		int i;
		for(i = 0;i<prog_length;i++)
		{
			free(prog[i]);
		}
		/* Free the memory allocated for storing pointers to pointers */
		free(prog);
	}
	prog = aprog;
	prog_length = lines;
	
	/*
		First sweep, check for errors, and add variables to the variable linked list
	*/
	
	int i;
	for(i = 0;i<lines;i++)
	{
		printf("Examining %s\n",prog[i]);
		char *op1 = asm_get_operand(prog[i]);
		/* Not a label :D */
		if(strcmp(op1,"add")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"sub")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"inc")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"dec")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"mul")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"div")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"mov")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"jmp")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strcmp(op1,"hlt")==0)
		{
			printf("Op1: %s\n",op1);
		}
		else if(strlen(op1)==strlen(prog[i]))
		{
			/* Label ? */
			printf("Found a label? %s\n",prog[i]);
		}
		else
		{
			printf("Found a possible variable :O! %s\n",op1);
			char *type = asm_get_operand(prog[i]+strlen(op1)+1);
			printf("Type: %s\n",type);
			char *value = asm_get_operand(prog[i]+strlen(op1)+strlen(type)+1);
			printf("Value: %s\n",value);
			int not_num = 0;
			if(strcasecmp(type,"db")==0)
			{
				char bvalue = 0;
				bvalue = asm_convert_number(value, &not_num);
				printf("BVALUE: %d and NOTNUM: %d\n",bvalue, not_num);
				if(not_num)
				{
					printf("Adding string\n");
					if(value[strlen(value)-1]=='$')
						memory_add_var(0, op1, strlen(value)+1, value);
					else
						fprintf(stderr,"8086 spec doesn't allow non dollar terminated strings!\n");
				}
				else
				{
					printf("Adding byte\n");
					memory_add_var(0, op1, 1, &bvalue);
				}
			}
			if(strcasecmp(type,"dw")==0)
			{
				short wvalue = 0;
				wvalue = asm_convert_number(value, &not_num);
				if(not_num)
				{
					fprintf(stderr, "Unrecognized dw number\n");
				}
				memory_add_var(1, op1, 2, &wvalue);
			}
		}
	}
}

static unsigned int asm_find_label(char *label)
{
	int i = 0;
	for(i = 0;i<prog_length;i++)
	{
		printf("%s == %s?\n",prog[i], label);
		if(strncmp(prog[i],label,strlen(label)) == 0)
		{
			printf("found the label!\n");
			if(prog[i][strlen(label)]==':')
			{
				return i;
			}
			printf("Didn't find the semi-colon :(\n");
		}
	}
	printf("Couldn't find loop!\n");
	return 1;
}

static short asm_lookup_reg(char *name, int *not_found)
{
	/* Full 16bit Register */
	if(strcasecmp(name,"ax")==0)
	{
		return ax;
	}
	if(strcasecmp(name,"bx")==0)
	{
		return bx;
	}
	if(strcasecmp(name,"cx")==0)
	{
		return cx;
	}
	if(strcasecmp(name,"dx")==0)
	{
		return dx;
	}
	/* High 8bit registers */
	if(strcasecmp(name,"ah")==0)
	{
		return ax>>8;
	}
	if(strcasecmp(name,"bh")==0)
	{
		return bx>>8;
	}
	if(strcasecmp(name,"ch")==0)
	{
		return cx>>8;
	}
	if(strcasecmp(name,"dh")==0)
	{
		return dx>>8;
	}
	/* Low 8bit register */
	if(strcasecmp(name,"al")==0)
	{
		return ax&0xff;
	}
	if(strcasecmp(name,"bl")==0)
	{
		return bx&0xff;
	}
	if(strcasecmp(name,"cl")==0)
	{
		return cx&0xff;
	}
	if(strcasecmp(name,"dl")==0)
	{
		return dx&0xff;
	}

	if(not_found)
		*not_found = 1;
	return 0;
}

static short asm_lookup_var(char *name, int *not_found)
{
	/*TODO Look up vars here! */
	return 0;
}

static void asm_set_var(char *name, void *data)
{

}

void asm_set_reg(char *name, short data)
{
	printf("Setting %s to %d\n",name, data);
	if(strcasecmp(name,"ax")==0)
	{
		ax = data;
	}
	if(strcasecmp(name,"bx")==0)
	{
		bx = data;
	}
	if(strcasecmp(name,"cx")==0)
	{
		cx = data;
	}
	if(strcasecmp(name,"dx")==0)
	{
		dx = data;
	}
	
	if(strcasecmp(name,"ah")==0)
	{
		ax = ax&0x00ff;
		ax += ((char)data)<<8;
	}
	if(strcasecmp(name,"bh")==0)
	{
		bx = bx&0x00ff;
		bx += ((char)data)<<8;
	}
	if(strcasecmp(name,"ch")==0)
	{
		cx = cx&0x00ff;
		cx += ((char)data)<<8;
	}
	if(strcasecmp(name,"dh")==0)
	{
		dx = dx&0x00ff;
		dx += ((char)data)<<8;
	}

	if(strcasecmp(name,"al")==0)
	{
		ax = ax&0xff00;
		ax += ((char)data);
	}
	if(strcasecmp(name,"bl")==0)
	{
		bx = bx&0xff00;
		bx += ((char)data);
	}
	if(strcasecmp(name,"cl")==0)
	{
		cx = cx&0xff00;
		cx += ((char)data);
	}
	if(strcasecmp(name,"dl")==0)
	{
		dx = dx&0xff00;
		dx += ((char)data);
	}
	
	/*TODO Look up vars here! */
}

int asm_step()
{
	gui_update();
	/* Find out which opcode we are dealing with */
	
	/* Adding */
	if(strncmp(prog[pc],"add",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		char *op2 = asm_get_operand(((prog[pc])+4)+strlen(op1));
		
		int not_found = 0, not_number = 0;
		short cval = asm_lookup_reg(op1,&not_found);
		
		if(not_found)
		{
			fprintf(stderr, "Couldn't find register to add to\n");
			return -1;
		}
		short cval2 = asm_lookup_reg(op2, &not_found);
		if(not_found)
		{
			cval2 = asm_convert_number(op2,&not_number);
			if(not_number)
			{
				fprintf(stderr, "Second operand is not a numeric or variable value\n");
				return -1;
			}
		}
		cval += cval2;
		asm_set_reg(op1, cval);
	}
	/* Subtracting */
	else if(strncmp(prog[pc],"sub",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		char *op2 = asm_get_operand(((prog[pc])+4)+strlen(op1));
		
		int not_found = 0, not_number = 0;
		short cval = asm_lookup_reg(op1,&not_found);
		
		if(not_found)
		{
			fprintf(stderr, "Couldn't find register to subtract from\n");
			return -1;
		}
		short cval2 = asm_lookup_reg(op2, &not_found);
		if(not_found)
		{
			cval2 = asm_convert_number(op2,&not_number);
			if(not_number)
			{
				fprintf(stderr, "Second operand is not a numeric or variable value\n");
				return -1;
			}
		}
		cval -= cval2;
		asm_set_reg(op1, cval);
	}
	/* Incrementing */
	else if(strncmp(prog[pc],"inc",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		/* Check that the operator is a reg or a variable */
		
		int not_found = 0;
		
		short cval = asm_lookup_reg(op1,&not_found);
		if(not_found)
		{
			fprintf(stderr, "Failed to find register/variable to increment\n");
			return -1;
		}
		cval++;
		asm_set_reg(op1, cval);
	}
	/* Decrementing */
	else if(strncmp(prog[pc],"dec",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		/* Check that the operator is a reg or a variable */
		int not_found = 0;
		short cval = asm_lookup_reg(op1, &not_found);
		if(not_found)
		{
			fprintf(stderr, "Failed to find register/variable to decrement\n");
			return -1;
		}
		cval--;
		asm_set_reg(op1, cval);
	}
	/* Moving */
	else if(strncmp(prog[pc],"mov",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		char *op2 = asm_get_operand(((prog[pc])+4)+strlen(op1));
		
		int data = 0;
		int not_found = 0;
		if(isalpha(op2[0]))
		{
			/* It's a variable or register so lets look up the value of it! */
			data = asm_lookup_reg(op2,&not_found);
		}
		if(!isalpha(op2[0])||not_found)
		{
			int not_number = 0;
			data = asm_convert_number(op2, &not_number);
			if(not_number)
			{
				fprintf(stderr, "Failed to find a value to move into register\n");
				return -1;
			}
		}
		asm_set_reg(op1, data);
	}
	/* Multiply */
	else if(strncmp(prog[pc],"mul",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		int not_found = 0;
		unsigned short val = asm_lookup_reg("al",NULL);
		unsigned short val2 = asm_lookup_reg(op1,&not_found);
		if(not_found)
		{
			fprintf(stderr, "Failed to find register to multiply by\n");
			return -1;
		}
		asm_set_reg("ax",val*val2);
	}
	/* Divide */
	else if(strncmp(prog[pc],"div",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		int not_found = 0;
		unsigned short val = asm_lookup_reg("ax",NULL);
		unsigned short val2 = asm_lookup_reg(op1,&not_found);
		if(not_found)
		{
			fprintf(stderr, "Failed to find register to divide by\n");
		}
		asm_set_reg("al",val/val2);
		asm_set_reg("ah",val%val2);
	}
	/* Jumping */
	else if(strncmp(prog[pc],"jmp",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		pc = asm_find_label(op1);
	}
	/* Interrupt */
	else if(strncmp(prog[pc],"int",3)==0)
	{
		char *op1 = asm_get_operand(((prog[pc])+4));
		int int_num = atoi(op1);
		pc = asm_find_label(op1);
	}
	/* Halting */
	else if(strncmp(prog[pc],"hlt",3)==0)
	{
		pc = 0;
		return -1;
	}
	pc++;
	if(pc>=prog_length)
	{
		printf("Shouldn't of got this far!\n");
		return -1;
	}
	/* On a Hlt return 0 */
	return pc;
}
