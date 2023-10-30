#pragma once
#include "token.h"
#include "built_in_types.h"
#include "symbol.h"

#define ENUM_ITEM(x) \
	x,
#define ENUM_LIST \
	ENUM_ITEM(TYPE_PRIMITIVE) \
	ENUM_ITEM(TYPE_REFERENCE) \
	ENUM_ITEM(TYPE_ARRAY) \
	ENUM_ITEM(NAME_SIMPLE) \
	ENUM_ITEM(NAME_QUALIFIED) \
	ENUM_ITEM(EXPR_GROUP) \
	ENUM_ITEM(EXPR_LITERAL) \
	ENUM_ITEM(EXPR_UNARY) \
	ENUM_ITEM(EXPR_BINARY) \
	ENUM_ITEM(EXPR_CAST) \
	ENUM_ITEM(EXPR_ARRAY_GET) \
	ENUM_ITEM(EXPR_ARRAY_SET) \
	ENUM_ITEM(EXPR_FIELD_GET) \
	ENUM_ITEM(EXPR_FIELD_SET) \
	ENUM_ITEM(EXPR_ASSIGN) \
	ENUM_ITEM(EXPR_NAME) \
	ENUM_ITEM(STMT_EXPR) \
	ENUM_ITEM(STMT_BLOCK) \
	ENUM_ITEM(DECL_VAR) \

enum class NodeKind
{
	ENUM_LIST
};

#define ENUM_ITEM(x) \
	case NodeKind::x: return #x;

static const char* node_kind_to_string(NodeKind kind)
{
	switch (kind)
	{
		ENUM_LIST
		default: return "undefined";
	}
}

#define ast_node_accept_visitor \
	void accept(Visitor* visitor) override { visitor->visit(this); }


struct ASTNode;
struct ASTNameSimple;
struct ASTNameQualified;
struct ASTTypePrimitive;
struct ASTExpressionCast;
struct ASTExpressionGroup;
struct ASTExpressionLiteral;
struct ASTExpressionUnary;
struct ASTExpressionBinary;
struct ASTExpressionAssign;
struct ASTExpressionName;
struct ASTStatementExpression;
struct ASTStatementBlock;
struct ASTDeclarationVariable;

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
	virtual void visit(ASTExpressionAssign* node) {}
	virtual void visit(ASTExpressionName* node) {}

	virtual void visit(ASTStatementExpression* node) {}
	virtual void visit(ASTStatementBlock* node) {}

	virtual void visit(ASTDeclarationVariable* node) {}
};


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
	ASTNode* clone() override;
	ast_node_accept_visitor
};




struct ASTNameSimple : ASTName
{
	Token token;

	ASTNameSimple(Token token);
	~ASTNameSimple() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTNameQualified : ASTName
{
	ASTName* qualifier = nullptr;
	ASTNameSimple* name = nullptr;

	ASTNameQualified(ASTName* qualifier, ASTNameSimple* name);
	~ASTNameQualified() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};


struct ASTExpressionLiteral : ASTExpression
{
	Token token;

	ASTExpressionLiteral(Token token);
	~ASTExpressionLiteral() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTExpressionUnary : ASTExpression
{
	Token op;
	ASTExpression* expr = nullptr;

	ASTExpressionUnary(Token op, ASTExpression* expr);
	~ASTExpressionUnary() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTExpressionBinary : ASTExpression
{
	Token op;
	ASTExpression* left = nullptr;
	ASTExpression* right = nullptr;

	ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right);
	~ASTExpressionBinary() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTExpressionGroup : ASTExpression
{
	ASTExpression* expr = nullptr;

	ASTExpressionGroup(ASTExpression* expr);
	~ASTExpressionGroup() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTExpressionCast : ASTExpression
{
	ASTType* dst_type = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionCast(ASTExpression* expr, ASTType* dst_type);
	~ASTExpressionCast() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTExpressionAssign : ASTExpression
{
	ASTExpression* assignee = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionAssign(ASTExpression* assignee, ASTExpression* expr);
	~ASTExpressionAssign() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTExpressionName : ASTExpression
{
	ASTName* name = nullptr;

	ASTExpressionName(ASTName* name);
	~ASTExpressionName() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};




struct ASTDeclarationVariable : ASTDeclaration
{
	Token name;
	ASTType* type = nullptr;
	ASTExpression* expr = nullptr;

	ASTDeclarationVariable(Token name, ASTType* type, ASTExpression* expr);
	~ASTDeclarationVariable();
	ast_node_accept_visitor
	ASTNode* clone() override;
};



struct Scope;

struct ASTStatementBlock : ASTStatement
{
	Scope* scope = nullptr;
	std::vector<ASTStatement*> statements;
	int32_t var_index = 0;

	ASTStatementBlock(std::vector<ASTStatement*>& statements);
	~ASTStatementBlock() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};

struct ASTStatementExpression : ASTStatement
{
	ASTExpression* expr = nullptr;

	ASTStatementExpression(ASTExpression* expr);
	~ASTStatementExpression() override;
	ast_node_accept_visitor
	ASTNode* clone() override;
};
