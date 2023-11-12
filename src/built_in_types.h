#pragma once
#include <unordered_map>

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name, str) \
	name,

enum class PrimitiveType
{
#include "primitive_kind.enum"
};

const char* primitive_type_to_string(PrimitiveType type);

bool is_string_a_primitive_type(PrimitiveType& type, std::string str);

