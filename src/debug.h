#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <cassert>

void pt_error(const char* format, ...);
void pt_log(const char* format, ...);

#define pt_assert(expr)         assert(expr)
#define pt_unreachable()        assert(false)
#define pt_not_implemented()    assert(false)

