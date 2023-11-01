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
	ENUM_ITEM(KW_VAR) \
	ENUM_ITEM(KW_CAST) \
	ENUM_ITEM(KW_AND) \
	ENUM_ITEM(KW_OR) \
	ENUM_ITEM(KW_NOT) \
	ENUM_ITEM(PLUS) \
	ENUM_ITEM(MINUS) \
	ENUM_ITEM(STAR) \
	ENUM_ITEM(SLASH) \
	ENUM_ITEM(PERCENT) \
	ENUM_ITEM(EQUAL) \
	ENUM_ITEM(NOT_EQUAL) \
	ENUM_ITEM(AMPERSAND) \
	ENUM_ITEM(CARET) \
	ENUM_ITEM(VERTICAL_BAR) \
	ENUM_ITEM(HASHTAG) \
	ENUM_ITEM(TILDE) \
	ENUM_ITEM(DOT) \
	ENUM_ITEM(COMMA) \
	ENUM_ITEM(COLON) \
	ENUM_ITEM(SEMICOLON) \
	ENUM_ITEM(LEFT_ANGLE) \
	ENUM_ITEM(LEFT_ANGLE_EQUAL) \
	ENUM_ITEM(LEFT_ANGLE_LEFT_ANGLE) \
	ENUM_ITEM(RIGHT_ANGLE) \
	ENUM_ITEM(RIGHT_ANGLE_EQUAL) \
	ENUM_ITEM(RIGHT_ANGLE_RIGHT_ANGLE) \
	ENUM_ITEM(LEFT_BRAKET) \
	ENUM_ITEM(RIGHT_BRACKET) \
	ENUM_ITEM(LEFT_BRACE) \
	ENUM_ITEM(RIGHT_BRACE) \
	ENUM_ITEM(LEFT_PAREN) \
	ENUM_ITEM(RIGHT_PAREN) \
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
	size_t line = 1, column = 1;
	std::string buffer;

	Token();
	Token(TokenKind kind);
	Token(TokenKind kind, char* buffer, size_t buffer_length);
	Token(TokenKind kind, std::string str);
};
