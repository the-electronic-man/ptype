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