#include "built_in_types.h"

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name, str) \
	case PrimitiveType::name: return str;

const char* primitive_type_to_string(PrimitiveType type)
{
    switch (type)
    {
#include "primitive_kind.enum"
        default: return "undefined";
    }
}

bool is_string_a_primitive_type(PrimitiveType& type, std::string str)
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