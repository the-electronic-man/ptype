#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>

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