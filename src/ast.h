#pragma once
#include "token.h"


enum class NodeKind
{
	EXPR_GROUP,
	EXPR_LITERAL,
	EXPR_UNARY,
	EXPR_BINARY,
	EXPR_ARRAY_GET,
	EXPR_ARRAY_SET,
	EXPR_FIELD_GET,
	EXPR_FIELD_SET,

	DECL_VAR,
};

struct Visitor;

struct ASTNode
{
	NodeKind kind;
	virtual ~ASTNode() = 0;
	virtual void accept(Visitor* visitor) = 0;
};

struct ASTStatement : ASTNode
{
	virtual ~ASTStatement() override = 0;
	virtual void accept(Visitor* visitor) override = 0;
};

struct ASTDeclaration : ASTStatement
{
	virtual ~ASTDeclaration() override = 0;
	virtual void accept(Visitor* visitor) override = 0;
};

struct ASTType : ASTNode
{
	virtual ~ASTType() override = 0;
	virtual void accept(Visitor* visitor) override = 0;
};

struct ASTName : ASTNode
{
	virtual ~ASTName() override = 0;
	virtual void accept(Visitor* visitor) override = 0;
};

struct ASTExpression : ASTNode
{
	ASTType* type;
	virtual ~ASTExpression() override = 0;
	virtual void accept(Visitor* visitor) override = 0;
};



struct ASTExpressionLiteral : ASTExpression
{
	Token token;

	ASTExpressionLiteral(Token token);
	~ASTExpressionLiteral();
	void accept(Visitor* visitor) override;
};

struct ASTExpressionUnary : ASTExpression
{
	Token op;
	ASTExpression* expr = nullptr;

	ASTExpressionUnary(Token op, ASTExpression* expr);
	~ASTExpressionUnary();
	void accept(Visitor* visitor) override;
};

struct ASTExpressionBinary : ASTExpression
{
	Token op;
	ASTExpression* left = nullptr;
	ASTExpression* right = nullptr;

	ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right);
	~ASTExpressionBinary();
	void accept(Visitor* visitor) override;
};


