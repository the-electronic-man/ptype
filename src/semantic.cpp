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

void SemanticAnalyzer::visit(ASTTypePrimitive* node)
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

void SemanticAnalyzer::visit(ASTExpressionGroup* node)
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

void SemanticAnalyzer::visit(ASTExpressionLiteral* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Resolve:
		{
			switch (node->token.kind)
			{
				case TokenKind::LITERAL_INT:
				{
					node->type = new ASTTypePrimitive(PrimitiveType::T_INT);
					break;
				}
				default:
				{
					break;
				}
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTExpressionUnary* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{
			node->expr->accept(this);
			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{
			node->left->accept(this);
			node->right->accept(this);
			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTExpressionAssign* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{
			node->assignee->accept(this);
			node->expr->accept(this);
			break;
		}
		default:
		{
			break;
		}
	}
}

void SemanticAnalyzer::visit(ASTDeclarationVariable* node)
{
	switch (pass_type)
	{
		case SemanticAnalyzer::PassType::Declare:
		{

			break;
		}
		case SemanticAnalyzer::PassType::Resolve:
		{
			node->expr->accept(this);
			break;
		}
		default:
		{
			break;
		}
	}
}
