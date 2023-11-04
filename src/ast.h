#pragma once
#include "token.h"
#include "built_in_types.h"
#include "symbol.h"
#include "debug.h"

#include <vector>
#include <algorithm>


#undef ENUM_ITEM
#define ENUM_ITEM(name, str) \
	name,

enum class NodeKind
{
#include "ast_kind.def"
};

const char* node_kind_to_string(NodeKind kind);

struct ASTNode;

struct ASTNameSimple;
struct ASTNameQualified;

struct ASTTypePrimitive;
struct ASTTypeReference;
struct ASTTypeArray;

struct ASTExpressionCast;
struct ASTExpressionGroup;
struct ASTExpressionLiteral;
struct ASTExpressionUnary;
struct ASTExpressionBinary;
struct ASTExpressionAssign;
struct ASTExpressionName;
struct ASTExpressionCall;
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

	virtual void visit(ASTTypePrimitive* node) {}
	virtual void visit(ASTTypeReference* node) {}
	virtual void visit(ASTTypeArray* node) {}

	virtual void visit(ASTExpressionCast* node) {}
	virtual void visit(ASTExpressionGroup* node) {}
	virtual void visit(ASTExpressionLiteral* node) {}
	virtual void visit(ASTExpressionUnary* node) {}
	virtual void visit(ASTExpressionBinary* node) {}
	virtual void visit(ASTExpressionAssign* node) {}
	virtual void visit(ASTExpressionName* node) {}
	virtual void visit(ASTExpressionCall* node) {}
	virtual void visit(ASTExpressionFieldGet* node) {}
	virtual void visit(ASTExpressionFieldSet* node) {}
	virtual void visit(ASTExpressionArrayGet* node) {}
	virtual void visit(ASTExpressionArraySet* node) {}

	virtual void visit(ASTStatementExpression* node) {}
	virtual void visit(ASTStatementBlock* node) {}

	virtual void visit(ASTDeclarationVariable* node) {}
};
struct Symbol;

struct ASTNode
{
protected:

	ASTNode(NodeKind kind) { this->kind = kind; }

public:

	NodeKind kind;
	ASTNode* parent = nullptr;
	std::vector<ASTNode*> children;

	void attach_child(ASTNode* child)
	{
		if (!child)
		{
			children.push_back(nullptr);
			return;
		}
		else
		{
			assert(find_child(child) == children.end());
			children.push_back(child);
			child->parent = this;
		}
	}

	void detach_child(ASTNode* child)
	{
		auto it = find_child(child);
		if (it != children.end())
		{
			children.erase(it);
			return;
		}
		assert(false);
	}

	std::vector<ASTNode*>::iterator find_child(ASTNode* child)
	{
		return std::find(children.begin(), children.end(), child);
	}

	virtual ~ASTNode()
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			delete children[i];
		}
	}

	virtual void accept(Visitor* visitor) {}

	virtual ASTNode* clone()
	{
		ASTNode* node = new ASTNode(kind);
		for (size_t i = 0; i < children.size(); i++)
		{
			node->attach_child(children[i]->clone());
		}
		return node;
	}

	size_t get_children_count()
	{
		return children.size();
	}

	void replace_child(ASTNode* old_child, ASTNode* new_child)
	{
		auto it = find_child(old_child);
		assert(it != children.end());
		*it = new_child;
	}
};

struct ASTStatement : ASTNode
{
	ASTStatement(NodeKind kind) : ASTNode(kind) {}
	virtual ~ASTStatement() override {}
	virtual void accept(Visitor* visitor) override = 0;
};

struct ASTName;
struct ASTNameSimple;
struct ASTNameQualified;
struct ASTType;

struct ASTDeclaration : ASTStatement
{
	ASTDeclaration(ASTNameSimple* name, NodeKind kind) : ASTStatement(kind)
	{
		attach_child((ASTNode*)name);
	}
	virtual ~ASTDeclaration() override {}
	virtual void accept(Visitor* visitor) override = 0;
	virtual ASTNode* clone() override
	{
		return ASTStatement::clone();
	}
	ASTNameSimple* get_name()
	{
		return (ASTNameSimple*)children[0];
	}
};

struct ASTType : ASTNode
{
	ASTType(NodeKind kind) : ASTNode(kind) {}
	virtual ~ASTType() override {}
	virtual void accept(Visitor* visitor) override = 0;
	virtual ASTNode* clone() override { return ASTNode::clone(); }
};

struct ASTTypePrimitive : ASTType
{
	PrimitiveType primitive_type;

	ASTTypePrimitive(PrimitiveType primitive_type) : ASTType(NodeKind::TYPE_PRIMITIVE)
	{
		this->primitive_type = primitive_type;
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTNode* clone() override
	{
		ASTTypePrimitive* node = (ASTTypePrimitive*)ASTType::clone();
		node->primitive_type = primitive_type;
		return node;
	}
	PrimitiveType get_name()
	{
		return primitive_type;
	}
};

struct ASTTypeReference : ASTType
{
	ASTTypeReference(ASTName* name) : ASTType(NodeKind::TYPE_REFERENCE)
	{
		attach_child((ASTNode*)name);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTName* get_name()
	{
		return (ASTName*)children[0];
	}
};

struct ASTTypeArray : ASTType
{
	ASTTypeArray(ASTType* subtype) : ASTType(NodeKind::TYPE_ARRAY)
	{
		attach_child(subtype);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTType* get_subtype()
	{
		return (ASTType*)children[0];
	}
};

struct ASTName : ASTNode
{
	Symbol* symbol = nullptr;
	ASTName(NodeKind kind) : ASTNode(kind) {}
	virtual void accept(Visitor* visitor) override = 0;
	virtual ASTNode* clone() override { return ASTNode::clone(); }
};

struct ASTNameSimple : ASTName
{
	Token token;
	ASTNameSimple(Token token) : ASTName(NodeKind::NAME_SIMPLE)
	{
		this->token = token;
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTNode* clone() override
	{
		ASTNameSimple* node = (ASTNameSimple*)ASTName::clone();
		node->token = token;
		return node;
	}
};

struct ASTNameQualified : ASTName
{
	ASTNameQualified(ASTName* qualifier, ASTNameSimple* name) : ASTName(NodeKind::NAME_QUALIFIED)
	{
		attach_child(qualifier);
		attach_child(name);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTName* get_qualifier()
	{
		return (ASTName*)children[0];
	}
	ASTNameSimple* get_name()
	{
		return (ASTNameSimple*)children[1];
	}
};

struct ASTExpression : ASTNode
{
	ASTType* type = nullptr;
	ASTExpression(NodeKind kind) : ASTNode(kind)
	{

	}
	virtual ~ASTExpression() override
	{
		delete type;
	}
	virtual void accept(Visitor* visitor) override = 0;
	bool is_type_resolved()
	{
		return type != nullptr;
	}
};


struct ASTExpressionLiteral : ASTExpression
{
	Token token;

	ASTExpressionLiteral(Token token) : ASTExpression(NodeKind::EXPR_LITERAL)
	{
		this->token = token;
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTNode* clone()
	{
		ASTExpressionLiteral* node = (ASTExpressionLiteral*)ASTExpression::clone();
		node->token = token;
		return node;
	}
};

struct ASTExpressionUnary : ASTExpression
{
	Token op;
	ASTExpressionUnary(Token op, ASTExpression* expr) : ASTExpression(NodeKind::EXPR_UNARY)
	{
		this->op = op;
		attach_child(expr);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTNode* clone()
	{
		ASTExpressionUnary* node = (ASTExpressionUnary*)ASTExpression::clone();
		node->op = op;
		return node;
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
};

struct ASTExpressionBinary : ASTExpression
{
	Token op;
	ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right)
		: ASTExpression(NodeKind::EXPR_BINARY)
	{
		this->op = op;
		attach_child(left);
		attach_child(right);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTNode* clone()
	{
		ASTExpressionBinary* node = (ASTExpressionBinary*)ASTExpression::clone();
		node->op = op;
		return node;
	}
	ASTExpression* get_left()
	{
		return (ASTExpression*)children[0];
	}
	ASTExpression* get_right()
	{
		return (ASTExpression*)children[1];
	}
};

struct ASTExpressionFieldGet : ASTExpression
{
	ASTExpressionFieldGet(ASTExpression* expr, ASTNameSimple* field)
		: ASTExpression(NodeKind::EXPR_FIELD_GET)
	{
		attach_child(expr);
		attach_child(field);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
	ASTExpression* get_field()
	{
		return (ASTExpression*)children[1];
	}
};

struct ASTExpressionFieldSet : ASTExpression
{
	ASTExpressionFieldSet(ASTExpression* expr, ASTNameSimple* field, ASTExpression* value)
		: ASTExpression(NodeKind::EXPR_FIELD_SET)
	{
		attach_child(expr);
		attach_child(field);
		attach_child(value);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
	ASTNameSimple* get_field()
	{
		return (ASTNameSimple*)children[1];
	}
	ASTExpression* get_value()
	{
		return (ASTExpression*)children[2];
	}
};

struct ASTExpressionArrayGet : ASTExpression
{
	ASTExpressionArrayGet(ASTExpression* expr, ASTNameSimple* index)
		: ASTExpression(NodeKind::EXPR_ARRAY_GET)
	{
		attach_child(expr);
		attach_child(index);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
	ASTExpression* get_index()
	{
		return (ASTExpression*)children[1];
	}
};

struct ASTExpressionArraySet : ASTExpression
{
	ASTExpressionArraySet(ASTExpression* expr, ASTNameSimple* index, ASTNameSimple* value)
		: ASTExpression(NodeKind::EXPR_ARRAY_SET)
	{
		attach_child(expr);
		attach_child(index);
		attach_child(value);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
	ASTExpression* get_index()
	{
		return (ASTExpression*)children[1];
	}
	ASTExpression* get_value()
	{
		return (ASTExpression*)children[2];
	}
};

struct ASTExpressionGroup : ASTExpression
{
	ASTExpressionGroup(ASTExpression* expr) : ASTExpression(NodeKind::EXPR_GROUP)
	{
		attach_child(expr);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
};

struct ASTExpressionCast : ASTExpression
{
	ASTExpressionCast(ASTExpression* expr, ASTType* type)
		: ASTExpression(NodeKind::EXPR_CAST)
	{
		attach_child(expr);

		this->type = type;
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
};

struct ASTExpressionAssign : ASTExpression
{
	ASTExpressionAssign(ASTExpression* assignee, ASTExpression* expr)
		: ASTExpression(NodeKind::EXPR_ASSIGN)
	{
		attach_child(assignee);
		attach_child(expr);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_assignee()
	{
		return (ASTExpression*)children[0];
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[1];
	}
};

struct Symbol;

struct ASTExpressionName : ASTExpression
{
	ASTExpressionName(ASTName* name) : ASTExpression(NodeKind::EXPR_NAME)
	{
		attach_child(name);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTName* get_name()
	{
		return (ASTName*)children[0];
	}
};



struct SymbolVariable;

struct ASTDeclarationVariable : ASTDeclaration
{
	ASTDeclarationVariable(ASTNameSimple* name, ASTType* type, ASTExpression* expr)
		: ASTDeclaration(name, NodeKind::DECL_VAR)
	{
		attach_child(type);
		attach_child(expr);
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	void deduce_type_from_expr()
	{
		if (!children[1])
		{
			children[1] = get_expr()->type->clone();
		}
	}

	ASTType* get_type()
	{
		return (ASTType*)children[1];
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[2];
	}
};



struct Scope;

struct ASTStatementBlock : ASTStatement
{
	Scope* scope = nullptr;
	int32_t var_index = 0;

	ASTStatementBlock(std::vector<ASTStatement*>& statements)
		: ASTStatement(NodeKind::STMT_BLOCK)
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			attach_child(statements[i]);
		}
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_stmt(size_t index)
	{
		return (ASTExpression*)children[index];
	}
};

struct ASTStatementExpression : ASTStatement
{
	ASTStatementExpression(ASTExpression* expr) : ASTStatement(NodeKind::STMT_EXPR)
	{
		attach_child(expr);
	}
	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}
	ASTExpression* get_expr()
	{
		return (ASTExpression*)children[0];
	}
};
