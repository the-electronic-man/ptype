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

}

void SemanticAnalyzer::visit(ASTTypeReference* node)
{

}

void SemanticAnalyzer::visit(ASTTypeArray* node)
{

}

void SemanticAnalyzer::visit(ASTExpressionCast* node)
{
	// resolve the expression that is being cast
	node->expr->accept(this);

	// the cast node type is already set
}

void SemanticAnalyzer::visit(ASTExpressionGroup* node)
{
	node->expr->accept(this);
	node->type = (ASTType*)node->expr->type->clone();
}

void SemanticAnalyzer::visit(ASTExpressionLiteral* node)
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

ASTType* SemanticAnalyzer::resolve_un_op_primitive_type(TokenKind op, PrimitiveType primitive_type)
{
	switch (op)
	{
		// char, int, float
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		{
			switch (primitive_type)
			{
				case PrimitiveType::T_CHAR:
				case PrimitiveType::T_INT:
				case PrimitiveType::T_FLOAT:
				{
					return new ASTTypePrimitive(primitive_type);
				}
				default:
				{
					pt_error
					(
						"cannot use %s operator on %s",
						token_kind_to_string(op),
						primitive_type_to_string(primitive_type)
					);
				}
			}
			break;
		}
		// bool
		case TokenKind::KW_NOT:
		{
			if (primitive_type == PrimitiveType::T_BOOL)
			{
				return new ASTTypePrimitive(primitive_type);
			}

			pt_error
			(
				"cannot use %s operator on %s",
				token_kind_to_string(op),
				primitive_type_to_string(primitive_type)
			);
			break;
		}
		// char, int
		case TokenKind::TILDE:
		{
			if (primitive_type == PrimitiveType::T_INT)
			{
				return new ASTTypePrimitive(primitive_type);
			}

			pt_error
			(
				"cannot use %s operator on %s",
				token_kind_to_string(op),
				primitive_type_to_string(primitive_type)
			);
			break;
		}
		default: pt_unreachable();
	}
}

void SemanticAnalyzer::visit(ASTExpressionUnary* node)
{
	node->expr->accept(this);

	if (node->expr->type->kind == NodeKind::TYPE_PRIMITIVE)
	{
		node->type =
			resolve_un_op_primitive_type
			(
				node->op.kind,
				((ASTTypePrimitive*)node->expr->type)->primitive_type
			);
	}
	else
	{

	}
	node->type = (ASTType*)node->expr->type->clone();
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