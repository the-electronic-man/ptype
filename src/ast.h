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
	virtual void visit(ASTExpressionFieldGet* node) {}
	virtual void visit(ASTExpressionFieldSet* node) {}
	virtual void visit(ASTExpressionArrayGet* node) {}
	virtual void visit(ASTExpressionArraySet* node) {}

	virtual void visit(ASTStatementExpression* node) {}
	virtual void visit(ASTStatementBlock* node) {}

	virtual void visit(ASTDeclarationVariable* node) {}
};
struct Symbol;
struct ast_node
{
	NodeKind kind;
	union
	{
		struct
		{
			/*
				prim	: primitive_type -> primitive_type
				ref		: name -> item
				array	: subtype -> item
			*/

			union
			{
				struct
				{
					PrimitiveType primitive_type;
				} primitive;

				struct
				{
					ast_node* name;
				} ref;

				struct
				{
					ast_node* subtype;
				} arr;
			};
		} type;

		struct
		{
			/*
				simple	: name -> name
				qlfd	: qualifier -> qualifier, simple_name -> simple-name
			*/

			union
			{
				struct
				{
					token_index_t token;
				} simple;
				struct
				{
					ast_node* simple_name;
					ast_node* qualifier;
				} qualified;
			};
		} name;

		struct
		{
			/*
				[x] unary	: type -> type, token -> op, left -> expr
				[x] binary	: type -> type, token -> op, left -> left, right -> right
				[x] assign	: type -> type, token -> op, left -> assignee, right -> value
				[x] name	: type -> type, token -> name, symbol -> symbol
				[x] get		: type -> type, left -> obj / arr, right -> index
				[x] set		: type -> type, left -> obj / arr, right -> index, value -> value
				[x] cast	: type -> type, left -> expr
				[x] group	: type -> type, left -> expr
				[x] literal : type -> type, token -> token
			*/

			// all expression types share this
			ast_node* type;

			union
			{
				struct
				{
					token_index_t op;
					ast_node* expr;
				} unary;

				struct
				{
					token_index_t op;
					ast_node* left;
					ast_node* right;
				} binary;

				struct
				{
					ast_node* expr;
				} cast;

				struct
				{
					ast_node* expr;
				} group;

				struct
				{
					ast_node* name;
					Symbol* symbol;
				} name;

				struct
				{
					token_index_t token;
				} literal;

				struct
				{
					token_index_t op;
					ast_node* assignee;
					ast_node* value;
				} assign;

				struct
				{
					ast_node* arr;
					ast_node* index;
				} arr_get;

				struct
				{
					ast_node* arr;
					ast_node* index;
					ast_node* value;
				} arr_set;

				struct
				{
					ast_node* obj;
					ast_node* field;
				} field_get;

				struct
				{
					ast_node* obj;
					ast_node* value;
					ast_node* field;
				} field_set;
			};
		} expr;

		struct
		{
			/*
				expr	: next -> next, expr -> expr
				cond	: next -> next, then -> stmt, else -> else_stmt
				block	: next -> next, crt -> stmt
			*/

			// all statement nodes have a next node
			ast_node* next;

			union
			{
				struct
				{
					ast_node* expr;
				} expr;

				struct
				{
					ast_node* then_stmt;
					ast_node* else_stmt;
				} if_else;

				struct
				{
					ast_node* stmt;
				} block;
			};

		} stmt;

		struct
		{
			token_index_t name;
			struct
			{
				Symbol* symbol;
				ast_node* type;
				ast_node* expr;
			} var;
		} decl;
	};

	static ast_node* make_expr_literal(token_index_t token_index);
	static ast_node* make_expr_unary(token_index_t op, ast_node* expr);
	static ast_node* make_expr_binary(token_index_t op, ast_node* left, ast_node* right);
	static ast_node* make_expr_name(ast_node* name);
	static ast_node* make_expr_literal(ast_node* dst_type, ast_node* expr);
	static ast_node* make_expr_group(ast_node* expr);
	static ast_node* make_expr_assign(token_index_t op, ast_node* assignee, ast_node* expr);
	static ast_node* make_expr_field_get(ast_node* expr, ast_node* field);
	static ast_node* make_expr_array_get(ast_node* expr, ast_node* field);
	static ast_node* make_expr_field_set(ast_node* expr, ast_node* field, ast_node* index);
	static ast_node* make_expr_array_set(ast_node* expr, ast_node* field, ast_node* index);

};

struct ASTNode
{
protected:

	ASTNode(NodeKind kind) { this->kind = kind; }

public:

	NodeKind kind;
	ASTNode* parent = nullptr;
	std::vector<ASTNode*> children;

	void AttachChild(ASTNode* child)
	{
		assert(child);

		// should we also check if it's duplicate?
		assert(FindChildIndex(child) == children.end());

		children.push_back(child);
		child->parent = this;
	}

	void AttachNullChild()
	{
		children.push_back(nullptr);
	}

	void DetachChild(ASTNode* child)
	{
		auto it = FindChildIndex(child);
		if (it != children.end())
		{
			children.erase(it);
			return;
		}
		assert(false);
	}

	std::vector<ASTNode*>::iterator FindChildIndex(ASTNode* child)
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
			node->AttachChild(children[i]->clone());
		}
		return node;
	}

	size_t get_children_count()
	{
		return children.size();
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
		AttachChild((ASTNode*)name);
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
		AttachChild((ASTNode*)name);
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
		AttachChild(subtype);
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
		AttachChild(qualifier);
		AttachChild(name);
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
		AttachChild(expr);
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
		AttachChild(left);
		AttachChild(right);
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
		AttachChild(expr);
		AttachChild(field);
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
		AttachChild(expr);
		AttachChild(field);
		AttachChild(value);
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
		AttachChild(expr);
		AttachChild(index);
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
		AttachChild(expr);
		AttachChild(index);
		AttachChild(value);
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
		AttachChild(expr);
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
		AttachChild(expr);

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
		AttachChild(assignee);
		AttachChild(expr);
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
		AttachChild(name);
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
		if (type)
		{
			AttachChild(type);
		}
		else
		{
			AttachNullChild();
		}
		AttachChild(expr);
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
			AttachChild(statements[i]);
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
		AttachChild(expr);
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
