#pragma once

#undef ENUM_ITEM
#define ENUM_ITEM(name, str, n, stack) \
	name,

enum class Bytecode
{
#include "bytecode_kind.def"
};


const char* bytecode_to_string(Bytecode code);
int bytecode_operand_count(Bytecode code);
int bytecode_stack_transition(Bytecode code);
