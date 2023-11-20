#include "symbol.h"

Scope::Scope(ScopeKind kind)
{
	this->kind = kind;
}

Scope::~Scope()
{
	for (auto& it : symbols)
	{
		delete it.second;
	}
}

bool Scope::add_symbol(Symbol* symbol, std::string& name)
{
	if (symbol == nullptr)
	{
		return false;
	}

	auto it = symbols.find(name);
	if (it == symbols.end())
	{
		symbols[name] = symbol;
		return true;
	}
	return false;
}

Symbol* Scope::get_symbol(std::string& name)
{
	auto it = symbols.find(name);
	if (it != symbols.end())
	{
		return it->second;
	}
	return nullptr;
}

SymbolVariable::SymbolVariable(Token name, ASTType* type)
	: Symbol(name, SymbolKind::VARIABLE, ScopeKind::NONE)
{
	this->type = type;
}
SymbolVariable::~SymbolVariable()
{
	delete type;
}

bool SymbolFunction::AddOverload(ASTType* type, ASTStatementBlock* block)
{
	for (size_t i = 0; i < signatures.size(); i++)
	{
		if (signatures[i].first->is_equal(type))
		{
			pt_error("overload already exists %s", type->to_string().data());
			return false;
		}
	}
	signatures.push_back(std::pair(type, block));
	return true;
}