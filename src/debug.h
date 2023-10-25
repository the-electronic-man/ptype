#pragma once
#include <stdio.h>
#include <string.h>
#include <varargs.h>

void _pt_error(const char* format, ...);
void _pt_log(const char* format, ...);

#define pt_log(fmt, ...)	_pt_log(fmt, __VA_ARGS__)
#define pt_error(fmt, ...)	_pt_error(fmt, __VA_ARGS__)

void _pt_error(const char* format, ...)
{
	va_list args;
	printf("[error]: ");
	va_start(args);
	(void)va_arg(args, char);
	vprintf(format, args);
	va_end(args);
	printf("\n");
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
