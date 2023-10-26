#pragma once
#include <ctype.h>
#include <vector>
#include "debug.h"

enum class TokenKind
{
	NONE,

	IDENTIFIER,

	LITERAL_NULL,
	LITERAL_BOOL,
	LITERAL_CHAR,
	LITERAL_INT,
	LITERAL_FLOAT,
	LITERAL_STRING,

	PLUS,
	MINUS,
	STAR,
	SLASH,
	PERCENT,

	LESS_THAN,
	LESS_EQUAL,
	GREATER_THAN,
	GREATER_EQUAL,
	EQUALS,
	NOT_EQUALS,

	KW_VAR,

	KW_AND,
	KW_OR,
	KW_NOT,

	BIT_AND,
	BIT_XOR,
	BIT_OR,
	BIT_COMP,

	PUNCT_DOT,
	PUNCT_COMMA,
	PUNCT_COLON,
	PUNCT_SEMICOLON,

	GROUP_LEFT_BRAKET,
	GROUP_RIGHT_BRACKET,
	GROUP_LEFT_BRACE,
	GROUP_RIGHT_BRACE,
	GROUP_LEFT_PAREN,
	GROUP_RIGHT_PAREN,

	ASSIGN_DIRECT,

	END_OF_FILE,
};

char* token_kind_to_string(TokenKind kind)
{
	static const char* token_kind_names[] =
	{
		"NONE",

		"IDENTIFIER",

		"LITERAL_NULL",
		"LITERAL_BOOL",
		"LITERAL_CHAR",
		"LITERAL_INT",
		"LITERAL_FLOAT",
		"LITERAL_STRING",

		"PLUS",
		"MINUS",
		"STAR",
		"SLASH",
		"PERCENT",

		"LESS_THAN",
		"LESS_EQUAL",
		"GREATER_THAN",
		"GREATER_EQUAL",
		"EQUALS",
		"NOT_EQUALS",

		"KW_VAR",

		"KW_AND",
		"KW_OR",
		"KW_NOT",

		"BIT_AND",
		"BIT_XOR",
		"BIT_OR",
		"BIT_COMP",

		"PUNCT_DOT",
		"PUNCT_COMMA",
		"PUNCT_COLON",
		"PUNCT_SEMICOLON",

		"GROUP_LEFT_BRAKET",
		"GROUP_RIGHT_BRACKET",
		"GROUP_LEFT_BRACE",
		"GROUP_RIGHT_BRACE",
		"GROUP_LEFT_PAREN",
		"GROUP_RIGHT_PAREN",

		"ASSIGN_DIRECT",

		"END_OF_FILE",
	};

	return (char*)token_kind_names[(size_t)kind];
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

struct Lexer
{
	char* buffer;
	size_t buffer_length;
	char ch;
	size_t position;

	Lexer(char* buffer, size_t buffer_length)
	{
		this->buffer = buffer;
		this->buffer_length = buffer_length;
		this->ch = buffer[0];
		this->position = 0;
	}

	void advance()
	{
		position++;
		if (position < buffer_length)
		{
			ch = buffer[position];
		}
		else
		{
			ch = '\0';
		}
	}

	Token get_identifier()
	{
		size_t it_begin = position;

		while (isalnum(ch) ||
			isdigit(ch) ||
			ch == '_')
		{
			advance();
		}

		size_t it_end = position;

		std::string str = std::string(buffer + it_begin, it_end - it_begin);

		TokenKind kind;

		if (str == "var") { kind = TokenKind::KW_VAR; }
		else if (str == "and") { kind = TokenKind::KW_AND; }
		else if (str == "or") { kind = TokenKind::KW_OR; }
		else if (str == "not") { kind = TokenKind::KW_NOT; }
		else { kind = TokenKind::IDENTIFIER; }

		return Token(kind, str);
	}

	Token get_number()
	{
		TokenKind kind = TokenKind::LITERAL_INT;
		size_t it_begin = position;

		while (isdigit(ch))
		{
			advance();
		}

		if (ch == '.')
		{
			advance();
			while (isdigit(ch))
			{
				advance();
			}

			kind = TokenKind::LITERAL_FLOAT;
		}

		size_t it_end = position;

		return Token(kind, buffer + it_begin, it_end - it_begin);
	}

	Token get_token()
	{
		while (ch != '\0')
		{
			if (isalpha(ch) || ch == '_')
			{
				return get_identifier();
			}
			else if (isdigit(ch))
			{
				return get_number();
			}

			switch (ch)
			{
				case '\n':
				case '\t':
				case '\r':
				case '\f':
				case '\v':
				case ' ':
				{
					advance();
					break;
				}
				case '=':
				{
					advance();
					return Token(TokenKind::ASSIGN_DIRECT);
				}
				case ';':
				{
					advance();
					return Token(TokenKind::PUNCT_SEMICOLON);
				}
				default:
				{
					pt_error("unknown symbol: %c", ch);
				}
			}
		}

		return Token(TokenKind::END_OF_FILE);
	}

};

void pt_generate_tokens(char* buffer, size_t buffer_length, std::vector<Token>* token_list)
{
	Lexer lexer = Lexer(buffer, buffer_length);
	Token token;
	do
	{
		token = lexer.get_token();
		token_list->push_back(token);
		
	} while (token.kind != TokenKind::END_OF_FILE);


	for (size_t i = 0; i < token_list->size(); i++)
	{
		Token tok = token_list->at(i);
		pt_log("%s\t%s", token_kind_to_string(tok.kind), tok.buffer.data());
	}
}