#include "token.h"

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif

#define ENUM_ITEM(name) \
	case TokenKind::name: return #name;

const char* token_kind_to_string(TokenKind kind)
{
	switch (kind)
	{
#include "token_kind.enum"
		default: return "undefined";
	}
}

const std::unordered_map<std::string, TokenKind> reserved_keywords =
{
	{ "var", TokenKind::KW_VAR },
	{ "cast", TokenKind::KW_CAST },
	{ "and", TokenKind::KW_AND },
	{ "or", TokenKind::KW_OR },
	{ "not", TokenKind::KW_NOT },
	{ "is", TokenKind::KW_IS },
	{ "null", TokenKind::LITERAL_NULL },
	{ "bool", TokenKind::KW_BOOL },
	{ "char", TokenKind::KW_CHAR },
	{ "i8", TokenKind::KW_INT8 },
	{ "i16", TokenKind::KW_INT16 },
	{ "i32", TokenKind::KW_INT32 },
	/* { "i64", TokenKind::KW_INT64 }, */
	{ "f32", TokenKind::KW_FLOAT32 },
	/* { "f64", TokenKind::KW_FLOAT64 }, */
};

Token::Token()
{
	this->kind = TokenKind::NONE;
	this->buffer = "";
}

Token::Token(TokenKind kind)
{
	this->kind = kind;
	this->buffer = "";
}

Token::Token(TokenKind kind, char* buffer, size_t buffer_length)
{
	this->kind = kind;
	this->buffer = std::string(buffer, buffer_length);
}

Token::Token(TokenKind kind, std::string str)
{
	this->kind = kind;
	this->buffer = str;
}