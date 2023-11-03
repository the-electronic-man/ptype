#include "debug.h"

void pt_error(const char* format, ...)
{
	va_list args;
	printf("[error]: ");
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
	exit(1);
}

void pt_log(const char* format, ...)
{
	va_list args;
	printf("[log]: ");
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\n");
}
