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
        { TokenKind::KW_I1, BuiltIn::T_I1 },
        { TokenKind::KW_I2, BuiltIn::T_I2 },
        { TokenKind::KW_I4, BuiltIn::T_I4 },
        // { TokenKind::KW_I8, BuiltIn::T_I8 },
        { TokenKind::KW_F4, BuiltIn::T_F4 },
        // { TokenKind::KW_F8, BuiltIn::T_F8 }
    };

    auto it = built_in_type_map.find(token_kind);
    if (it != built_in_type_map.end())
    {
        type = it->second;
        return true;
    }
    return false;
}

bool is_un_op_arith(TokenKind op)
{
    return
        op == TokenKind::PLUS ||
        op == TokenKind::MINUS;
}

bool is_un_op_logic(TokenKind op)
{
    return op == TokenKind::KW_NOT;
}

bool is_un_op_bitwise(TokenKind op)
{
    return op == TokenKind::TILDE;
}

bool is_numeric(BuiltIn built_in_type)
{
    return
        is_integral(built_in_type) ||
        is_decimal(built_in_type);
}

bool is_integral(BuiltIn built_in_type)
{
    return
        is_integer(built_in_type) ||
        is_char(built_in_type);
}

bool is_char(BuiltIn built_in_type)
{
    return
        built_in_type == BuiltIn::T_CHAR;
}

bool is_integer(BuiltIn built_in_type)
{
    return
        built_in_type == BuiltIn::T_I1
        || built_in_type == BuiltIn::T_I2
        || built_in_type == BuiltIn::T_I4
        //|| built_in_type == BuiltIn::T_I8
        ;
}

bool is_decimal(BuiltIn built_in_type)
{
    return
        built_in_type == BuiltIn::T_F4
        //|| built_in_type == BuiltIn::T_F8
        ;
}

bool is_logic(BuiltIn built_in_type)
{
    return
        built_in_type == BuiltIn::T_BOOL;
}

bool is_void(BuiltIn built_in_type)
{
    return
        built_in_type == BuiltIn::T_VOID;
}

BuiltIn get_common_numeric_type(BuiltIn src, BuiltIn dst)
{
    //	float32
    //	int32
    //	int16
    //	int8
    //	char
    return (BuiltIn)std::max((int)src, int(dst));
}

bool is_implicit_cast(BuiltIn src_type, BuiltIn dst_type)
{
    if (!is_numeric(src_type) || !is_numeric(dst_type))
    {
        return false;
    }

    static const std::vector<std::vector<bool>> implicit_cast_map =
    {
        // src\dst      VOID    BOOL    CHAR    I1      I2      I4      F4   
        /* VOID */  {   1,      0,      0,      0,      0,      0,      0,   },
        /* BOOL */  {   0,      1,      0,      0,      0,      0,      0,   },
        /* CHAR */  {   0,      0,      1,      1,      1,      1,      1,   },
        /* I1   */  {   0,      0,      0,      1,      1,      1,      1,   },
        /* I2   */  {   0,      0,      0,      0,      1,      1,      1,   },
        /* I4   */  {   0,      0,      0,      0,      0,      1,      1,   },
        /* F4   */  {   0,      0,      0,      0,      0,      0,      1,   },
    };

    return implicit_cast_map[(size_t)src_type][(size_t)dst_type];
}