#pragma once
#include "ast.h"

struct Symbol;

struct Scope
{
	enum class ScopeKind
	{
		NONE,
		BLOCK,
		FUNCTION,
		NAMESPACE,
	};

	ScopeKind kind = ScopeKind::NONE;

	int32_t var_index = 0;

	Scope* parent = nullptr;
	std::unordered_map<std::string, Symbol*> symbols;

	Scope(ScopeKind kind);
	~Scope();
	bool add_symbol(Symbol* symbol, std::string& name);
	Symbol* get_symbol(std::string& name);
};

struct Symbol : Scope
{
	enum class SymbolKind
	{
		NONE,
		NAMESPACE,
		CLASS,
		FUNCTION,
		VARIABLE
	};

	Token name;
	SymbolKind symbol_kind = SymbolKind::NONE;

	Symbol(Token name, SymbolKind symbol_kind, ScopeKind scope_kind)
		: Scope(scope_kind)
	{
		this->name = name;
		this->symbol_kind = symbol_kind;
	}
};

struct ASTType;

struct SymbolVariable : Symbol
{
	ASTType* type = nullptr;
	int32_t index = -1;

	enum VariableKind
	{
		GLOBAL,
		LOCAL,
		PARAMETER,
		MEMBER,
		STATIC
	};

	VariableKind var_kind;

	SymbolVariable(Token name, ASTType* type);
	~SymbolVariable();
};

enum FunctionTypeSpecifier
{
	FLAG_STATIC = 0x01,
	FLAG_CTOR = 0x02,
};

struct FunctionAttributes
{
	ASTType* type;
	bool is_ctor : 1;
	bool is_member : 1;
	bool is_static : 1;
	bool is_native : 1;
};


struct ASTStatementBlock;
struct ASTType;
struct SymbolFunction : Symbol
{
	Token name;
	std::vector<std::pair<ASTType*, ASTStatementBlock*>> signatures;

	SymbolFunction(Token name) : Symbol(name, SymbolKind::FUNCTION, ScopeKind::FUNCTION)
	{
		this->name = name;
	}
	~SymbolFunction()
	{
		for (size_t i = 0; i < signatures.size(); i++)
		{
			delete signatures[i].first;
			delete signatures[i].second;
		}
	}
	bool AddOverload(ASTType* type, ASTStatementBlock* block);
};

struct SymbolNamespace : Symbol
{
	SymbolNamespace(Token name)
		: Symbol(name, SymbolKind::NAMESPACE, ScopeKind::NAMESPACE) {}
	~SymbolNamespace() {}
};









