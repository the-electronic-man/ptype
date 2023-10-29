#pragma once
#include "ast.h"

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
	
	SymbolKind symbol_kind = SymbolKind::None;

	Symbol(SymbolKind symbol_kind, ScopeKind scope_kind)
		: Scope(scope_kind)
	{
		this->symbol_kind = symbol_kind;
	}
};

struct SymbolVariable : Symbol
{
	ASTType* type = nullptr;

	SymbolVariable(ASTType* type)
		: Symbol(SymbolKind::Variable, ScopeKind::None)
	{ 
		this->type = type; 
	}
	~SymbolVariable() 
	{ 
		delete type; 
	}
};









