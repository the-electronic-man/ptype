#include "compiler.h"

void Compiler::process(ASTNode* node)
{
	node->accept(this);
}

void Compiler::visit(ASTExpressionCast* node)
{

}

void Compiler::visit(ASTExpressionGroup* node)
{
	node->expr->accept(this);
}

void Compiler::visit(ASTExpressionLiteral* node)
{

}

void Compiler::visit(ASTExpressionUnary* node)
{

}

void Compiler::visit(ASTExpressionBinary* node)
{

}

void Compiler::visit(ASTExpressionAssign* node)
{

}

void Compiler::visit(ASTExpressionName* node)
{

}

void Compiler::visit(ASTDeclarationVariable* node)
{

}