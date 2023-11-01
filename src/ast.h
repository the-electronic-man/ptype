#pragma once
#include "token.h"
#include "built_in_types.h"
#include "symbol.h"

#define ENUM_ITEM(x) \
	x,
#define ENUM_LIST \
	ENUM_ITEM(TYPE) \
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

struct ASTNode;

struct ASTNameSimple;
struct ASTNameQualified;

struct ASTType;

struct ASTExpressionCast;
struct ASTExpressionGroup;
struct ASTExpressionLiteral;
struct ASTExpressionUnary;
struct ASTExpressionBinary;
struct ASTExpressionAssign;
struct ASTExpressionName;
struct ASTExpressionFieldGet;
struct ASTExpressionFieldSet;
struct ASTExpressionArrayGet;
struct ASTExpressionArraySet;

struct ASTStatementExpression;
struct ASTStatementBlock;

struct ASTDeclarationVariable;

struct Visitor
{
	virtual void process(ASTNode* node) {}

	virtual void visit(ASTNameSimple* node) {}
	virtual void visit(ASTNameQualified* node) {}

	virtual void visit(ASTType* node) {}

	virtual void visit(ASTExpressionCast* node) {}
	virtual void visit(ASTExpressionGroup* node) {}
	virtual void visit(ASTExpressionLiteral* node) {}
	virtual void visit(ASTExpressionUnary* node) {}
	virtual void visit(ASTExpressionBinary* node) {}
	virtual void visit(ASTExpressionAssign* node) {}
	virtual void visit(ASTExpressionName* node) {}
	virtual void visit(ASTExpressionFieldGet* node) {}
	virtual void visit(ASTExpressionFieldSet* node) {}
	virtual void visit(ASTExpressionArrayGet* node) {}
	virtual void visit(ASTExpressionArraySet* node) {}

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

struct ASTName;
struct ASTType;

struct ASTType : ASTNode
{
	PrimitiveType primitive_type;
	ASTType* element_type = nullptr;
	ASTName* reference_name = nullptr;

	ASTType(PrimitiveType primitive_type);
	ASTType(ASTType* element_type);
	ASTType(ASTName* reference_name);
	~ASTType() override;
	void accept(Visitor* visitor) override;
	ASTNode* clone() override;
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



struct ASTNameSimple : ASTName
{
	Token token;

	ASTNameSimple(Token token);
	~ASTNameSimple() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTNameQualified : ASTName
{
	ASTName* qualifier = nullptr;
	ASTNameSimple* name = nullptr;

	ASTNameQualified(ASTName* qualifier, ASTNameSimple* name);
	~ASTNameQualified() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};


struct ASTExpressionLiteral : ASTExpression
{
	Token token;

	ASTExpressionLiteral(Token token);
	~ASTExpressionLiteral() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionUnary : ASTExpression
{
	Token op;
	ASTExpression* expr = nullptr;

	ASTExpressionUnary(Token op, ASTExpression* expr);
	~ASTExpressionUnary() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionBinary : ASTExpression
{
	Token op;
	ASTExpression* left = nullptr;
	ASTExpression* right = nullptr;

	ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right);
	~ASTExpressionBinary() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionFieldGet : ASTExpression
{
	ASTNameSimple* field;
	ASTExpression* expr = nullptr;

	ASTExpressionFieldGet(ASTExpression* expr, ASTNameSimple* field);
	~ASTExpressionFieldGet() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionFieldSet : ASTExpression
{
	ASTNameSimple* field;
	ASTExpression* expr = nullptr;
	ASTExpression* value = nullptr;

	ASTExpressionFieldSet(ASTExpression* expr, ASTNameSimple* field, ASTExpression* value);
	~ASTExpressionFieldSet() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionArrayGet : ASTExpression
{
	ASTExpression* expr = nullptr;
	ASTExpression* index = nullptr;

	ASTExpressionArrayGet(ASTExpression* expr, ASTExpression* index);
	~ASTExpressionArrayGet() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionArraySet : ASTExpression
{
	ASTExpression* expr = nullptr;
	ASTExpression* index = nullptr;
	ASTExpression* value = nullptr;

	ASTExpressionArraySet(ASTExpression* expr, ASTExpression* index, ASTExpression* value);
	~ASTExpressionArraySet() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionGroup : ASTExpression
{
	ASTExpression* expr = nullptr;

	ASTExpressionGroup(ASTExpression* expr);
	~ASTExpressionGroup() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionCast : ASTExpression
{
	ASTType* dst_type = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionCast(ASTExpression* expr, ASTType* dst_type);
	~ASTExpressionCast() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTExpressionAssign : ASTExpression
{
	ASTExpression* assignee = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionAssign(ASTExpression* assignee, ASTExpression* expr);
	~ASTExpressionAssign() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct Symbol;

struct ASTExpressionName : ASTExpression
{
	ASTName* name = nullptr;
	Symbol* symbol = nullptr;

	ASTExpressionName(ASTName* name);
	~ASTExpressionName() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};



struct SymbolVariable;

struct ASTDeclarationVariable : ASTDeclaration
{
	Token name;
	ASTType* type = nullptr;
	ASTExpression* expr = nullptr;
	SymbolVariable* var_symbol = nullptr;

	ASTDeclarationVariable(Token name, ASTType* type, ASTExpression* expr);
	~ASTDeclarationVariable();
	void accept(Visitor* visitor) override { visitor->visit(this); }
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
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTStatementExpression : ASTStatement
{
	ASTExpression* expr = nullptr;

	ASTStatementExpression(ASTExpression* expr);
	~ASTStatementExpression() override;
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};
