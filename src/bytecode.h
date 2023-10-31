#pragma once

#define ENUM_ITEM(mnemonic, operand_count, stack_transition) \
	mnemonic,

#define ENUM_LIST \
	ENUM_ITEM(nop, 0, 0) \
	/* stack operations */ \
	ENUM_ITEM(load_imm, 4, 1) \
	ENUM_ITEM(load_const, 1, 1) \
	ENUM_ITEM(load_local, 1, 1) \
	ENUM_ITEM(load_array, 1, 1) \
	ENUM_ITEM(load_field, 1, 1) \
	ENUM_ITEM(store_local, 1, -1) \
	ENUM_ITEM(store_array, 1, -1) \
	ENUM_ITEM(store_field, 1, -1) \
	ENUM_ITEM(pop, 0, -1) \
	ENUM_ITEM(dup, 0, 1) \
	/* int binary arithmetic */ \
	ENUM_ITEM(add_i, 0, -1) \
	ENUM_ITEM(sub_i, 0, -1) \
	ENUM_ITEM(mul_i, 0, -1) \
	ENUM_ITEM(div_i, 0, -1) \
	ENUM_ITEM(mod_i, 0, -1) \
	/* int unary arithmetic */ \
	ENUM_ITEM(neg_i, 0, 0) \
	ENUM_ITEM(inc_i, 0, 0) \
	/* conversion */ \
	ENUM_ITEM(i2f, 0, 0) \
	ENUM_ITEM(f2i, 0, 0) \
	/* int bitwise */ \
	ENUM_ITEM(and_i, 0, -1) \
	ENUM_ITEM(xor_i, 0, -1) \
	ENUM_ITEM(or_i, 0, -1) \
	ENUM_ITEM(shl_i, 0, -1) \
	ENUM_ITEM(shr_i, 0, -1) \
	/* float arithmetic */ \
	ENUM_ITEM(add_f, 0, -1) \
	ENUM_ITEM(sub_f, 0, -1) \
	ENUM_ITEM(mul_f, 0, -1) \
	ENUM_ITEM(div_f, 0, -1) \
	ENUM_ITEM(mod_f, 0, -1) \
	/* float unary arithmetic */ \
	ENUM_ITEM(neg_f, 0, 0) \
	/* comparison with 0 */ \
	ENUM_ITEM(br_eq_z, 4, -1) \
	ENUM_ITEM(br_ne_z, 4, -1) \
	ENUM_ITEM(br_lt_z, 4, -1) \
	ENUM_ITEM(br_le_z, 4, -1) \
	ENUM_ITEM(br_gt_z, 4, -1) \
	ENUM_ITEM(br_ge_z, 4, -1) \
	/* comparison with 2 ints */ \
	ENUM_ITEM(br_eq, 4, -2) \
	ENUM_ITEM(br_ne, 4, -2) \
	ENUM_ITEM(br_lt, 4, -2) \
	ENUM_ITEM(br_le, 4, -2) \
	ENUM_ITEM(br_gt, 4, -2) \
	ENUM_ITEM(br_ge, 4, -2) \
	/* comparison with 0 */ \
	ENUM_ITEM(cmp_eq_z, 4, 0) \
	ENUM_ITEM(cmp_ne_z, 4, 0) \
	ENUM_ITEM(cmp_lt_z, 4, 0) \
	ENUM_ITEM(cmp_le_z, 4, 0) \
	ENUM_ITEM(cmp_gt_z, 4, 0) \
	ENUM_ITEM(cmp_ge_z, 4, 0) \
	/* comparison with 2 ints */ \
	ENUM_ITEM(cmp_eq, 4, -1) \
	ENUM_ITEM(cmp_ne, 4, -1) \
	ENUM_ITEM(cmp_lt, 4, -1) \
	ENUM_ITEM(cmp_le, 4, -1) \
	ENUM_ITEM(cmp_gt, 4, -1) \
	ENUM_ITEM(cmp_ge, 4, -1) \
	/* other */ \
	ENUM_ITEM(halt, 0, 0) \

enum class Bytecode
{
	ENUM_LIST
};

#undef ENUM_ITEM
#define ENUM_ITEM(mnemonic, operand_count, stack_transition) \
	case Bytecode::mnemonic: return #mnemonic;

static const char* bytecode_to_string(Bytecode code)
{
	switch (code)
	{
		ENUM_LIST
		default: return "undefined";
	}
}

#undef ENUM_ITEM
#define ENUM_ITEM(mnemonic, operand_count, stack_transition) \
	case Bytecode::mnemonic: return operand_count;

static int bytecode_operand_count(Bytecode code)
{
	switch (code)
	{
		ENUM_LIST
		default: return -1;
	}
}

#undef ENUM_ITEM
#define ENUM_ITEM(mnemonic, operand_count, stack_transition) \
	case Bytecode::mnemonic: return stack_transition;

static int bytecode_stack_transition(Bytecode code)
{
	switch (code)
	{
		ENUM_LIST
		default: return 255;
	}
}
