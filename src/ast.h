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
	virtual ~ASTNode() {}
	virtual void accept(Visitor* visitor) {}
	virtual ASTNode* clone() { return nullptr; }
};

struct ASTStatement : ASTNode
{
	virtual ~ASTStatement() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTDeclaration : ASTStatement
{
	virtual ~ASTDeclaration() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTType : ASTNode
{
	virtual ~ASTType() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTName : ASTNode
{
	virtual ~ASTName() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTExpression : ASTNode
{
	ASTType* type = nullptr;

	virtual ~ASTExpression() override 
	{
		delete type;
	}

	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};




struct ASTExpressionLiteral : ASTExpression
{
	Token token;

	ASTExpressionLiteral(Token token);
	~ASTExpressionLiteral() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};

struct ASTExpressionUnary : ASTExpression
{
	Token op;
	ASTExpression* expr = nullptr;

	ASTExpressionUnary(Token op, ASTExpression* expr);
	~ASTExpressionUnary() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};

struct ASTExpressionBinary : ASTExpression
{
	Token op;
	ASTExpression* left = nullptr;
	ASTExpression* right = nullptr;

	ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right);
	~ASTExpressionBinary() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};

struct ASTExpressionGroup : ASTExpression
{
	ASTExpression* expr = nullptr;

	ASTExpressionGroup(ASTExpression* expr);
	~ASTExpressionGroup() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};




struct ASTDeclarationVariable : ASTDeclaration
{
	Token name;
	ASTType* type = nullptr;
	ASTExpression* expr = nullptr;

	ASTDeclarationVariable(Token name, ASTType* type, ASTExpression* expr);
	~ASTDeclarationVariable();
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};

