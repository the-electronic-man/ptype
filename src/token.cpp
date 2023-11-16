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
#include "token.enum"
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
	{ "true", TokenKind::LITERAL_TRUE },
	{ "false", TokenKind::LITERAL_FALSE },
	{ "bool", TokenKind::KW_BOOL },
	{ "char", TokenKind::KW_CHAR },
	{ "i8", TokenKind::KW_INT8 },
	{ "i16", TokenKind::KW_INT16 },
	{ "i32", TokenKind::KW_INT32 },
	/* { "i64", TokenKind::KW_INT64 }, */
	{ "f32", TokenKind::KW_FLOAT32 },
	/* { "f64", TokenKind::KW_FLOAT64 }, */
};

const std::unordered_map<TokenKind, std::string> reserved_operators =
{
	{ TokenKind::PLUS, 						"+" },
	{ TokenKind::MINUS, 					"-" },
	{ TokenKind::STAR, 						"*" },
	{ TokenKind::SLASH, 					"/" },
	{ TokenKind::PERCENT, 					"%" },

	{ TokenKind::LEFT_ANGLE, 				"<" },
	{ TokenKind::LEFT_ANGLE_EQUAL, 			"<=" },
	{ TokenKind::RIGHT_ANGLE, 				">" },
	{ TokenKind::RIGHT_ANGLE_EQUAL,			">=" },
	{ TokenKind::EQUAL, 					"==" },
	{ TokenKind::NOT_EQUAL, 				"!=" },

	{ TokenKind::KW_AND, 					"and" },
	{ TokenKind::KW_OR, 					"or" },
	{ TokenKind::KW_NOT, 					"not" },

	{ TokenKind::AMPERSAND,					"&" },
	{ TokenKind::CARET, 					"^" },
	{ TokenKind::VERTICAL_BAR, 				"|" },
	{ TokenKind::TILDE, 					"~" },
	{ TokenKind::LEFT_ANGLE_LEFT_ANGLE,		"<<" },
	{ TokenKind::RIGHT_ANGLE_RIGHT_ANGLE,	">>" },
};

Token::Token()
{
	this->kind = TokenKind::ERROR;
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