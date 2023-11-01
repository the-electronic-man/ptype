#include "semantic.h"

void SemanticAnalyzer::process(ASTNode* node)
{
	pass_type = PassType::Declare;
	node->accept(this);
	pass_type = PassType::Resolve;
	node->accept(this);
}

void SemanticAnalyzer::visit(ASTNameSimple* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTNameQualified* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTType* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTExpressionCast* node)
{
	
}

void SemanticAnalyzer::visit(ASTExpressionGroup* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Resolve)
	{
		node->expr->accept(this);
	}
}

void SemanticAnalyzer::visit(ASTExpressionLiteral* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Resolve)
	{
		switch (node->token.kind)
		{
			case TokenKind::LITERAL_INT:
			{
				node->type = new ASTType(PrimitiveType::T_INT);
				break;
			}
			case TokenKind::LITERAL_FLOAT:
			{
				node->type = new ASTType(PrimitiveType::T_FLOAT);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void SemanticAnalyzer::visit(ASTExpressionUnary* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Resolve)
	{
		node->expr->accept(this);
		node->type = (ASTType*)node->expr->type->clone();
	}
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Resolve)
	{
		node->left->accept(this);
		node->right->accept(this);
		// TODO : just a temporary hack
		node->type = (ASTType*)node->left->type->clone();
	}
}

void SemanticAnalyzer::visit(ASTExpressionAssign* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Resolve)
	{
		node->assignee->accept(this);
		node->expr->accept(this);
		// insert a cast node if needed

		// the node's type will always be the assignee's type
		// node->type = (ASTType*)node->assignee->type->clone();
	}
}

void SemanticAnalyzer::visit(ASTExpressionName* node)
{
	
}

void SemanticAnalyzer::visit(ASTDeclarationVariable* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Declare)
	{
		SymbolVariable* var_symbol = 
			new SymbolVariable
			(
				node->name, 
				(ASTType*)node->type->clone()
			);
		var_symbol->index = crt_scope->var_index;
		crt_scope->var_index++;
		crt_scope->AddSymbol(var_symbol, node->name.buffer);
		node->var_symbol = var_symbol;
	}
	else
	{
		node->expr->accept(this);
		node->type = (ASTType*)node->expr->type->clone();
	}
}

void SemanticAnalyzer::visit(ASTStatementBlock* node)
{
	if (pass_type == SemanticAnalyzer::PassType::Declare)
	{
		node->scope = new Scope(Scope::ScopeKind::BLOCK);
	}

	for (size_t i = 0; i < node->statements.size(); i++)
	{
		node->statements[i]->accept(this);
	}

}