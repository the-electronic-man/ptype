#include "semantic.h"

void SemanticAnalyzer::process(ASTNode* node)
{
	pass_type = PassType::Declare;

	pass_type = PassType::Resolve;

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

			break;
		}
		default:
		{
			break;
		}
	}
}
