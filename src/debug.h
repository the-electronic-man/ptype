#pragma once
#include <stdio.h>
#include <varargs.h>
#include <string.h>
#include <stdlib.h>

void _pt_error(const char* format, ...);
void _pt_log(const char* format, ...);

#define pt_log(fmt, ...)	_pt_log(fmt, __VA_ARGS__)
#define pt_error(fmt, ...)	_pt_error(fmt, __VA_ARGS__)
