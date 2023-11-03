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
		case PassType::Declare:
		{

			break;
		}
		case PassType::Resolve:
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
		case PassType::Declare:
		{

			break;
		}
		case PassType::Resolve:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTTypePrimitive* node)
{
	switch (pass_type)
	{
		case PassType::Declare:
		{

			break;
		}
		case PassType::Resolve:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTTypeReference* node)
{

}

void SemanticAnalyzer::visit(ASTTypeArray* node)
{

}

void SemanticAnalyzer::visit(ASTExpressionCast* node)
{
	if (pass_type == PassType::Resolve)
	{
		node->expr->accept(this);
	}
}

void SemanticAnalyzer::visit(ASTExpressionGroup* node)
{
	if (pass_type == PassType::Resolve)
	{
		node->expr->accept(this);
	}
}

void SemanticAnalyzer::visit(ASTExpressionLiteral* node)
{
	if (pass_type == PassType::Resolve)
	{
		switch (node->token.kind)
		{
			case TokenKind::LITERAL_INT:
			{
				node->type = new ASTTypePrimitive(PrimitiveType::T_INT);
				break;
			}
			case TokenKind::LITERAL_FLOAT:
			{
				node->type = new ASTTypePrimitive(PrimitiveType::T_FLOAT);
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
	if (pass_type == PassType::Resolve)
	{
		node->expr->accept(this);
		node->type = (ASTType*)node->expr->type->clone();
	}
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);

	switch (node->op.kind)
	{
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		case TokenKind::STAR:
		case TokenKind::SLASH:
		case TokenKind::PERCENT:
		{

			break;
		}
		default:
		{
			break;
		}
	}

	node->type = (ASTType*)node->left->type->clone();
}

void SemanticAnalyzer::visit(ASTExpressionAssign* node)
{
	if (pass_type == PassType::Resolve)
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
	if (pass_type == PassType::Declare)
	{
		SymbolVariable* var_symbol =
			new SymbolVariable(
				node->name,
				(ASTType*)node->type->clone());
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
	if (pass_type == PassType::Declare)
	{
		node->scope = new Scope(Scope::ScopeKind::BLOCK);
	}

	for (size_t i = 0; i < node->statements.size(); i++)
	{
		node->statements[i]->accept(this);
	}
}