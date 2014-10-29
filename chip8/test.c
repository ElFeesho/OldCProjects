#include "ops.h"
#include "input.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>

static const int TEST_KEY_2 = 2;
static const int TEST_KEY_4 = 4;

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

/*
	Functions for faking input 
*/
void fake_set_readkey_result(int value);
void fake_set_keydown_result(int key);

int fake_gfx_cls_called();
int fake_gfx_draw_last_x();
int fake_gfx_draw_last_y();
int fake_gfx_draw_last_count();
void fake_gfx_cls_reset();
void fake_gfx_draw_reset();

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

void test_1NNN_isInvoked_PCRegisterUpdatesToNNN()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x1123);
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	assertEquals(0x123, cpu->PC, "PC should be 0x123", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_BNNN_isInvoked_PCRegisterUpdatesToNNN_Plus_Register_1()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xB002);
	cpu->regs[0] = 0x2;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x4, cpu->PC, "PC should be 0x4", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_2NNN_isInvoked_PCRegisterUpdatesToNNN()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x2123);

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x123, cpu->PC, "PC should be 0x4", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_2NNN_isInvoked_TheAddressOfTheNextInstructionIsStoredInTheCallStack()
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

void test_2NNN_isInvoked_TheStackPointerIsIncremented()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x2123);

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(1, cpu->SP, "Stack pointer does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_00EE_isInvoked_TheStackPointerIsDecremented()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x00EE);
	cpu->SP = 1;


	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0, cpu->SP, "Stack pointer does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_00EE_isInvoked_ThePCRegisterContainsTheAddressOfTheNextInstruction_FromTheStack()
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

void test_3XNN_isInvoked_GivenRegisterX_EqualsNN_ThePCIsIncrementedToSkipTheNextInstruction()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x30ff);
	cpu->regs[0] = 0xff;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_3XNN_isInvoked_GivenRegisterX_DoesNotEqual_NN_TheNextInstructionIsNotSkipped()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x30ff);
	cpu->regs[0] = 0x88;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x202, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}


void test_5XY0_isInvoked_GivenRegisterX_Equals_RegisterY_ThePCIsIncrementedToSkipTheNextInstruction()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x5120);
	cpu->regs[1] = 0x88;
	cpu->regs[2] = 0x88;
	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_5XY0_isInvoked_GivenRegisterX_DoesNotEqual_RegisterY_TheNextInstructionIsNotSkipped()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x5120);
	cpu->regs[1] = 0x88;
	cpu->regs[2] = 0xff;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x202, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_4XNN_isInvoked_GivenRegisterX_DoesNotEqual_NN_TheNextInstructionIsSkipped()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x4123);
	cpu->regs[1] = 0xff;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_4XNN_isInvoked_GivenRegisterX_isEqualTo_NN_TheNextInstructionIsNotSkipped()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x4123);
	cpu->regs[1] = 0x23;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x202, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_9XY0_isInvoked_GivenRegisterX_DoesNotEqual_RegisterY_TheNextInstructionIsSkipped()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x9120);
	cpu->regs[1] = 0x88;
	cpu->regs[2] = 0xff;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_9XY0_isInvoked_GivenRegisterX_isEqualTo_RegisterY_TheNextInstructionIsNotSkipped()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x9120);
	cpu->regs[1] = 0x88;
	cpu->regs[2] = 0x88;

	load_game(cpu, (void*)&op, 2);

	parse_op(cpu);

	assertEquals(0x202, cpu->PC, "PC does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);	
}

void test_FX15_isInvoked_DelayTimerIsSetTo_TheValueOfRegisterX()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF215);
	cpu->regs[2] = 0x88;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x88, cpu->DT, "Delay Timer does not match expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX07_isInvoked_RegisterX_IsSetToTheValueOf_TheDelayTimer()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF207);
	cpu->DT = 0x88;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x88, cpu->regs[2], "Register does not contain the correct value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX18_isInvoked_SoundTimer_IsSetToTheValueOf_RegisterX()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF218);
	cpu->regs[2] = 0x88;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x88, cpu->ST, "Sound Timer does not contain the correct value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX0A_isInvoked_TheSystemReadsAKey_ThePressedKeyIsStoredIn_RegisterX()
{
	fake_set_readkey_result(TEST_KEY_4);
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF30A);
	
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(TEST_KEY_4, cpu->regs[3], "Key press value was not stored", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_EX9E_isInvoked_IfKeyValueOfRegisterX_isDown_theNextInstructionIsSkipped()
{
	fake_set_keydown_result(TEST_KEY_4);
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xE39E);
	cpu->regs[3] = TEST_KEY_4;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_EX9E_isInvoked_IfKeyValueOfRegisterX_isNotDown_noInstructionsAreSkipped()
{
	fake_set_keydown_result(TEST_KEY_4);
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xE39E);
	cpu->regs[3] = TEST_KEY_2;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x202, cpu->PC, "PC is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_EXA1_isInvoked_IfKeyValueOfRegisterX_isNotDown_theNextInstructionIsSkipped()
{
	fake_set_keydown_result(TEST_KEY_2);
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xE3A1);
	cpu->regs[3] = TEST_KEY_4;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x204, cpu->PC, "PC is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_EXA1_isInvoked_IfKeyValueOfRegisterX_isDown_noInstructionsAreSkipped()
{
	fake_set_keydown_result(TEST_KEY_2);
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xE3A1);
	cpu->regs[3] = TEST_KEY_2;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x202, cpu->PC, "PC is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_ANNN_isInvoked_I_RegisterIsSetTo_NNN()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xA123);

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x123, cpu->I, "I is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX1E_isInvoked_TheValueOfRegisterX_IsAddedTo_I()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF11E);
	cpu->regs[1] = 0x10;
	cpu->I = 0x10;
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(0x20, cpu->I, "I is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_DXYN_isInvoked_ASpriteIsRendered_atXCoord_RegisterX_atYCoord_RegisterY_OfHeight_N()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xD123);
	cpu->regs[1] = 5;
	cpu->regs[2] = 15;
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertEquals(5, fake_gfx_draw_last_x(), "X position is not expected value", __FUNCTION__);
	assertEquals(15, fake_gfx_draw_last_y(), "Y position is not expected value", __FUNCTION__);
	assertEquals(3, fake_gfx_draw_last_count(), "Count is not expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_00E0_isInvoked_TheScreenIsCleared()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0x00E0);
	
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);

	assertTrue(fake_gfx_cls_called(), "X position is not expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX29_isInvoked_I_IsSetToTheAddressOfTheFontCharacter_WithTheValueOf_RegisterX()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF529);
	cpu->regs[5] = 0x02;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x02*5, cpu->I, "I is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX33_isInvoked_TheValueAt_RegisterX_IsStoredAsBinaryCodedDecimal_AtTheMemoryAddressOfI()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF033);
	cpu->regs[0] = 0x80;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(1, cpu->memory[cpu->I], "Memory at address I is not the expected value", __FUNCTION__);
	assertEquals(2, cpu->memory[cpu->I+1], "Memory at address I + 1 is not the expected value", __FUNCTION__);
	assertEquals(8, cpu->memory[cpu->I+2], "Memory at address I + 2 is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX55_isInvoked_Register0_throughTo_RegisterX_isStoredAtMemoryAddress_PointedAtBy_I()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF655);
	cpu->regs[0] = 0x00;
	cpu->regs[1] = 0x01;
	cpu->regs[2] = 0x02;
	cpu->regs[3] = 0x03;
	cpu->regs[4] = 0x04;
	cpu->regs[5] = 0x05;
	cpu->regs[6] = 0x06;


	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x00, cpu->memory[0], "Memory at address 0 is not the expected value", __FUNCTION__);
	assertEquals(0x01, cpu->memory[1], "Memory at address 1 is not the expected value", __FUNCTION__);
	assertEquals(0x02, cpu->memory[2], "Memory at address 2 is not the expected value", __FUNCTION__);
	assertEquals(0x03, cpu->memory[3], "Memory at address 3 is not the expected value", __FUNCTION__);
	assertEquals(0x04, cpu->memory[4], "Memory at address 4 is not the expected value", __FUNCTION__);
	assertEquals(0x05, cpu->memory[5], "Memory at address 5 is not the expected value", __FUNCTION__);
	assertEquals(0x06, cpu->memory[6], "Memory at address 6 is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX55_isInvoked_I_isIncrementedByHowManyRegistersHaveBeenStored_plusOne()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF655);
	
	cpu->I = 0x20;

	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x27, cpu->I, "I is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX65_isInvoked_RegisterValues_0_throughTo_RegisterX_AreSetToValuesFromMemory_PointedAtByI()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF665);
	cpu->memory[10] = 0x00;
	cpu->memory[11] = 0x01;
	cpu->memory[12] = 0x02;
	cpu->memory[13] = 0x03;
	cpu->memory[14] = 0x04;
	cpu->memory[15] = 0x05;
	cpu->memory[16] = 0x06;

	cpu->I = 10;
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(0x00, cpu->regs[0], "Register 0 is not the expected value", __FUNCTION__);
	assertEquals(0x01, cpu->regs[1], "Register 1 is not the expected value", __FUNCTION__);
	assertEquals(0x02, cpu->regs[2], "Register 2 is not the expected value", __FUNCTION__);
	assertEquals(0x03, cpu->regs[3], "Register 3 is not the expected value", __FUNCTION__);
	assertEquals(0x04, cpu->regs[4], "Register 4 is not the expected value", __FUNCTION__);
	assertEquals(0x05, cpu->regs[5], "Register 5 is not the expected value", __FUNCTION__);
	assertEquals(0x06, cpu->regs[6], "Register 6 is not the expected value", __FUNCTION__);
	destroy_cpu(cpu);
}

void test_FX65_isInvoked_I_isIncrementedByHowManyRegisterValuesHaveBeenLoaded_PlusOne()
{
	chip8_cpu_t *cpu = create_cpu();
	short op = TO_OP(0xF665);
	
	cpu->I = 10;
	load_game(cpu, (void*)&op, 2);
	parse_op(cpu);
	
	assertEquals(17, cpu->I, "I is not the expected value", __FUNCTION__);
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
	test_1NNN_isInvoked_PCRegisterUpdatesToNNN,
	test_BNNN_isInvoked_PCRegisterUpdatesToNNN_Plus_Register_1,
	test_2NNN_isInvoked_PCRegisterUpdatesToNNN,
	test_2NNN_isInvoked_TheAddressOfTheNextInstructionIsStoredInTheCallStack,
	test_2NNN_isInvoked_TheStackPointerIsIncremented,
	test_00EE_isInvoked_TheStackPointerIsDecremented,
	test_00EE_isInvoked_ThePCRegisterContainsTheAddressOfTheNextInstruction_FromTheStack,
	test_3XNN_isInvoked_GivenRegisterX_EqualsNN_ThePCIsIncrementedToSkipTheNextInstruction,
	test_3XNN_isInvoked_GivenRegisterX_DoesNotEqual_NN_TheNextInstructionIsNotSkipped,
	test_5XY0_isInvoked_GivenRegisterX_Equals_RegisterY_ThePCIsIncrementedToSkipTheNextInstruction,
	test_5XY0_isInvoked_GivenRegisterX_DoesNotEqual_RegisterY_TheNextInstructionIsNotSkipped,
	test_4XNN_isInvoked_GivenRegisterX_DoesNotEqual_NN_TheNextInstructionIsSkipped,
	test_4XNN_isInvoked_GivenRegisterX_isEqualTo_NN_TheNextInstructionIsNotSkipped,
	test_9XY0_isInvoked_GivenRegisterX_DoesNotEqual_RegisterY_TheNextInstructionIsSkipped,
	test_9XY0_isInvoked_GivenRegisterX_isEqualTo_RegisterY_TheNextInstructionIsNotSkipped,
	test_FX15_isInvoked_DelayTimerIsSetTo_TheValueOfRegisterX,
	test_FX07_isInvoked_RegisterX_IsSetToTheValueOf_TheDelayTimer,
	test_FX18_isInvoked_SoundTimer_IsSetToTheValueOf_RegisterX,
	test_FX0A_isInvoked_TheSystemReadsAKey_ThePressedKeyIsStoredIn_RegisterX,
	test_EX9E_isInvoked_IfKeyValueOfRegisterX_isDown_theNextInstructionIsSkipped,
	test_EX9E_isInvoked_IfKeyValueOfRegisterX_isNotDown_noInstructionsAreSkipped,
	test_EXA1_isInvoked_IfKeyValueOfRegisterX_isNotDown_theNextInstructionIsSkipped,
	test_EXA1_isInvoked_IfKeyValueOfRegisterX_isDown_noInstructionsAreSkipped,
	test_ANNN_isInvoked_I_RegisterIsSetTo_NNN,
	test_FX1E_isInvoked_TheValueOfRegisterX_IsAddedTo_I,
	test_DXYN_isInvoked_ASpriteIsRendered_atXCoord_RegisterX_atYCoord_RegisterY_OfHeight_N,
	test_00E0_isInvoked_TheScreenIsCleared,
	test_FX29_isInvoked_I_IsSetToTheAddressOfTheFontCharacter_WithTheValueOf_RegisterX,
	test_FX33_isInvoked_TheValueAt_RegisterX_IsStoredAsBinaryCodedDecimal_AtTheMemoryAddressOfI,
	test_FX55_isInvoked_Register0_throughTo_RegisterX_isStoredAtMemoryAddress_PointedAtBy_I,
	test_FX55_isInvoked_I_isIncrementedByHowManyRegistersHaveBeenStored_plusOne,
	test_FX65_isInvoked_RegisterValues_0_throughTo_RegisterX_AreSetToValuesFromMemory_PointedAtByI,
	test_FX65_isInvoked_I_isIncrementedByHowManyRegisterValuesHaveBeenLoaded_PlusOne,
	0
};

int main(int argc, char **argv)
{
	printf("Running tests...\n");
	int i = 0;

	testFunc currentTest = testFunctions[i];
	while(currentTest != NULL)
	{
		fake_gfx_cls_reset();
		fake_gfx_draw_reset();
		currentTest();
		currentTest = testFunctions[++i];
	}
	printf("\nRan %d tests\n", i);

	return 0;
}
