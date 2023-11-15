#include "ast.h"

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name, str) case NodeKind::name: return str;
const char* node_kind_to_string(NodeKind kind)
{
	switch (kind)
	{
#include "ast.enum"
		default: return "undefined";
	}
}