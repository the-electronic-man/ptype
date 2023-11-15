#include "bytecode.h"

#undef ENUM_ITEM
#define ENUM_ITEM(name, str, n, stack) \
	case Bytecode::name: return str;

const char* bytecode_to_string(Bytecode code)
{
	switch (code)
	{
#include "bytecode.def"
		default: return "undefined";
	}
}

#undef ENUM_ITEM
#define ENUM_ITEM(name, str, n, stack) \
	case Bytecode::name: return n;

int bytecode_operand_count(Bytecode code)
{
	switch (code)
	{
#include "bytecode.def"
		default: return -1;
	}
}

#undef ENUM_ITEM
#define ENUM_ITEM(name, str, n, stack) \
	case Bytecode::name: return stack;

int bytecode_stack_transition(Bytecode code)
{
	switch (code)
	{
#include "bytecode.def"
		default: return 255;
	}
}