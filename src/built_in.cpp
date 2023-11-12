#include "built_in.h"

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name, str) \
	case BuiltIn::name: return str;

const char* built_in_to_string(BuiltIn type)
{
    switch (type)
    {
#include "built_in.enum"
        default: return "undefined";
    }
}

bool is_token_built_in_type(BuiltIn& type, TokenKind token_kind)
{
    static const std::unordered_map<TokenKind, BuiltIn> built_in_type_map =
    {
        { TokenKind::KW_BOOL, BuiltIn::T_BOOL },
        { TokenKind::KW_CHAR, BuiltIn::T_CHAR },
        { TokenKind::KW_INT8, BuiltIn::T_I8 },
        { TokenKind::KW_INT16, BuiltIn::T_I16 },
        { TokenKind::KW_INT32, BuiltIn::T_I32 },
        /* { TokenKind::KW_INT64, BuiltIn::T_I64 }, */
        { TokenKind::KW_FLOAT32, BuiltIn::T_F32 },
        /* { TokenKind::KW_FLOAT64, BuiltIn::T_FLOAT64 }, */
    };

    auto it = built_in_type_map.find(token_kind);
    if (it != built_in_type_map.end())
    {
        type = it->second;
        return true;
    }
    return false;
}