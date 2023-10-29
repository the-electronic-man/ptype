#pragma once
#include "token.h"
#include "built_in_types.h"
#include "symbol.h"

enum class NodeKind
{
	TYPE_PRIMITIVE,
	TYPE_REFERENCE,
	TYPE_ARRAY,

	NAME_SIMPLE,
	NAME_QUALIFIED,

	EXPR_GROUP,
	EXPR_LITERAL,
	EXPR_UNARY,
	EXPR_BINARY,
	EXPR_CAST,
	EXPR_ARRAY_GET,
	EXPR_ARRAY_SET,
	EXPR_FIELD_GET,
	EXPR_FIELD_SET,

	STMT_EXPR,
	STMT_BLOCK,

	DECL_VAR,
};

struct Visitor;

struct ASTNode
{
	NodeKind kind;

	ASTNode(NodeKind kind);
	virtual ~ASTNode() {}
	virtual void accept(Visitor* visitor) {}
	virtual ASTNode* clone() { return nullptr; }
};

struct ASTStatement : ASTNode
{
	ASTStatement(NodeKind kind);
	virtual ~ASTStatement() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTDeclaration : ASTStatement
{
	ASTDeclaration(NodeKind kind);
	virtual ~ASTDeclaration() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTType : ASTNode
{
	ASTType(NodeKind kind);
	virtual ~ASTType() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTName : ASTNode
{
	ASTName(NodeKind kind);
	virtual ~ASTName() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTExpression : ASTNode
{
	ASTType* type = nullptr;

	ASTExpression(NodeKind kind);
	virtual ~ASTExpression() override;
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};




struct ASTTypePrimitive : ASTType
{
	PrimitiveType primitive_type;
	ASTTypePrimitive(PrimitiveType primitive_type);
	~ASTTypePrimitive() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};




struct ASTNameSimple : ASTName
{
	Token token;

	ASTNameSimple(Token token);
	~ASTNameSimple() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};

struct ASTNameQualified : ASTName
{
	ASTName* qualifier = nullptr;
	ASTNameSimple* name = nullptr;

	ASTNameQualified(ASTName* qualifier, ASTNameSimple* name);
	~ASTNameQualified() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
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

struct ASTExpressionCast : ASTExpression
{
	ASTType* dst_type = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionCast(ASTExpression* expr, ASTType* dst_type);
	~ASTExpressionCast() override;
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





struct ASTStatementBlock : ASTStatement
{
	Scope* scope = nullptr;
	std::vector<ASTStatement*> statements;
	int32_t var_index = 0;

	ASTStatementBlock(std::vector<ASTStatement*>& statements);
	~ASTStatementBlock() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};

struct ASTStatementExpression : ASTStatement
{
	ASTExpression* expr = nullptr;

	ASTStatementExpression(ASTExpression* expr);
	~ASTStatementExpression() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
};













struct Visitor
{
	enum class PassType
	{
		Declare,
		Resolve
	};

	PassType pass_type;

	virtual void process(ASTNode* node) {}

	virtual void visit(ASTNameSimple* node) {}
	virtual void visit(ASTNameQualified* node) {}

	virtual void visit(ASTTypePrimitive* node) {}

	virtual void visit(ASTExpressionCast* node) {}
	virtual void visit(ASTExpressionGroup* node) {}
	virtual void visit(ASTExpressionLiteral* node) {}
	virtual void visit(ASTExpressionUnary* node) {}
	virtual void visit(ASTExpressionBinary* node) {}

	virtual void visit(ASTStatementExpression* node) {}
	virtual void visit(ASTStatementBlock* node) {}

	virtual void visit(ASTDeclarationVariable* node) {}
};
