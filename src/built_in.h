#pragma once
#include "token.h"
#include <unordered_map>
#include <vector>

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name, str) \
	name,

enum class BuiltIn
{
#include "built_in.enum"
};

const char* built_in_to_string(BuiltIn type);

bool is_token_built_in_type(BuiltIn& type, TokenKind token_kind);

bool is_un_op_arith(TokenKind op);
bool is_un_op_logic(TokenKind op);
bool is_un_op_bitwise(TokenKind op);
bool is_numeric(BuiltIn built_in_type);
bool is_char(BuiltIn built_in_type);
bool is_integer(BuiltIn built_in_type);
bool is_integral(BuiltIn built_in_type);
bool is_decimal(BuiltIn built_in_type);
bool is_logic(BuiltIn built_in_type);
bool is_void(BuiltIn built_in_type);


bool is_implicit_cast(BuiltIn src_type, BuiltIn dst_type);

BuiltIn get_common_numeric_type(BuiltIn src, BuiltIn dst);