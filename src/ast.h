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

struct ASTName;
struct ASTNameGlobal;
struct ASTNameSimple;
struct ASTNameQualified;

struct ASTType;
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

	virtual void visit(ASTNameGlobal* node) {}
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
	NodeKind kind;
	ASTNode* parent = nullptr;
	ASTNode(NodeKind kind) { this->kind = kind; }
	virtual ~ASTNode() {}
	virtual void accept(Visitor* visitor) = 0;
	virtual ASTNode* clone() = 0;
};

struct ASTStatement : ASTNode
{
	ASTStatement(NodeKind kind) : ASTNode(kind) {}
};


struct ASTDeclaration : ASTStatement
{
	Token name;
	Symbol* symbol = nullptr;

	ASTDeclaration(Token name, NodeKind kind) : ASTStatement(kind)
	{
		this->name = name;
	}
};

struct ASTType : ASTNode
{
	ASTType(NodeKind kind) : ASTNode(kind) {}
	virtual bool is_equal(ASTType* other)
	{
		return kind == other->kind;
	}
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
		ASTTypePrimitive* node = new ASTTypePrimitive(primitive_type);
		return node;
	}

	virtual bool is_equal(ASTType* other)
	{
		return
			ASTType::is_equal(other) &&
			primitive_type == ((ASTTypePrimitive*)other)->primitive_type;
	}
};

struct ASTTypeArray : ASTType
{
	ASTType* subtype = nullptr;

	ASTTypeArray(ASTType* subtype) : ASTType(NodeKind::TYPE_ARRAY)
	{
		this->subtype = subtype;
		subtype->parent = this;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		ASTTypeArray* node =
			new ASTTypeArray((ASTType*)subtype->clone());
		return node;
	}

	virtual bool is_equal(ASTType* other) override
	{
		return false;
	}
};

struct ASTName : ASTNode
{
	Symbol* symbol = nullptr;
	ASTName(NodeKind kind) : ASTNode(kind) {}
};


struct ASTTypeReference : ASTType
{
	ASTName* name = nullptr;

	ASTTypeReference(ASTName* name) : ASTType(NodeKind::TYPE_REFERENCE)
	{
		this->name = name;
		name->parent = this;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		ASTTypeReference* node =
			new ASTTypeReference((ASTName*)name->clone());
		return node;
	}

	virtual bool is_equal(ASTType* other) override
	{
		return false;
	}
};

struct ASTNameGlobal : ASTName
{
	ASTNameGlobal() : ASTName(NodeKind::NAME_GLOBAL) {}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		ASTNameGlobal* node = (ASTNameGlobal*)ASTName::clone();
		return node;
	}
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

	ASTNode* clone()
	{
		ASTNameSimple* node =
			new ASTNameSimple
			(
				token
			);
		return node;
	}
};

struct ASTNameQualified : ASTName
{
	ASTName* qualifier = nullptr;
	ASTNameSimple* name = nullptr;

	ASTNameQualified(ASTName* qualifier, ASTNameSimple* name) : ASTName(NodeKind::NAME_QUALIFIED)
	{
		this->qualifier = qualifier;
		qualifier->parent = this;
		this->name = name;
		name->parent = this;
	}

	~ASTNameQualified()
	{

	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTNameQualified* node =
			new ASTNameQualified
			(
				(ASTName*)qualifier->clone(),
				(ASTNameSimple*)name->clone()
			);
		return node;
	}
};

struct ASTExpression : ASTNode
{
	ASTType* type = nullptr;

	ASTExpression(NodeKind kind) : ASTNode(kind) {}

	~ASTExpression() override
	{
		if (type)
		{
			delete type;
		}
	}

	virtual void accept(Visitor* visitor) override = 0;

	void resolve_type(ASTType* type)
	{
		this->type = type;
		type->parent = this;
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
		ASTExpressionLiteral* node = new ASTExpressionLiteral(token);
		return node;
	}
};

struct ASTExpressionUnary : ASTExpression
{
	Token op;
	ASTExpression* expr = nullptr;

	ASTExpressionUnary(Token op, ASTExpression* expr) : ASTExpression(NodeKind::EXPR_UNARY)
	{
		this->op = op;

		this->expr = expr;
		expr->parent = this;
	}

	~ASTExpressionUnary()
	{
		delete expr;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionUnary* node = new ASTExpressionUnary(op, (ASTExpression*)expr->clone());
		return node;
	}
};

struct ASTExpressionBinary : ASTExpression
{
	Token op;
	ASTExpression* left = nullptr;
	ASTExpression* right = nullptr;
	ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right)
		: ASTExpression(NodeKind::EXPR_BINARY)
	{
		this->op = op;

		this->left = left;
		left->parent = this;

		this->right = right;
		right->parent = this;
	}

	~ASTExpressionBinary()
	{
		delete left;
		delete right;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionBinary* node =
			new ASTExpressionBinary
			(
				op,
				(ASTExpression*)left->clone(),
				(ASTExpression*)right->clone()
			);
		node->op = op;
		return node;
	}
};

struct ASTExpressionFieldGet : ASTExpression
{
	ASTExpression* expr = nullptr;
	ASTNameSimple* field = nullptr;

	ASTExpressionFieldGet(ASTExpression* expr, ASTNameSimple* field)
		: ASTExpression(NodeKind::EXPR_FIELD_GET)
	{
		this->expr = expr;
		expr->parent = this;

		this->field = field;
		field->parent = this;
	}

	~ASTExpressionFieldGet()
	{
		delete expr;
		delete field;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionFieldGet* node =
			new ASTExpressionFieldGet
			(
				(ASTExpression*)expr->clone(),
				(ASTNameSimple*)field->clone()
			);
		return node;
	}
};

struct ASTExpressionFieldSet : ASTExpression
{
	ASTExpression* expr = nullptr;
	ASTNameSimple* field = nullptr;
	ASTExpression* value = nullptr;

	ASTExpressionFieldSet(ASTExpression* expr, ASTNameSimple* field, ASTExpression* value)
		: ASTExpression(NodeKind::EXPR_FIELD_SET)
	{
		this->expr = expr;
		expr->parent = this;

		this->field = field;
		field->parent = this;

		this->value = value;
		value->parent = this;
	}

	~ASTExpressionFieldSet()
	{
		delete expr;
		delete field;
		delete value;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionFieldSet* node =
			new ASTExpressionFieldSet
			(
				(ASTExpression*)expr->clone(),
				(ASTNameSimple*)field->clone(),
				(ASTExpression*)value->clone()
			);
		return node;
	}
};

struct ASTExpressionArrayGet : ASTExpression
{
	ASTExpression* expr = nullptr;
	ASTExpression* index = nullptr;

	ASTExpressionArrayGet(ASTExpression* expr, ASTExpression* index)
		: ASTExpression(NodeKind::EXPR_ARRAY_GET)
	{
		this->expr = expr;
		expr->parent = this;

		this->index = index;
		index->parent = this;
	}

	~ASTExpressionArrayGet()
	{
		delete expr;
		delete index;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionArrayGet* node =
			new ASTExpressionArrayGet
			(
				(ASTExpression*)expr->clone(),
				(ASTExpression*)index->clone()
			);
		return node;
	}

};

struct ASTExpressionArraySet : ASTExpression
{
	ASTExpression* expr = nullptr;
	ASTExpression* index = nullptr;
	ASTExpression* value = nullptr;

	ASTExpressionArraySet(ASTExpression* expr, ASTExpression* index, ASTExpression* value)
		: ASTExpression(NodeKind::EXPR_ARRAY_SET)
	{
		this->expr = expr;
		expr->parent = this;

		this->index = index;
		index->parent = this;

		this->value = value;
		value->parent = this;
	}

	~ASTExpressionArraySet()
	{
		delete expr;
		delete index;
		delete value;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionArraySet* node =
			new ASTExpressionArraySet
			(
				(ASTExpression*)expr->clone(),
				(ASTExpression*)index->clone(),
				(ASTExpression*)value->clone()
			);
		return node;
	}
};

struct ASTExpressionGroup : ASTExpression
{
	ASTExpression* expr = nullptr;

	ASTExpressionGroup(ASTExpression* expr) : ASTExpression(NodeKind::EXPR_GROUP)
	{
		this->expr = expr;
		expr->parent = this;
	}

	~ASTExpressionGroup()
	{
		delete expr;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionGroup* node =
			new ASTExpressionGroup
			(
				(ASTExpression*)expr->clone()
			);
		return node;
	}
};

struct ASTExpressionCast : ASTExpression
{
	ASTExpression* expr = nullptr;

	ASTExpressionCast(ASTExpression* expr, ASTType* type)
		: ASTExpression(NodeKind::EXPR_CAST)
	{
		this->expr = expr;
		expr->parent = this;

		this->type = type;
		type->parent = this;
	}

	~ASTExpressionCast()
	{
		delete expr;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionCast* node =
			new ASTExpressionCast
			(
				(ASTExpression*)expr->clone(),
				(ASTType*)type->clone()
			);
		return node;
	}
};

struct ASTExpressionAssign : ASTExpression
{
	ASTExpression* assignee = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionAssign(ASTExpression* assignee, ASTExpression* expr)
		: ASTExpression(NodeKind::EXPR_ASSIGN)
	{
		this->assignee = assignee;
		assignee->parent = this;

		this->expr = expr;
		expr->parent = this;
	}

	~ASTExpressionAssign()
	{
		delete assignee;
		if (expr)
		{
			delete expr;
		}
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionAssign* node =
			new ASTExpressionAssign
			(
				(ASTExpression*)assignee->clone(),
				(ASTExpression*)expr->clone()
			);
		return node;
	}
};

struct Symbol;

struct ASTExpressionName : ASTExpression
{
	ASTName* name = nullptr;

	ASTExpressionName(ASTName* name) : ASTExpression(NodeKind::EXPR_NAME)
	{
		this->name = name;
		name->parent = this;
	}

	~ASTExpressionName()
	{
		delete name;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTExpressionName* node =
			new ASTExpressionName
			(
				(ASTName*)name->clone()
			);
		return node;
	}
};



struct ASTDeclarationVariable : ASTDeclaration
{
	Token name;
	ASTType* type = nullptr;
	ASTExpression* expr = nullptr;

	ASTDeclarationVariable(Token name, ASTType* type, ASTExpression* expr)
		: ASTDeclaration(name, NodeKind::DECL_VAR)
	{
		this->name = name;

		this->type = type;
		type->parent = this;

		this->expr = expr;
		expr->parent = this;
	}

	~ASTDeclarationVariable()
	{
		delete type;
		delete expr;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTDeclarationVariable* node =
			new ASTDeclarationVariable
			(
				name,
				(ASTType*)type->clone(),
				(ASTExpression*)expr->clone()
			);
		return node;
	}
};



struct Scope;

struct ASTStatementBlock : ASTStatement
{
	Scope* scope = nullptr;
	int32_t var_index = 0;
	std::vector<ASTStatement*> statements;

	ASTStatementBlock(std::vector<ASTStatement*>& statements)
		: ASTStatement(NodeKind::STMT_BLOCK)
	{
		this->statements = statements;
		for (size_t i = 0; i < statements.size(); i++)
		{
			this->statements[i]->parent = this;
		}
	}

	~ASTStatementBlock()
	{
		for (size_t i = 0; i < statements.size(); i++)
		{
			delete statements[i];
		}

		delete scope;

	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		std::vector<ASTStatement*> cloned_statements;
		for (size_t i = 0; i < statements.size(); i++)
		{
			cloned_statements.push_back((ASTStatement*)statements[i]->clone());
		}

		ASTStatementBlock* node =
			new ASTStatementBlock
			(
				cloned_statements
			);
		return node;
	}
};

struct ASTStatementExpression : ASTStatement
{
	ASTExpression* expr = nullptr;

	ASTStatementExpression(ASTExpression* expr) : ASTStatement(NodeKind::STMT_EXPR)
	{
		this->expr = expr;
		expr->parent = this;
	}

	~ASTStatementExpression()
	{
		delete expr;
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTStatementExpression* node =
			new ASTStatementExpression
			(
				(ASTExpression*)expr->clone()
			);
		return node;
	}
};
