#pragma once
#include <stdint.h>
#include <fstream>

#include "debug.h"
#include "lexer.h"

void pt_cmd_print_help()
{
	printf("%s",
		R"(----------------------------------------------------------------
Help
----------------------------------------------------------------
The ptype command line tool has the following
commands available:
    --help : display this help message
    <path> : compile & run the specified file
----------------------------------------------------------------)");
}

void pt_read_file(char** buffer, size_t* buffer_length, char* file_name)
{
	int ret = 0;
	std::ifstream file;

	file = std::ifstream(file_name, std::ios::binary | std::ios::ate);

	if (!file.is_open())
	{
		return;
	}

	*buffer_length = file.tellg();

	if (*buffer_length == 0)
	{
		return;
	}

	file.seekg(0, std::ios::beg);
	*buffer = new char[*buffer_length];
	file.read(*buffer, *buffer_length);
	file.close();
}

int pt_cmd_run_file(char* file_name)
{
	char* buffer = nullptr;
	size_t buffer_length = 0;

	pt_read_file(&buffer, &buffer_length, file_name);

	if (!buffer || buffer_length == 0)
	{
		pt_error("file '%s' is invalid", file_name);
	}


	Lexer lexer;
	std::vector<Token> token_list;

	lexer.get_token_list(token_list, buffer, buffer_length);

	for (size_t i = 0; i < token_list.size(); i++)
	{
		Token token = token_list[i];
		pt_log("%s\t%s", token_kind_to_string(token.kind), token.buffer.data());
	}


	return 0;
}