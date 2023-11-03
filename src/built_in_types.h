#pragma once
#include <unordered_map>

#undef ENUM_ITEM
#define ENUM_ITEM(x) \
	x,

#undef ENUM_LIST
#define ENUM_LIST \
	ENUM_ITEM(T_VOID) \
	ENUM_ITEM(T_BOOL) \
	ENUM_ITEM(T_CHAR) \
	ENUM_ITEM(T_INT) \
	ENUM_ITEM(T_FLOAT) \
	ENUM_ITEM(T_ARRAY) \
	ENUM_ITEM(T_REF) \


enum class PrimitiveType
{
	ENUM_LIST	
};

#undef ENUM_ITEM
#define ENUM_ITEM(x) \
	case PrimitiveType::x: return #x;

static const char* primitive_type_to_string(PrimitiveType type)
{
	switch (type)
	{
		ENUM_LIST
		default: return "undefined";
	}
}

static bool is_string_a_primitive_type(PrimitiveType& type, std::string str)
{
	static const std::unordered_map<std::string, PrimitiveType> primitive_type_map =
	{
		{ "void", PrimitiveType::T_VOID },
		{ "bool", PrimitiveType::T_BOOL },
		{ "char", PrimitiveType::T_CHAR },
		{ "int", PrimitiveType::T_INT },
		{ "float", PrimitiveType::T_FLOAT },
	};

	auto it = primitive_type_map.find(str);
	if (it != primitive_type_map.end())
	{
		type = it->second;
		return true;
	}
	return false;
}

