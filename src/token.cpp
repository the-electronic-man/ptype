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
	{ "fun", TokenKind::KW_FUN },
	{ "class", TokenKind::KW_CLASS },
	{ "return", TokenKind::KW_RETURN },
	{ "break", TokenKind::KW_BREAK },
	{ "continue", TokenKind::KW_CONTINUE },
	{ "cast", TokenKind::KW_CAST },
	{ "and", TokenKind::KW_AND },
	{ "or", TokenKind::KW_OR },
	{ "not", TokenKind::KW_NOT },
	{ "is", TokenKind::KW_IS },
	{ "null", TokenKind::LIT_NULL },
	{ "true", TokenKind::LIT_TRUE },
	{ "false", TokenKind::LIT_FALSE },
	{ "bool", TokenKind::KW_BOOL },
	{ "char", TokenKind::KW_CHAR },
	{ "i8", TokenKind::KW_I1 },
	{ "i16", TokenKind::KW_I2 },
	{ "i32", TokenKind::KW_I4 },
	//{ "i64", TokenKind::KW_I8 },
	{ "f32", TokenKind::KW_F4 },
	//{ "f64", TokenKind::KW_F8 },
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