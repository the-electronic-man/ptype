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
	bool AddSymbol(Symbol* symbol, std::string& name);
	Symbol* GetSymbol(std::string& name);
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

	SymbolVariable(Token name, ASTType *type);
	~SymbolVariable();
};

struct SymbolNamespace : Symbol
{
	SymbolNamespace(Token name)
		: Symbol(name, SymbolKind::NAMESPACE, ScopeKind::NAMESPACE) {}
	~SymbolNamespace() {}
};









