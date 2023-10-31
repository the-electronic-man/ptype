#pragma once
#include "ast.h"

struct Symbol;

struct Scope
{
	enum class ScopeKind
	{
		None,
		Block,
		Function,
		Namespace,
	};

	ScopeKind kind = ScopeKind::None;

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
		None,
		Namespace,
		Class,
		Function,
		Variable
	};
	
	Token name;
	SymbolKind symbol_kind = SymbolKind::None;

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

	SymbolVariable(Token name, ASTType* type)
		: Symbol(name, SymbolKind::Variable, ScopeKind::None)
	{
		this->type = type;
	}
	~SymbolVariable()
	{
		delete type;
	}
};

struct SymbolNamespace : Symbol
{
	SymbolNamespace(Token name)
		: Symbol(name, SymbolKind::Namespace, ScopeKind::Namespace) {}
	~SymbolNamespace() {}
};









