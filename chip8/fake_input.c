#include "input.h"

static int input_readkey_result = 0;
static int input_keydown_result = -1;

void fake_set_readkey_result(int value)
{
	input_readkey_result = value;
}

void fake_set_keydown_result(int key)
{
	input_keydown_result = key;
}

int input_readkey()
{
	return input_readkey_result;
}

int input_keydown(int key)
{	
	return key == input_keydown_result;
}