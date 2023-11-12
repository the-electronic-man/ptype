#pragma once
#include <string>

#include "debug.h"

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name) name,

enum class TokenKind : int32_t
{
#include "token_kind.enum"
};

const char* token_kind_to_string(TokenKind kind);

struct Token
{
	TokenKind kind;
	size_t line = 1, column = 1;
	std::string buffer;

	Token();
	Token(TokenKind kind);
	Token(TokenKind kind, char* buffer, size_t buffer_length);
	Token(TokenKind kind, std::string str);
};

typedef size_t token_index_t;
