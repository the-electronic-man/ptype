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

bool Scope::AddSymbol(Symbol* symbol, std::string& name)
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

Symbol* Scope::GetSymbol(std::string& name)
{
	auto it = symbols.find(name);
	if (it != symbols.end())
	{
		return it->second;
	}
	return nullptr;
}