#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "debug.h"

struct Object;

enum class ObjectKind
{
    BOOL,
    I1,
    I2,
    I4,
    F4,
    REF
};

typedef size_t** MethodTable;

struct Reference
{
    MethodTable method_table;
};

struct Object
{
    ObjectKind kind;
    union
    {
        bool b;
        int8_t i1;
        int16_t i2;
        int32_t i4;
        float f4;
        void* ref;
    };
};

#define IS_BOOL(obj)      ((obj).kind == ValueKind::BOOL)
#define IS_I1(obj)        ((obj).kind == ValueKind::I1)
#define IS_I2(obj)        ((obj).kind == ValueKind::I2)
#define IS_I4(obj)        ((obj).kind == ValueKind::I4)
#define IS_F4(obj)        ((obj).kind == ValueKind::F4)
#define IS_OBJ(obj)       ((obj).kind == ValueKind::REF)

#define AS_BOOL(obj)      ((obj).b)
#define AS_I1(obj)        ((obj).i1)
#define AS_I2(obj)        ((obj).i2)
#define AS_I4(obj)        ((obj).i4)
#define AS_F4(obj)        ((obj).f4)
#define AS_REF(obj)       ((obj).ref)