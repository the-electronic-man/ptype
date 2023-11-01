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
	size_t line = 1, column = 1;


	void advance();
	Token get_identifier();
	Token get_number();
	Token get_token_raw();
	Token get_token();
	void update_cursor_position();
	void get_token_list(std::vector<Token>& token_list, char* buffer, size_t buffer_length);
};
