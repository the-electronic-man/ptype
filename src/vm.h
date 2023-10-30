#pragma once
#include <stdint.h>

#define MAX_STACK	1024

typedef uint64_t value_t;

struct VirtualMachine
{
	value_t stack[MAX_STACK];
	
};