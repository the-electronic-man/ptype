#pragma once
#include <ctype.h>
#include <vector>

#include "token.h"


struct Lexer
{
	char* buffer;
	size_t buffer_length;
	char ch;
	size_t position;

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
		else if (str == "as") { kind = TokenKind::KW_AS; }
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
				case '+':
				{
					advance();
					return Token(TokenKind::PLUS);
				}
				case '-':
				{
					advance();
					return Token(TokenKind::MINUS);
				}
				case '*':
				{
					advance();
					return Token(TokenKind::STAR);
				}
				case '/':
				{
					advance();
					return Token(TokenKind::SLASH);
				}
				case '%':
				{
					advance();
					return Token(TokenKind::PERCENT);
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

	void get_token_list(std::vector<Token>& token_list, char* buffer, size_t buffer_length)
	{
		this->buffer = buffer;
		this->buffer_length = buffer_length;
		this->ch = buffer[0];
		this->position = 0;

		Token token;
		do
		{
			token = get_token();
			token_list.push_back(token);

		} while (token.kind != TokenKind::END_OF_FILE);
	}
};
