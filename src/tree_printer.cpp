#include "tree_printer.h"


#define VISIT_CHILD(CHILD) node->CHILD->accept(this)

#define XML_ATTR(NAME, VALUE) { NAME, VALUE }

#define XML_ATTR_LIST(...) { __VA_ARGS__ }

#define XML_PRINT_ELEMENT_ATTR(NAME, ATTRIBUTES, ...) \
	output_file << ident_str << "<" << (NAME); \
	std::vector<std::pair<std::string, std::string>> attributes = ATTRIBUTES; \
	for (size_t i = 0; i < attributes.size(); i++) \
	{ \
		output_file << " " << attributes[i].first << "=\"" << attributes[i].second << "\""; \
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
		output_file << attributes[i].first << "=\"" << attributes[i].second << "\""; \
	} \
	output_file << ">" << (TEXT) << "</" << (NAME) << ">\n"; \

#define XML_PRINT_ELEMENT_ATTR_INLINE_NO_TEXT(NAME, ATTRIBUTES) \
	output_file << ident_str << "<" << (NAME); \
	std::vector<std::pair<std::string, std::string>> attributes = ATTRIBUTES; \
	for (size_t i = 0; i < attributes.size(); i++) \
	{ \
		output_file << " "; \
		output_file << attributes[i].first << "=\"" << attributes[i].second << "\""; \
	} \
	output_file << "/>\n"; \

#define XML_PRINT_ELEMENT(NAME, ...) \
	output_file << ident_str << "<" << (NAME) << ">\n"; \
	increase_ident(); \
	__VA_ARGS__; \
	decrease_ident(); \
	output_file << ident_str << "</" << (NAME) << ">\n"; \

#define XML_PRINT_ELEMENT_INLINE(NAME, TEXT) \
	output_file << ident_str << "<" << (NAME) << ">" << TEXT << "</" << (NAME) << ">\n"; \

void TreePrinter::process(ASTNode* node)
{
	node->accept(this);
}

void TreePrinter::visit(ASTExpressionCast* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a")
		),
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTExpressionGroup* node)
{
	node->expr->accept(this);
}

void TreePrinter::visit(ASTExpressionLiteral* node)
{
	XML_PRINT_ELEMENT_ATTR_INLINE_NO_TEXT
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a"),
			XML_ATTR("value", node->token.buffer)
		)
	)
}

void TreePrinter::visit(ASTExpressionUnary* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a"),
			XML_ATTR("op", token_kind_to_string(node->op.kind))
		),
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
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a"),
			XML_ATTR("op", token_kind_to_string(node->op.kind))
		),
		VISIT_CHILD(left),
		VISIT_CHILD(right)
	)
}
void TreePrinter::visit(ASTExpressionAssign* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a")
		),
		VISIT_CHILD(assignee),
		VISIT_CHILD(expr)
	)
}

void TreePrinter::visit(ASTExpressionName* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a")
		),
		VISIT_CHILD(name)
	)
}

void TreePrinter::visit(ASTExpressionFieldGet* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a")
		),
		VISIT_CHILD(expr),
		VISIT_CHILD(field)
	)
}

void TreePrinter::visit(ASTExpressionFieldSet* node)
{
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a")
		),
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
	XML_PRINT_ELEMENT_ATTR
	(
		node_kind_to_string(node->kind),
		XML_ATTR_LIST
		(
			XML_ATTR("name", node->name.buffer),
			XML_ATTR("type", node->type ? node->type->to_string() : "n/a")
		),
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
		for (size_t i = 0; i < node->declarations.size(); i++)
		{
			node->declarations[i]->accept(this);
		}
	)
}