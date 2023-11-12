#pragma once
#include "token.h"
#include <unordered_map>

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

