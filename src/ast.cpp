#include "ast.h"

#define ENUM_ITEM(name, str) \
	case NodeKind::name: return str;

const char* node_kind_to_string(NodeKind kind)
{
	switch (kind)
	{
#include "ast_kind.def"
		default: return "undefined";
	}
}

