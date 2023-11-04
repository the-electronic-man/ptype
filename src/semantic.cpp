#include "semantic.h"

void SemanticAnalyzer::process(ASTNode* node)
{
	pass_type = PassType::Declare;
	node->accept(this);
	pass_type = PassType::Resolve;
	node->accept(this);

	ast_node an;
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
	node->get_expr()->accept(this);

	// the cast node type is already set
}

void SemanticAnalyzer::visit(ASTExpressionGroup* node)
{
	node->get_expr()->accept(this);
	node->type = (ASTType*)node->get_expr()->type->clone();
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
	node->get_expr()->accept(this);

	if (node->get_expr()->type->kind == NodeKind::TYPE_PRIMITIVE)
	{
		node->type =
			resolve_un_op_primitive_type
			(
				node->op.kind,
				((ASTTypePrimitive*)node->get_expr()->type)->primitive_type
			);
	}
	else
	{

	}
	node->type = (ASTType*)node->get_expr()->type->clone();
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	node->get_left()->accept(this);
	node->get_right()->accept(this);

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
}

void SemanticAnalyzer::visit(ASTExpressionAssign* node)
{
	if (pass_type == PassType::Resolve)
	{
		node->get_assignee()->accept(this);
		node->get_expr()->accept(this);
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
		SymbolVariable* symbol =
			new SymbolVariable(
				node->get_name()->token,
				(ASTType*)node->get_type()->clone());
		symbol->index = crt_scope->var_index;
		crt_scope->var_index++;
		crt_scope->AddSymbol(symbol, node->get_name()->token.buffer);
		node->get_name()->symbol = symbol;
	}
	else
	{
		node->get_expr()->accept(this);
		node->deduce_type_from_expr();
	}
}

void SemanticAnalyzer::visit(ASTStatementBlock* node)
{
	if (pass_type == PassType::Declare)
	{
		node->scope = new Scope(Scope::ScopeKind::BLOCK);
	}

	for (size_t i = 0; i < node->get_children_count(); i++)
	{
		node->get_stmt(i)->accept(this);
	}
}