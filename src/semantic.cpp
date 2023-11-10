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

bool SemanticAnalyzer::is_un_op_arith(TokenKind op)
{
	return
		op == TokenKind::PLUS ||
		op == TokenKind::MINUS;
}

bool SemanticAnalyzer::is_un_op_logic(TokenKind op)
{
	return op == TokenKind::KW_NOT;
}

bool SemanticAnalyzer::is_un_op_bitwise(TokenKind op)
{
	return op == TokenKind::TILDE;
}

bool SemanticAnalyzer::is_numeric(PrimitiveType primitive_type)
{
	return
		is_integral(primitive_type) ||
		is_decimal(primitive_type);
}

bool SemanticAnalyzer::is_integral(PrimitiveType primitive_type)
{
	return
		primitive_type == PrimitiveType::T_INT ||
		primitive_type == PrimitiveType::T_CHAR;
}

bool SemanticAnalyzer::is_decimal(PrimitiveType primitive_type)
{
	return
		primitive_type == PrimitiveType::T_FLOAT;
}

bool SemanticAnalyzer::is_logic(PrimitiveType primitive_type)
{
	return
		primitive_type == PrimitiveType::T_BOOL;
}

bool SemanticAnalyzer::is_void(PrimitiveType primitive_type)
{
	return
		primitive_type == PrimitiveType::T_VOID;
}


void SemanticAnalyzer::visit(ASTExpressionUnary* node)
{
	node->expr->accept(this);

	if (node->expr->type->kind == NodeKind::TYPE_PRIMITIVE)
	{
		PrimitiveType primitive_type =
			((ASTTypePrimitive*)node->expr->type)->primitive_type;

		switch (node->op.kind)
		{
			case TokenKind::PLUS:
			case TokenKind::MINUS:
			{
				if (is_numeric(primitive_type))
				{
					node->type = new ASTTypePrimitive(primitive_type);
					return;
				}
				break;
			}
			case TokenKind::KW_NOT:
			{
				if (is_logic(primitive_type))
				{
					node->type = new ASTTypePrimitive(PrimitiveType::T_BOOL);
					return;
				}
				break;
			}
			case TokenKind::TILDE:
			{
				if (is_integral(primitive_type))
				{
					node->type = new ASTTypePrimitive(primitive_type);
					return;
				}
				break;
			}
			default:
			{
				pt_unreachable();
			}
		}
	}

	// if the expr child of the unary node is not a
	// primitive type then we have to replace the whole
	// unary node with a function call to the corresponding
	// overloaded operator (if it exists)

	// TODO : replace with a function call
	// node->parent->replace_child(node, nullptr);

	pt_unreachable();
}

PrimitiveType SemanticAnalyzer::get_common_numeric_type(PrimitiveType src, PrimitiveType dst)
{
	return (PrimitiveType)std::max((int)src, int(dst));
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);

	// node->type = new ASTTypePrimitive(PrimitiveType::T_INT);

	if (node->left->type->kind == NodeKind::TYPE_PRIMITIVE &&
		node->right->type->kind == NodeKind::TYPE_PRIMITIVE)
	{
		PrimitiveType left = ((ASTTypePrimitive*)node->left->type)->primitive_type;
		PrimitiveType right = ((ASTTypePrimitive*)node->right->type)->primitive_type;

		// handle arithmetic operators here
		switch (node->op.kind)
		{
			case TokenKind::PLUS:
			case TokenKind::MINUS:
			case TokenKind::STAR:
			case TokenKind::SLASH:
			case TokenKind::PERCENT:
			{
				if (is_numeric(left) && is_numeric(right))
				{
					ASTType* result_type = new ASTTypePrimitive(get_common_numeric_type(left, right));
					if (!node->left->type->is_equal(result_type))
					{
						node->left = new ASTExpressionCast(node->left, (ASTType*)result_type->clone());
					}
					if (!node->right->type->is_equal(result_type))
					{
						ASTType* dst_type = (ASTType*)result_type->clone();
						node->right = new ASTExpressionCast(node->right, dst_type);
					}
					node->type = result_type;
					return;
				}
				break;
			}
			case TokenKind::KW_AND:
			case TokenKind::KW_OR:
			{
				if (is_logic(left) && is_logic(right))
				{
					node->type = new ASTTypePrimitive(PrimitiveType::T_BOOL);
					return;
				}
				break;
			}
			case TokenKind::AMPERSAND:
			case TokenKind::CARET:
			case TokenKind::VERTICAL_BAR:
			{
				if (is_integral(left) && is_integral(right))
				{
					node->type = new ASTTypePrimitive(get_common_numeric_type(left, right));
					return;
				}
				break;
			}
			default:
			{
				pt_unreachable();
			}
		}
	}

	// handle operator overloading here
	pt_unreachable();

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
		SymbolVariable* symbol =
			new SymbolVariable
			(
				node->name,
				nullptr
			);
		symbol->index = crt_scope->var_index;
		crt_scope->var_index++;
		crt_scope->AddSymbol(symbol, node->name.buffer);
		node->symbol = symbol;
	}
	else
	{
		if (node->expr)
		{
			node->expr->accept(this);

			if (!node->type)
			{
				((SymbolVariable*)node->symbol)->type =
					(ASTType*)node->expr->type->clone();
			}
		}
		//((SymbolVariable*)node->get_name()->symbol)->type = (ASTType*)node->expr->type->clone();

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