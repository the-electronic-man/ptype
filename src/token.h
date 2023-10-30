#pragma once
#include <string>

#include "debug.h"

#ifdef def_enum_item
#undef def_enum_item
#endif

#define ENUM_ITEM(x) \
	x,

#define ENUM_LIST \
	ENUM_ITEM(NONE) \
	ENUM_ITEM(IDENTIFIER) \
	ENUM_ITEM(LITERAL_NULL) \
	ENUM_ITEM(LITERAL_BOOL) \
	ENUM_ITEM(LITERAL_CHAR) \
	ENUM_ITEM(LITERAL_INT) \
	ENUM_ITEM(LITERAL_FLOAT) \
	ENUM_ITEM(LITERAL_STRING) \
	ENUM_ITEM(PLUS) \
	ENUM_ITEM(MINUS) \
	ENUM_ITEM(STAR) \
	ENUM_ITEM(SLASH) \
	ENUM_ITEM(PERCENT) \
	ENUM_ITEM(LESS_THAN) \
	ENUM_ITEM(LESS_EQUAL) \
	ENUM_ITEM(GREATER_THAN) \
	ENUM_ITEM(GREATER_EQUAL) \
	ENUM_ITEM(EQUALS) \
	ENUM_ITEM(NOT_EQUALS) \
	ENUM_ITEM(KW_VAR) \
	ENUM_ITEM(KW_AS) \
	ENUM_ITEM(KW_AND) \
	ENUM_ITEM(KW_OR) \
	ENUM_ITEM(KW_NOT) \
	ENUM_ITEM(BIT_SHL) \
	ENUM_ITEM(BIT_SHR) \
	ENUM_ITEM(BIT_AND) \
	ENUM_ITEM(BIT_XOR) \
	ENUM_ITEM(BIT_OR) \
	ENUM_ITEM(BIT_COMP) \
	ENUM_ITEM(PUNCT_HASHTAG) \
	ENUM_ITEM(PUNCT_TILDE) \
	ENUM_ITEM(PUNCT_DOT) \
	ENUM_ITEM(PUNCT_COMMA) \
	ENUM_ITEM(PUNCT_COLON) \
	ENUM_ITEM(PUNCT_SEMICOLON) \
	ENUM_ITEM(GROUP_LEFT_BRAKET) \
	ENUM_ITEM(GROUP_RIGHT_BRACKET) \
	ENUM_ITEM(GROUP_LEFT_BRACE) \
	ENUM_ITEM(GROUP_RIGHT_BRACE) \
	ENUM_ITEM(GROUP_LEFT_PAREN) \
	ENUM_ITEM(GROUP_RIGHT_PAREN) \
	ENUM_ITEM(ASSIGN_DIRECT) \
	ENUM_ITEM(END_OF_FILE) \

enum class TokenKind : int32_t
{
	ENUM_LIST
};

#undef ENUM_ITEM
#define ENUM_ITEM(x) \
	case TokenKind::x: return #x;

static const char* token_kind_to_string(TokenKind kind)
{
	switch (kind)
	{
		ENUM_LIST
		default: return "undefined";
	}
}

struct Token
{
	TokenKind kind;

	std::string buffer;

	Token();
	Token(TokenKind kind);
	Token(TokenKind kind, char* buffer, size_t buffer_length);
	Token(TokenKind kind, std::string str);
};
