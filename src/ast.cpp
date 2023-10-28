#pragma once
#include "ast.h"

struct Visitor
{
	virtual void visit(ASTExpressionGroup* node) = 0;
	virtual void visit(ASTExpressionLiteral* node) = 0;
	virtual void visit(ASTExpressionUnary* node) = 0;
	virtual void visit(ASTExpressionBinary* node) = 0;

	virtual void visit(ASTDeclarationVariable* node) = 0;
};



ASTExpressionLiteral::ASTExpressionLiteral(Token token)
{
	this->kind = NodeKind::EXPR_LITERAL;
	this->token = token;
}

ASTExpressionLiteral::~ASTExpressionLiteral() {}

void ASTExpressionLiteral::accept(Visitor* visitor)
{
	visitor->visit(this);
}

ASTNode* ASTExpressionLiteral::clone()
{
	ASTExpressionLiteral* node = new ASTExpressionLiteral(token);
	return node;
}




ASTExpressionUnary::ASTExpressionUnary(Token op, ASTExpression* expr)
{
	this->kind = NodeKind::EXPR_UNARY;
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

ASTNode* ASTExpressionUnary::clone()
{
	ASTExpressionUnary* node = 
		new ASTExpressionUnary
		(
			op, 
			(ASTExpression*)expr->clone()
		);
	return node;
}





ASTExpressionBinary::ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right)
{
	this->kind = NodeKind::EXPR_BINARY;
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

ASTNode* ASTExpressionBinary::clone()
{
	ASTExpressionBinary* node =
		new ASTExpressionBinary
		(
			op,
			(ASTExpression*)left->clone(),
			(ASTExpression*)right->clone()
		);
	return node;
}




ASTExpressionGroup::ASTExpressionGroup(ASTExpression* expr)
{
	this->kind = NodeKind::EXPR_GROUP;
	this->expr = expr;
}

ASTExpressionGroup::~ASTExpressionGroup()
{
	delete expr;
}

void ASTExpressionGroup::accept(Visitor* visitor)
{
	visitor->visit(this);
}

ASTNode* ASTExpressionGroup::clone()
{
	ASTExpressionGroup* node =
		new ASTExpressionGroup((ASTExpression*)expr->clone());
	return node;
}






ASTDeclarationVariable::ASTDeclarationVariable(Token name, ASTType* type, ASTExpression* expr)
{
	this->kind = NodeKind::DECL_VAR;
	this->name = name;
	this->type = type;
	this->expr = expr;
}

ASTDeclarationVariable::~ASTDeclarationVariable()
{
	//delete name;
	delete type;
	delete expr;
}

void ASTDeclarationVariable::accept(Visitor* visitor)
{
	visitor->visit(this);
}








