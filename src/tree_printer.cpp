#include "tree_printer.h"


#define VISIT_CHILD(CHILD) node->CHILD->accept(this)
#define XML_ATTR(NAME, VALUE) { NAME, VALUE }
#define XML_ATTR_LIST(...) { __VA_ARGS__ }
#define XML_PRINT_ELEMENT_ATTR(NAME, ATTRIBUTES, ...) \
	output_file << ident_str << "<" << (NAME); \
	std::vector<std::pair<std::string, std::string>> attributes = ATTRIBUTES; \
	for (size_t i = 0; i < attributes.size(); i++) \
	{ \
		output_file << " "; \
		output_file << attributes[i].first << "=" << attributes[i].second; \
	} \
	output_file << ">\n"; \
	increase_ident(); \
	__VA_ARGS__; \
	decrease_ident(); \
	output_file << ident_str << "</" << (NAME) << ">\n"; \

#define XML_PRINT_ELEMENT_ATTR_INLINE(NAME, ATTRIBUTES, TEXT) \
	output_file << ident_str << "<" << (NAME); \
	std::vector<std::pair<std::string, std::string>> attributes = ATTRIBUTES; \
	for (size_t i = 0; i < attributes.size(); i++) \
	{ \
		output_file << " "; \
		output_file << attributes[i].first << "=" << attributes[i].second; \
	} \
	output_file << ">" << (TEXT) << "</" << (NAME) << ">\n"; \

#define XML_PRINT_ELEMENT(NAME, ...) \
	output_file << ident_str << "<" << (NAME) << ">\n"; \
	increase_ident(); \
	__VA_ARGS__; \
	decrease_ident(); \
	output_file << ident_str << "</" << (NAME) << ">\n"; \

#define XML_PRINT_ELEMENT_INLINE(NAME, TEXT) \
	output_file << ident_str << "<" << (NAME) << ">" << TEXT << "</" << (NAME) << ">\n"; \

void TreePrinter::xml_print
(
	std::string name,
	std::vector<std::pair<std::string, std::string>> attributes,
	std::vector<ASTNode*> children,
	std::string content
)
{
	output_file << ident_str << "<" << name;
	for (size_t i = 0; i < attributes.size(); i++)
	{
		output_file << " " << attributes[i].first << "=" << attributes[i].second;
	}
	output_file << ">";
	if (children.size())
	{
		output_file << "\n";
		increase_ident();
		for (size_t i = 0; i < children.size(); i++)
		{
			children[i]->accept(this);
		}
		output_file << content;
		decrease_ident();
		output_file << ident_str;
	}
	else
	{
		output_file << content;
	}
	output_file << "</" << name << ">\n";
}

void TreePrinter::process(ASTNode* node)
{
	node->accept(this);
}

void TreePrinter::visit(ASTNameSimple* node)
{
	xml_print(node_kind_to_string(node->kind), {}, {}, node->token.buffer);
}

void TreePrinter::visit(ASTNameQualified* node)
{
	xml_print
	(
		node_kind_to_string(node->kind),
		{},
		{
			node->qualifier,
			node->name
		}
		);
}

void TreePrinter::visit(ASTTypePrimitive* node)
{
	XML_PRINT_ELEMENT_INLINE
	(
		node_kind_to_string(node->kind),
		primitive_type_to_string(node->primitive_type)
	)

}

void TreePrinter::visit(ASTTypeReference* node)
{

}

void TreePrinter::visit(ASTTypeArray* node)
{

}

void TreePrinter::visit(ASTExpressionCast* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		//VISIT_CHILD(type)
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTExpressionGroup* node)
{
	node->expr->accept(this);
}

void TreePrinter::visit(ASTExpressionLiteral* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("value", node->token.buffer)
		)
		//VISIT_CHILD(type)
	)
}

void TreePrinter::visit(ASTExpressionUnary* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("op", token_kind_to_string(node->op.kind))
		),
		//VISIT_CHILD(type),
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTExpressionBinary* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("op", token_kind_to_string(node->op.kind))
		),
		//VISIT_CHILD(type),
		VISIT_CHILD(left),
		VISIT_CHILD(right)
	)
}
void TreePrinter::visit(ASTExpressionAssign* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		//VISIT_CHILD(type),
		VISIT_CHILD(assignee),
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTExpressionName* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		//VISIT_CHILD(type),
		VISIT_CHILD(name)
	)
}

void TreePrinter::visit(ASTExpressionFieldGet* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		//VISIT_CHILD(type),
		VISIT_CHILD(expr),
		VISIT_CHILD(field)
	)
}

void TreePrinter::visit(ASTExpressionFieldSet* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		//VISIT_CHILD(type),
		VISIT_CHILD(expr),
		VISIT_CHILD(field),
		VISIT_CHILD(value)
	)
}

void TreePrinter::visit(ASTExpressionArrayGet* node)
{

}

void TreePrinter::visit(ASTExpressionArraySet* node)
{

}

void TreePrinter::visit(ASTDeclarationVariable* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		//VISIT_CHILD(type),
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTStatementExpression* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTStatementBlock* node)
{
	XML_PRINT_ELEMENT
	(
		node_kind_to_string(node->kind),
		for (size_t i = 0; i < node->statements.size(); i++)
		{
			node->statements[i]->accept(this);
		}
	)
}