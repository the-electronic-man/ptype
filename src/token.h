#pragma once
#include <string>

#include "debug.h"

#define token_kind(x) \
	x,

#define token_kind_list \
	token_kind(NONE) \
	token_kind(IDENTIFIER) \
	token_kind(LITERAL_NULL) \
	token_kind(LITERAL_BOOL) \
	token_kind(LITERAL_CHAR) \
	token_kind(LITERAL_INT) \
	token_kind(LITERAL_FLOAT) \
	token_kind(LITERAL_STRING) \
	token_kind(PLUS) \
	token_kind(MINUS) \
	token_kind(STAR) \
	token_kind(SLASH) \
	token_kind(PERCENT) \
	token_kind(LESS_THAN) \
	token_kind(LESS_EQUAL) \
	token_kind(GREATER_THAN) \
	token_kind(GREATER_EQUAL) \
	token_kind(EQUALS) \
	token_kind(NOT_EQUALS) \
	token_kind(KW_VAR) \
	token_kind(KW_AND) \
	token_kind(KW_OR) \
	token_kind(KW_NOT) \
	token_kind(BIT_AND) \
	token_kind(BIT_XOR) \
	token_kind(BIT_OR) \
	token_kind(BIT_COMP) \
	token_kind(PUNCT_TILDE) \
	token_kind(PUNCT_DOT) \
	token_kind(PUNCT_COMMA) \
	token_kind(PUNCT_COLON) \
	token_kind(PUNCT_SEMICOLON) \
	token_kind(GROUP_LEFT_BRAKET) \
	token_kind(GROUP_RIGHT_BRACKET) \
	token_kind(GROUP_LEFT_BRACE) \
	token_kind(GROUP_RIGHT_BRACE) \
	token_kind(GROUP_LEFT_PAREN) \
	token_kind(GROUP_RIGHT_PAREN) \
	token_kind(ASSIGN_DIRECT) \
	token_kind(END_OF_FILE) \

enum class TokenKind : int32_t
{
	token_kind_list
};

#undef token_kind
#define token_kind(x) \
	case TokenKind::x: return #x;

static const char* token_kind_to_string(TokenKind kind)
{
	switch (kind)
	{
		token_kind_list
		default: return "undefined";
	}
}

struct Token
{
	TokenKind kind;

	std::string buffer;

	Token()
	{
		this->kind = TokenKind::NONE;
		this->buffer = "";
	}

	Token(TokenKind kind)
	{
		this->kind = kind;
		this->buffer = "";
	}

	Token(TokenKind kind, char* buffer, size_t buffer_length)
	{
		this->kind = kind;
		this->buffer = std::string(buffer, buffer_length);
	}

	Token(TokenKind kind, std::string str)
	{
		this->kind = kind;
		this->buffer = str;
	}
};
