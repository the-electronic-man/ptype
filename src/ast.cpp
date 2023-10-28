#pragma once
#include "ast.h"

struct Visitor
{
	virtual void visit(ASTExpressionLiteral* node) = 0;
	virtual void visit(ASTExpressionUnary* node) = 0;
	virtual void visit(ASTExpressionBinary* node) = 0;
};



ASTExpressionLiteral::ASTExpressionLiteral(Token token)
{
	this->token = token;
}

ASTExpressionLiteral::~ASTExpressionLiteral() {}

void ASTExpressionLiteral::accept(Visitor* visitor)
{
	visitor->visit(this);
}




ASTExpressionUnary::ASTExpressionUnary(Token op, ASTExpression* expr)
{
	this->op = op;
	this->expr = expr;
}

ASTExpressionUnary::~ASTExpressionUnary()
{
	delete expr;
}

void ASTExpressionUnary::accept(Visitor* visitor)
{
	visitor->visit(this);
}




ASTExpressionBinary::ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right)
{
	this->op = op;
	this->left = left;
	this->right = right;
}

ASTExpressionBinary::~ASTExpressionBinary()
{
	delete left;
	delete right;
}

void ASTExpressionBinary::accept(Visitor* visitor)
{
	visitor->visit(this);
}