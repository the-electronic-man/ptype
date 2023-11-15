#include "lexer.h"

#define pt_switch_case_letter_lower \
	case 'a': \
	case 'b': \
	case 'c': \
	case 'd': \
	case 'e': \
	case 'f': \
	case 'g': \
	case 'h': \
	case 'i': \
	case 'j': \
	case 'k': \
	case 'l': \
	case 'm': \
	case 'n': \
	case 'o': \
	case 'p': \
	case 'q': \
	case 'r': \
	case 's': \
	case 't': \
	case 'u': \
	case 'v': \
	case 'w': \
	case 'x': \
	case 'y': \
	case 'z' \

#define pt_switch_case_letter_upper \
	case 'A': \
	case 'B': \
	case 'C': \
	case 'D': \
	case 'E': \
	case 'F': \
	case 'G': \
	case 'H': \
	case 'I': \
	case 'J': \
	case 'K': \
	case 'L': \
	case 'M': \
	case 'N': \
	case 'O': \
	case 'P': \
	case 'Q': \
	case 'R': \
	case 'S': \
	case 'T': \
	case 'U': \
	case 'V': \
	case 'W': \
	case 'X': \
	case 'Y': \
	case 'Z' \

#define pt_switch_case_digit \
	case '0': \
	case '1': \
	case '2': \
	case '3': \
	case '4': \
	case '5': \
	case '6': \
	case '7': \
	case '8': \
	case '9' \

#define pt_switch_case_alpha \
	pt_switch_case_letter_lower: \
	pt_switch_case_letter_upper

#define pt_switch_case_alpha_numeric \
	pt_switch_case_alpha: \
	pt_switch_case_digit

#define pt_switch_case_whitespace \
	case '\r': \
	case '\n': \
	case '\t': \
	case '\f': \
	case '\v': \
	case ' ' \

void Lexer::update_cursor_position()
{
	if (ch == '\n')
	{
		column = 1;
		line++;
	}
	else
	{
		column++;
	}
}

void Lexer::advance()
{
	position++;
	if (position < buffer_length)
	{
		ch = buffer[position];
		update_cursor_position();
	}
	else
	{
		ch = '\0';
	}
}

Token Lexer::get_identifier()
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
	auto it = reserved_keywords.find(str);
	if (it != reserved_keywords.end())
	{
		return Token{ it->second };
	}

	return Token{ TokenKind::IDENTIFIER, str };
}

Token Lexer::get_number()
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

Token Lexer::get_token_raw()
{
	while (ch != '\0')
	{
		switch (ch)
		{
		pt_switch_case_whitespace:
			{
				advance();
				break;
			}
		pt_switch_case_alpha:
			{
				return get_identifier();
			}
		pt_switch_case_digit:
			{
				return get_number();
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
				switch (ch)
				{
					case '=':
					{
						advance();
						return Token(TokenKind::EQUAL);
					}
					default:
					{
						return Token(TokenKind::ASSIGN_DIRECT);
					}
				}
			}
			case '<':
			{
				advance();
				switch (ch)
				{
					case '<':
					{
						advance();
						return Token(TokenKind::LEFT_ANGLE_LEFT_ANGLE);
					}
					case '=':
					{
						advance();
						return Token(TokenKind::LEFT_ANGLE_EQUAL);
					}
					default:
					{
						return Token(TokenKind::LEFT_ANGLE);
					}
				}
			}
			case '>':
			{
				advance();
				switch (ch)
				{
					case '>':
					{
						advance();
						return Token(TokenKind::RIGHT_ANGLE_RIGHT_ANGLE);
					}
					case '=':
					{
						advance();
						return Token(TokenKind::RIGHT_ANGLE_EQUAL);
					}
					default:
					{
						return Token(TokenKind::RIGHT_ANGLE);
					}
				}
			}
			case '!':
			{
				advance();
				switch (ch)
				{
					case '=':
					{
						advance();
						return Token(TokenKind::NOT_EQUAL);
					}
					default:
					{
						pt_error("unknown symbol: %c", ch);
						break;
					}
				}
			}
			case '(':
			{
				advance();
				return Token(TokenKind::LEFT_PAREN);
			}
			case ')':
			{
				advance();
				return Token(TokenKind::RIGHT_PAREN);
			}
			case '[':
			{
				advance();
				return Token(TokenKind::LEFT_BRACKET);
			}
			case ']':
			{
				advance();
				return Token(TokenKind::RIGHT_BRACKET);
			}
			case '.':
			{
				advance();
				return Token(TokenKind::DOT);
			}
			case ',':
			{
				advance();
				return Token(TokenKind::COMMA);
			}
			case ':':
			{
				advance();
				return Token(TokenKind::COLON);
			}
			case ';':
			{
				advance();
				return Token(TokenKind::SEMICOLON);
			}
			default:
			{
				pt_error("unknown symbol: %c", ch);
			}
		}
	}
	return Token(TokenKind::END_OF_FILE);
}

Token Lexer::get_token()
{
	Token token = get_token_raw();
	token.line = line;
	token.column = column;
	return token;
}

void Lexer::get_token_list(std::vector<Token>& token_list, char* buffer, size_t buffer_length)
{
	this->buffer = buffer;
	this->buffer_length = buffer_length;
	this->ch = buffer[0];
	this->position = 0;
	update_cursor_position();

	Token token;
	do
	{
		token = get_token();
		token_list.push_back(token);

	} while (token.kind != TokenKind::END_OF_FILE);
}