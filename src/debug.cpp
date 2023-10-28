#pragma once
#include "debug.h"

void _pt_error(const char* format, ...)
{
	va_list args;
	printf("[error]: ");
	va_start(args);
	(void)va_arg(args, char);
	vprintf(format, args);
	va_end(args);
	printf("\n");
	exit(1);
}

void _pt_log(const char* format, ...)
{
	va_list args;
	printf("[log]: ");
	va_start(args);
	(void)va_arg(args, char);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}
