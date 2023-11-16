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

bool is_primitive(BuiltIn built_in_type)
{
    return
        is_void(built_in_type) ||
        is_numeric(built_in_type) ||
        is_logic(built_in_type);
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
        built_in_type == BuiltIn::T_I8 ||
        built_in_type == BuiltIn::T_I16 ||
        built_in_type == BuiltIn::T_I32;
}

bool is_decimal(BuiltIn built_in_type)
{
    return
        built_in_type == BuiltIn::T_F32;
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

bool is_reference(BuiltIn built_in_type)
{
    return built_in_type == BuiltIn::T_REF;
}

bool is_array(BuiltIn built_in_type)
{
    return built_in_type == BuiltIn::T_ARR;
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

    // ENUM_ITEM(T_VOID, "void") \
    // ENUM_ITEM(T_BOOL, "bool") \
    // ENUM_ITEM(T_CHAR, "char") \
    // ENUM_ITEM(T_I8, "i8") \
    // ENUM_ITEM(T_I16, "i16") \
    // ENUM_ITEM(T_I32, "i32") \
    // ENUM_ITEM(T_F32, "f32") \
    // ENUM_ITEM(T_ARR, "array") \
    // ENUM_ITEM(T_REF, "ref") \
        
    static const std::vector<std::vector<bool>> implicit_cast_map =
    {
        // src\dst      void    bool    char    i8      i16     i32     f32     arr     ref
        /* void */  {   1,      0,      0,      0,      0,      0,      0,      0,      0   },
        /* bool */  {   0,      1,      0,      0,      0,      0,      0,      0,      0   },
        /* char */  {   0,      0,      1,      1,      1,      1,      1,      0,      0   },
        /* i8   */  {   0,      0,      0,      1,      1,      1,      1,      0,      0   },
        /* i16  */  {   0,      0,      0,      0,      1,      1,      1,      0,      0   },
        /* i32  */  {   0,      0,      0,      0,      0,      1,      1,      0,      0   },
        /* f32  */  {   0,      0,      0,      0,      0,      0,      1,      0,      0   },
        /* arr  */  {   0,      0,      0,      0,      0,      0,      0,      0,      0   },
        /* ref  */  {   0,      0,      0,      0,      0,      0,      0,      0,      0   },
    };

    return implicit_cast_map[(size_t)src_type][(size_t)dst_type];
}