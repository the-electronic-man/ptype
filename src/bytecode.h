#pragma once

#define ENUM_ITEM(x, n) \
	x,

#define ENUM_LIST \
	ENUM_ITEM(NOP, 0) \
	ENUM_ITEM(HALT, 0) \

enum class Bytecode
{
	ENUM_LIST
};

#undef ENUM_ITEM
#define ENUM_ITEM(x, n) \
	case Bytecode::x: return #x;

static const char* bytecode_to_string(Bytecode code)
{
	switch (code)
	{
		ENUM_LIST
		default: return "undefined";
	}
}

#undef ENUM_ITEM
#define ENUM_ITEM(x, n) \
	case Bytecode::x: return n;

static int bytecode_arg_count(Bytecode code)
{
	switch (code)
	{
		ENUM_LIST
		default: return -1;
	}
}
