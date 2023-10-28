#include "token.h"


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