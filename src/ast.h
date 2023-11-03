#pragma once
#include "token.h"
#include "built_in_types.h"
#include "symbol.h"
#include "debug.h"

#include <vector>


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
					PrimitiveType* primitive_type;
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
					Token token;
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

			struct
			{
				TokenKind op;
				ast_node* expr;
			} unary;

			struct
			{
				TokenKind op;
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
				Token token;
			} literal;

			struct
			{
				TokenKind op;
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

		} decl;
	};
};


struct ASTNode
{
	NodeKind kind;
	ASTNode* parent = nullptr;
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
	ASTType(NodeKind kind) : ASTNode(kind) {}
	virtual ~ASTType() override {}
	virtual void accept(Visitor* visitor) override {}
	virtual ASTNode* clone() override { return nullptr; }
};

struct ASTTypePrimitive : ASTType
{
	PrimitiveType primitive_type;
	ASTTypePrimitive(PrimitiveType primitive_type)
		: ASTType(NodeKind::TYPE_PRIMITIVE)
	{
		this->primitive_type = primitive_type;
	}
	~ASTTypePrimitive() override {}
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override { return new ASTTypePrimitive(primitive_type); }
};

struct ASTTypeReference : ASTType
{
	ASTName* name = nullptr;
	ASTTypeReference(ASTName* name);
	~ASTTypeReference() override {}
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override;
};

struct ASTTypeArray : ASTType
{
	ASTType* subtype = nullptr;
	ASTTypeArray(ASTType* subtype)
		: ASTType(NodeKind::TYPE_ARRAY)
	{
		this->subtype = subtype;
		subtype->parent = this;
	}
	~ASTTypeArray() override {}
	void accept(Visitor* visitor) override { visitor->visit(this); }
	ASTNode* clone() override { return new ASTTypeArray((ASTType*)subtype->clone()); }
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
	void SetType(ASTType* type)
	{
		this->type = type;
		type->parent = this;
	}
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
