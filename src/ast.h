#pragma once
#include "token.h"
#include "built_in.h"
#include "symbol.h"
#include "debug.h"

#include <vector>
#include <algorithm>

#ifdef ENUM_ITEM
#undef ENUM_ITEM
#endif
#define ENUM_ITEM(name, str) name,
enum class NodeKind
{
#include "ast.enum"
};
const char* node_kind_to_string(NodeKind kind);



struct ASTNode;

struct ASTName;
struct ASTNameGlobal;
struct ASTNameSimple;
struct ASTNameQualified;

struct ASTType;
struct ASTTypePrimitive;
struct ASTTypeArray;
struct ASTTypeReference;
struct ASTTypeFunction;
struct ASTTypeParameterized;

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
struct ASTStatementReturn;
struct ASTStatementBreak;
struct ASTStatementContinue;
struct ASTStatementBlock;

struct ASTDeclarationVariable;
struct ASTDeclarationFunction;
struct ASTDeclarationClass;

struct Visitor
{
	virtual void process(ASTNode* node) {}

	virtual void visit(ASTNameGlobal* node) {}
	virtual void visit(ASTNameSimple* node) {}
	virtual void visit(ASTNameQualified* node) {}

	virtual void visit(ASTTypePrimitive* node) {}
	virtual void visit(ASTTypeArray* node) {}
	virtual void visit(ASTTypeReference* node) {}
	virtual void visit(ASTTypeFunction* node) {}
	virtual void visit(ASTTypeParameterized* node) {}

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
	virtual void visit(ASTStatementReturn* node) {}
	virtual void visit(ASTStatementBreak* node) {}
	virtual void visit(ASTStatementContinue* node) {}
	virtual void visit(ASTStatementBlock* node) {}

	virtual void visit(ASTDeclarationVariable* node) {}
	virtual void visit(ASTDeclarationFunction* node) {}
	virtual void visit(ASTDeclarationClass* node) {}
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

struct ASTDeclaration : ASTNode
{
	Token name;
	Symbol* symbol = nullptr;

	ASTDeclaration(Token name, NodeKind kind) : ASTNode(kind)
	{
		this->name = name;
	}

	ASTDeclaration(NodeKind kind) : ASTNode(kind) {}
};

struct ASTStatement : ASTDeclaration
{
	ASTStatement(NodeKind kind) : ASTDeclaration(kind) {}
};


struct ASTName : ASTNode
{
	Symbol* symbol = nullptr;
	ASTName(NodeKind kind) : ASTNode(kind) {}
	virtual std::string to_string() = 0;
	virtual bool is_equal(ASTName* other) { return kind == other->kind; }
};

struct ASTParameterList
{
	std::vector<ASTType*> parameter_types;
};

struct ASTType : ASTNode
{
	ASTType(NodeKind kind) : ASTNode(kind) {}
	virtual std::string to_string() = 0;
	virtual bool is_equal(ASTType* other) { return kind == other->kind; }

	static bool is_void(ASTType* type);
	static bool is_integer(ASTType* type);
	static bool is_decimal(ASTType* type);
};

struct ASTTypePrimitive : ASTType
{
	BuiltIn built_in;

	ASTTypePrimitive(BuiltIn built_in) : ASTType(NodeKind::TYPE_PRIM)
	{
		this->built_in = built_in;
	}

	std::string to_string() override
	{
		return built_in_to_string(built_in);
	}

	void accept(Visitor* visitor)
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		ASTTypePrimitive* node =
			new ASTTypePrimitive(built_in);
		return node;
	}

	bool is_equal(ASTType* other) override
	{
		return
			ASTType::is_equal(other) &&
			built_in == ((ASTTypePrimitive*)other)->built_in;
	}
};

struct ASTTypeArray : ASTType
{
	ASTType* subtype = nullptr;

	ASTTypeArray(ASTType* subtype) : ASTType(NodeKind::TYPE_ARR)
	{
		this->subtype = subtype;
		subtype->parent = this;
	}

	~ASTTypeArray()
	{
		delete subtype;
	}

	std::string to_string() override
	{
		return subtype->to_string() + "[]";
	}

	void accept(Visitor* visitor)
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		ASTTypeArray* node =
			new ASTTypeArray((ASTType*)subtype->clone());
		return node;
	}

	bool is_equal(ASTType* other) override
	{
		return
			ASTType::is_equal(other) &&
			subtype->is_equal(((ASTTypeArray*)other)->subtype);
	}
};

struct ASTTypeReference : ASTType
{
	ASTName* name = nullptr;

	ASTTypeReference(ASTName* name) : ASTType(NodeKind::TYPE_REF)
	{
		assert(name);
		this->name = name;
		name->parent = this;
	}

	std::string to_string() override
	{
		return name->to_string();
	}

	void accept(Visitor* visitor)
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		ASTTypeReference* node =
			new ASTTypeReference((ASTName*)name->clone());
		return node;
	}

	bool is_equal(ASTType* other) override
	{
		return
			ASTType::is_equal(other) &&
			name->is_equal(((ASTTypeReference*)other)->name);
	}
};

struct ASTTypeFunction : ASTType
{
	std::vector<ASTType*> signature;

	ASTTypeFunction(std::vector<ASTType*>& signature) : ASTType(NodeKind::TYPE_FUN)
	{
		pt_assert(signature.size() >= 1);
		this->signature = signature;
		for (size_t i = 0; i < signature.size(); i++)
		{
			signature[i]->parent = this;
		}
	}

	~ASTTypeFunction()
	{
		for (size_t i = 0; i < signature.size(); i++)
		{
			delete signature[i];
		}

	}

	std::string to_string() override
	{
		std::string s = "({";
		for (size_t i = 0; i < signature.size() - 1; i++)
		{
			if (i != 0) { s += ", "; }
			s += signature[i]->to_string();
		}
		s += "} -> " + signature.back()->to_string() + ")";
		return s;
	}

	void accept(Visitor* visitor)
	{
		visitor->visit(this);
	}

	ASTNode* clone() override
	{
		std::vector<ASTType*> signature;

		for (size_t i = 0; i < this->signature.size(); i++)
		{
			signature.push_back((ASTType*)this->signature[i]->clone());
		}

		ASTTypeFunction* node =
			new ASTTypeFunction(signature);

		return node;
	}

	bool is_equal(ASTType* other) override
	{
		if (!ASTType::is_equal(other))
		{
			return false;
		}

		ASTTypeFunction* other_func = (ASTTypeFunction*)other;

		if (signature.size() != other_func->signature.size())
		{
			return false;
		}

		for (size_t i = 0; i < signature.size(); i++)
		{
			if (!signature[i]->is_equal(other_func->signature[i]))
			{
				return false;
			}
		}

		return true;
	}
};

struct ASTTypeParameterized : ASTType
{
	ASTName* name = nullptr;
	std::vector<ASTType*> parameters;
	ASTTypeParameterized(ASTName* name, std::vector<ASTType*>& parameters)
		: ASTType(NodeKind::TYPE_PARAM)
	{
		assert(name);
		this->name = name;
		name->parent = this;

		this->parameters = parameters;
		for (size_t i = 0; i < parameters.size(); i++)
		{
			assert(parameters[i]);
			parameters[i]->parent = this;
		}
	}

	~ASTTypeParameterized()
	{
		delete name;
		for (size_t i = 0; i < parameters.size(); i++)
		{
			delete parameters[i];
		}

	}

	std::string to_string() override
	{
		std::string s = name->to_string();
		s += "<";
		for (size_t i = 0; i < parameters.size(); i++)
		{
			if (i != 0) { s += ", "; }
			s += parameters[i]->to_string();
		}
		s += ">";
		return s;
	}

	void accept(Visitor* visitor)
	{
		visitor->visit(this);
	}

	bool is_equal(ASTType* other) override
	{
		if (!ASTType::is_equal(other))
		{
			return false;
		}

		ASTTypeParameterized* other_parameterized = (ASTTypeParameterized*)other;

		if (parameters.size() != other_parameterized->parameters.size())
		{
			return false;
		}

		if (!name->is_equal(other_parameterized->name))
		{
			return false;
		}

		for (size_t i = 0; i < parameters.size(); i++)
		{
			if (!parameters[i]->is_equal(other_parameterized->parameters[i]))
			{
				return false;
			}
		}

		return true;
	}

	ASTNode* clone() override
	{
		std::vector<ASTType*> parameters;
		for (size_t i = 0; i < this->parameters.size(); i++)
		{
			parameters.push_back((ASTType*)this->parameters[i]->clone());
		}

		ASTTypeParameterized* node =
			new ASTTypeParameterized
			(
				(ASTName*)name->clone(),
				parameters
			);
		return node;
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
		ASTNameGlobal* node = new ASTNameGlobal();
		return node;
	}

	std::string to_string() override
	{
		return "global";
	}

	bool is_equal(ASTName* other) override
	{
		return
			ASTName::is_equal(other);
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

	std::string to_string() override
	{
		return token.buffer;
	}

	bool is_equal(ASTName* other) override
	{
		return
			ASTName::is_equal(other) &&
			token.buffer == ((ASTNameSimple*)other)->token.buffer;
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
		delete qualifier;
		delete name;
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

	std::string to_string() override
	{
		return qualifier->to_string() + "::" + name->to_string();
	}

	bool is_equal(ASTName* other) override
	{
		return
			ASTName::is_equal(other) &&
			name->is_equal(((ASTNameQualified*)other)->name) &&
			qualifier->is_equal(((ASTNameQualified*)other)->qualifier);
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


struct ASTExpressionFieldSet : ASTExpression
{
	ASTExpressionName* expr = nullptr;
	ASTNameSimple* field = nullptr;
	ASTExpression* value = nullptr;

	ASTExpressionFieldSet(ASTExpressionName* expr, ASTNameSimple* field, ASTExpression* value)
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
				(ASTExpressionName*)expr->clone(),
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
	ASTExpressionName* expr = nullptr;
	ASTExpression* index = nullptr;
	ASTExpression* value = nullptr;

	ASTExpressionArraySet(ASTExpressionName* expr, ASTExpression* index, ASTExpression* value)
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
				(ASTExpressionName*)expr->clone(),
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
	ASTExpressionName* assignee = nullptr;
	ASTExpression* expr = nullptr;

	ASTExpressionAssign(ASTExpressionName* assignee, ASTExpression* expr)
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
				(ASTExpressionName*)assignee->clone(),
				(ASTExpression*)expr->clone()
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

		if (type)
		{
			this->type = type;
			type->parent = this;
		}

		if (expr)
		{
			this->expr = expr;
			expr->parent = this;
		}
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
	std::vector<ASTDeclaration*> declarations;

	ASTStatementBlock(std::vector<ASTDeclaration*>& declarations)
		: ASTStatement(NodeKind::STMT_BLOCK)
	{
		this->declarations = declarations;
		for (size_t i = 0; i < declarations.size(); i++)
		{
			this->declarations[i]->parent = this;
		}
	}

	~ASTStatementBlock()
	{
		for (size_t i = 0; i < declarations.size(); i++)
		{
			delete declarations[i];
		}

		delete scope;

	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		std::vector<ASTDeclaration*> cloned_declarations;
		for (size_t i = 0; i < declarations.size(); i++)
		{
			cloned_declarations.push_back((ASTStatement*)declarations[i]->clone());
		}

		ASTStatementBlock* node =
			new ASTStatementBlock
			(
				cloned_declarations
			);
		return node;
	}
};


struct ASTDeclarationFunction : ASTDeclaration
{
	Token name;
	ASTTypeFunction* signature = nullptr;
	std::vector<Token> parameter_name;
	ASTStatementBlock* block = nullptr;

	ASTDeclarationFunction
	(
		Token name,
		ASTTypeFunction* signature,
		std::vector<Token>& parameter_name,
		ASTStatementBlock* block
	)
		: ASTDeclaration(name, NodeKind::DECL_FUN)
	{
		this->name = name;

		pt_assert(signature);

		this->signature = signature;
		signature->parent = this;

		this->parameter_name = parameter_name;

		if (block)
		{
			this->block = block;
			block->parent = this;
		}

	}

	~ASTDeclarationFunction()
	{
		delete signature;

		if (block)
		{
			delete block;
		}
	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTDeclarationFunction* node =
			new ASTDeclarationFunction
			(
				name,
				(ASTTypeFunction*)signature->clone(),
				parameter_name,
				(ASTStatementBlock*)block->clone()
			);
		return node;
	}
};


struct ASTDeclarationClass : ASTDeclaration
{
	ASTDeclarationClass
	(
		Token name,
		std::vector<ASTDeclaration*>& declarations
	)
		: ASTDeclaration(name, NodeKind::DECL_CLASS)
	{

	}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		return nullptr;
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

struct ASTStatementReturn : ASTStatement
{
	ASTExpression* expr = nullptr;

	ASTStatementReturn(ASTExpression* expr) : ASTStatement(NodeKind::STMT_RETURN)
	{
		if (expr)
		{
			this->expr = expr;
			expr->parent = this;
		}
	}

	~ASTStatementReturn()
	{
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
		ASTStatementReturn* node =
			new ASTStatementReturn
			(
				(ASTExpression*)expr->clone()
			);
		return node;
	}
};


struct ASTStatementContinue : ASTStatement
{
	ASTStatementContinue() : ASTStatement(NodeKind::STMT_CONTINUE) {}

	~ASTStatementContinue() {}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTStatementContinue* node =
			new ASTStatementContinue();
		return node;
	}
};

struct ASTStatementBreak : ASTStatement
{
	ASTStatementBreak() : ASTStatement(NodeKind::STMT_BREAK) {}

	~ASTStatementBreak() {}

	void accept(Visitor* visitor) override
	{
		visitor->visit(this);
	}

	ASTNode* clone()
	{
		ASTStatementBreak* node =
			new ASTStatementBreak();
		return node;
	}
};
