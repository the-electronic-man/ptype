#pragma once
#include <string>

#include "debug.h"

#ifdef def_enum_item
#undef def_enum_item
#endif

#define def_enum_item(x) \
	x,

#define def_enum_list \
	def_enum_item(NONE) \
	def_enum_item(IDENTIFIER) \
	def_enum_item(LITERAL_NULL) \
	def_enum_item(LITERAL_BOOL) \
	def_enum_item(LITERAL_CHAR) \
	def_enum_item(LITERAL_INT) \
	def_enum_item(LITERAL_FLOAT) \
	def_enum_item(LITERAL_STRING) \
	def_enum_item(PLUS) \
	def_enum_item(MINUS) \
	def_enum_item(STAR) \
	def_enum_item(SLASH) \
	def_enum_item(PERCENT) \
	def_enum_item(LESS_THAN) \
	def_enum_item(LESS_EQUAL) \
	def_enum_item(GREATER_THAN) \
	def_enum_item(GREATER_EQUAL) \
	def_enum_item(EQUALS) \
	def_enum_item(NOT_EQUALS) \
	def_enum_item(KW_VAR) \
	def_enum_item(KW_AS) \
	def_enum_item(KW_AND) \
	def_enum_item(KW_OR) \
	def_enum_item(KW_NOT) \
	def_enum_item(BIT_AND) \
	def_enum_item(BIT_XOR) \
	def_enum_item(BIT_OR) \
	def_enum_item(BIT_COMP) \
	def_enum_item(PUNCT_TILDE) \
	def_enum_item(PUNCT_DOT) \
	def_enum_item(PUNCT_COMMA) \
	def_enum_item(PUNCT_COLON) \
	def_enum_item(PUNCT_SEMICOLON) \
	def_enum_item(GROUP_LEFT_BRAKET) \
	def_enum_item(GROUP_RIGHT_BRACKET) \
	def_enum_item(GROUP_LEFT_BRACE) \
	def_enum_item(GROUP_RIGHT_BRACE) \
	def_enum_item(GROUP_LEFT_PAREN) \
	def_enum_item(GROUP_RIGHT_PAREN) \
	def_enum_item(ASSIGN_DIRECT) \
	def_enum_item(END_OF_FILE) \

enum class TokenKind : int32_t
{
	def_enum_list
};

#undef def_enum_item
#define def_enum_item(x) \
	case TokenKind::x: return #x;

static const char* token_kind_to_string(TokenKind kind)
{
	switch (kind)
	{
		def_enum_list
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
