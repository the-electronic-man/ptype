#pragma once
#include <stdint.h>
#include <fstream>

#include "debug.h"

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

int pt_cmd_run_file(char* file_name)
{
	std::ifstream file = 
		std::ifstream(file_name, std::ios::binary | std::ios::ate);

	pt_log("open file: %s", file_name);

	if (!file.is_open())
	{
		pt_error("could not open file %s", file_name);
	}

	size_t file_length = file.tellg();

	file.seekg(0, std::ios::beg);

	pt_log("file length: %llu", file_length);

	file.close();

	return 0;
}