#include "ops.h"
#include <stdio.h>
#include <string.h>

static inline short TO_OP(short op)
{
	int test = 1;
	if(((char*)&test)[0] == 1)
	{
		// Convert LE to BE
		char *operation = (char*)&op;
		operation[0] ^= operation[1];
		operation[1] ^= operation[0];
		operation[0] ^= operation[1];
	}
	return op;
}

static void assertEquals(int expected, int actual, const char *message, const char *func)
{
	if(expected != actual)
	{
		fprintf(stderr, "%s: %s\n", func, message);
		fprintf(stderr, "Expected: %d (%x) actual: %d (%x)\n", expected, expected, actual, actual);
		printf("\nF\n");
	}
	else
	{
		printf(".");
	}
}

static void assertTrue(int value, const char *message, const char *func)
{
	if(!value)
	{
		fprintf(stderr, "%s: %s\n", func, message);
		printf("\nF\n");
	}
	else
	{
		printf(".");
	}
}

static void assertNotNull(void *ptr, const char *message, const char *func)
{
	if(ptr == NULL)
	{
		fprintf(stderr, "%s: %s\n", func, message);
		printf("\nF\n");
	}
	else
	{
		printf(".");
	}
}

void test_aCpuCanBeCreated_withCorrectPCDefaultValue()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x200, cpu->PC, "Program counter incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withCorrectIDefaultValue()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x0, cpu->I, "I register incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withCorrectRegisterDefaultValues()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x0, cpu->regs[0], "Register 0 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[1], "Register 1 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[2], "Register 2 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[3], "Register 3 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[4], "Register 4 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[5], "Register 5 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[6], "Register 6 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[7], "Register 7 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[8], "Register 8 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[9], "Register 9 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[10], "Register 10 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[11], "Register 11 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[12], "Register 12 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[13], "Register 13 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[14], "Register 14 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->regs[15], "Register 15 value incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withCorrectStackDefaultValues()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x0, cpu->stack[0], "Stack 0 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[1], "Stack 1 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[2], "Stack 2 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[3], "Stack 3 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[4], "Stack 4 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[5], "Stack 5 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[6], "Stack 6 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[7], "Stack 7 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[8], "Stack 8 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[9], "Stack 9 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[10], "Stack 10 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[11], "Stack 11 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[12], "Stack 12 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[13], "Stack 13 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[14], "Stack 14 value incorrect", __FUNCTION__);
	assertEquals(0x0, cpu->stack[15], "Stack 15 value incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}


void test_aCpuCanBeCreated_withCorrectSTDefaultValue()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x0, cpu->ST, "Sound Timer value incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withCorrectDTDefaultValue()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x0, cpu->DT, "Delay Timer value incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withCorrectSPDefaultValue()
{
	chip8_cpu_t *cpu = create_cpu();
	assertEquals(0x0, cpu->SP, "Stack Pointer value incorrect", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withMemoryAllocated()
{
	chip8_cpu_t *cpu = create_cpu();
	assertNotNull(cpu->memory, "Memory is null", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aCpuCanBeCreated_withMemoryDefaultValue()
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
	chip8_cpu_t *cpu = create_cpu();
	assertTrue(memcmp(font, cpu->memory, 80) == 0, "Font not loaded into memory", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_aLoadedProgram_LoadsAtTheCorrectAddressInMemory()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = 0x8008;
	load_game(cpu, (void*)&op, 2);
	assertTrue(memcmp((void*)&op, cpu->memory+0x200, 2) == 0, "Game not loaded into memory", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_whenDelayTimeIsZero_DecrementingTimersDoesNotAffectDelayTimer()
{
	chip8_cpu_t *cpu = create_cpu();
	decrement_timers(cpu);
	assertEquals(0, cpu->DT, "Delay timer is not 0", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_whenDelayTimeIsNonZero_DecrementingTimersResultsInDelayTimer()
{
	chip8_cpu_t *cpu = create_cpu();
	cpu->DT = 10;
	decrement_timers(cpu);
	assertEquals(9, cpu->DT, "Delay timer is not 0", __FUNCTION__);
	destroy_cpu(cpu);
}

/*
Instruction tests 
*/

void test_whenOp_1NNN_isInvoked_PCRegisterUpdatesToNNN()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x1123);
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	assertEquals(0x123, cpu->PC, "PC should be 0x123", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_whenOp_BNNN_isInvoked_PCRegisterUpdatesToNNN_Plus_Register_1()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xB002);
	cpu->regs[0] = 0x2;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x4, cpu->PC, "PC should be 0x4", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_whenOp_2NNN_isInvoked_PCRegisterUpdatesToNNN()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x2123);

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x123, cpu->PC, "PC should be 0x4", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_whenOp_2NNN_isInvoked_TheAddressOfTheNextInstructionIsStoredInTheCallStack()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x2123);

	load_game(cpu, (void*)&op, 2);

	short nextInstructionAddress = cpu->PC + 2;

	parse_op(cpu);

	short stackContents = cpu->stack[0];
	
	assertEquals(nextInstructionAddress, stackContents, "Stack contents does not match expected return address", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_whenOp_2NNN_isInvoked_TheStackPointerIsIncremented()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x2123);

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(1, cpu->SP, "Stack pointer does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_whenOp_00EE_isInvoked_TheStackPointerIsDecremented()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x00EE);
	cpu->SP = 1;


	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0, cpu->SP, "Stack pointer does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_whenOp_00EE_isInvoked_ThePCRegisterContainsTheAddressOfTheNextInstruction_FromTheStack()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x00EE);
	cpu->SP = 1;
	cpu->stack[0] = 0x123;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x123, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_whenOp_3XNN_isInvoked_GivenRegisterX_EqualsNN_ThePCIsIncrementedToSkipTheNextInstruction()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x30ff);
	cpu->regs[0] = 0xff;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

typedef void (*testFunc)();

testFunc testFunctions[] = {
	test_aCpuCanBeCreated_withCorrectPCDefaultValue,
	test_aCpuCanBeCreated_withCorrectIDefaultValue,
	test_aCpuCanBeCreated_withCorrectRegisterDefaultValues,
	test_aCpuCanBeCreated_withCorrectStackDefaultValues,
	test_aCpuCanBeCreated_withCorrectSTDefaultValue,
	test_aCpuCanBeCreated_withCorrectDTDefaultValue,
	test_aCpuCanBeCreated_withCorrectSPDefaultValue,
	test_aCpuCanBeCreated_withMemoryAllocated,
	test_aCpuCanBeCreated_withMemoryDefaultValue,
	test_aLoadedProgram_LoadsAtTheCorrectAddressInMemory,
	test_whenDelayTimeIsZero_DecrementingTimersDoesNotAffectDelayTimer,
	test_whenDelayTimeIsNonZero_DecrementingTimersResultsInDelayTimer,
	test_whenOp_1NNN_isInvoked_PCRegisterUpdatesToNNN,
	test_whenOp_BNNN_isInvoked_PCRegisterUpdatesToNNN_Plus_Register_1,
	test_whenOp_2NNN_isInvoked_PCRegisterUpdatesToNNN,
	test_whenOp_2NNN_isInvoked_TheAddressOfTheNextInstructionIsStoredInTheCallStack,
	test_whenOp_2NNN_isInvoked_TheStackPointerIsIncremented,
	test_whenOp_00EE_isInvoked_TheStackPointerIsDecremented,
	test_whenOp_00EE_isInvoked_ThePCRegisterContainsTheAddressOfTheNextInstruction_FromTheStack,
	test_whenOp_3XNN_isInvoked_GivenRegisterX_EqualsNN_ThePCIsIncrementedToSkipTheNextInstruction,
	0
};

int main(int argc, char **argv)
{
	printf("Running tests...\n");
	int i = 0;

	testFunc currentTest = testFunctions[i];
	while(currentTest != NULL)
	{
		currentTest();
		currentTest = testFunctions[++i];
	}
	printf("\nRan %d tests\n", i);

	return 0;
}
