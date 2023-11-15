#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <cassert>

void pt_error(const char* format, ...);
void pt_log(const char* format, ...);
void pt_assert(bool expr, const char* file_name, const char* line, const char* msg = "");

#define pt_assert(expr, msg)    pt_assert(expr, __FILE__, __LINE__, msg)
#define pt_unreachable()        assert(false)
#define pt_not_implemented()    assert(false)

