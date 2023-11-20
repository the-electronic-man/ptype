#include "semantic.h"

void SemanticAnalyzer::push_scope(Scope* scope)
{
	pt_assert(scope);
	scope->parent = crt_scope;
	crt_scope = scope;
}

void SemanticAnalyzer::pop_scope()
{
	pt_assert(crt_scope->parent);
	crt_scope = crt_scope->parent;
}

void SemanticAnalyzer::process(ASTNode* node)
{
	pass_type = PassType::Declare;
	node->accept(this);
	pass_type = PassType::Resolve;
	node->accept(this);
}

void SemanticAnalyzer::visit(ASTNameSimple* node)
{
	Symbol* symbol = crt_scope->get_symbol(node->token.buffer);
	if (!symbol)
	{
		pt_error("symbol %s not found in this scope", node->token.buffer);
	}
	node->symbol = symbol;
}

void SemanticAnalyzer::visit(ASTNameQualified* node)
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
		case TokenKind::LIT_I4:
		{
			node->type = new ASTTypePrimitive(BuiltIn::T_I4);
			break;
		}
		case TokenKind::LIT_F4:
		{
			node->type = new ASTTypePrimitive(BuiltIn::T_F4);
			break;
		}
		// case TokenKind::LIT_F8:
		// {
		// 	node->type = new ASTType(BuiltIn::T_F8);
		// 	break;
		// }
		case TokenKind::LIT_CHAR:
		{
			node->type = new ASTTypePrimitive(BuiltIn::T_CHAR);
			break;
		}
		case TokenKind::LIT_TRUE:
		case TokenKind::LIT_FALSE:
		{
			node->type = new ASTTypePrimitive(BuiltIn::T_BOOL);
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
	node->expr->accept(this);

	if (node->expr->type->kind == NodeKind::TYPE_PRIM)
	{
		BuiltIn built_in = ((ASTTypePrimitive*)node->expr->type)->built_in;

		switch (node->op.kind)
		{
			case TokenKind::PLUS:
			case TokenKind::MINUS:
			{
				if (!is_numeric(built_in))
				{
					break;
				}

				node->type = new ASTTypePrimitive(built_in);
				return;
			}
			case TokenKind::KW_NOT:
			{
				if (!is_logic(built_in))
				{
					break;
				}

				node->type = new ASTTypePrimitive(BuiltIn::T_BOOL);
				return;
			}
			case TokenKind::TILDE:
			{
				if (!is_integral(built_in))
				{
					break;
				}

				node->type = new ASTTypePrimitive(built_in);
				return;
			}
			default:
			{
				pt_unreachable();
			}
		}
	}
	else if (node->expr->type->kind == NodeKind::TYPE_REF)
	{

	}

	pt_error
	(
		"invalid argument type '%s' to unary operator '%s'",
		node->expr->type->to_string().data(),
		reserved_operators.at(node->op.kind).data()
	);

}

ASTExpression* SemanticAnalyzer::insert_cast_to(ASTExpression* dst_node, ASTTypePrimitive* src_type, ASTTypePrimitive* dst_type)
{
	if (src_type->is_equal(dst_type))
	{
		return dst_node;
	}
	else if (is_implicit_cast(src_type->built_in, dst_type->built_in))
	{
		return new ASTExpressionCast(dst_node, (ASTType*)dst_type->clone());
	}

	pt_error
	(
		"cannot perform implicit conversion between types: %s and %s",
		src_type->to_string(),
		dst_type->to_string()
	);

	return nullptr;
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);

	if (node->left->type->kind == NodeKind::TYPE_PRIM &&
		node->left->type->kind == NodeKind::TYPE_PRIM)
	{
		ASTTypePrimitive* left = (ASTTypePrimitive*)node->left->type;
		ASTTypePrimitive* right = (ASTTypePrimitive*)node->right->type;

		switch (node->op.kind)
		{
			case TokenKind::PLUS:
			case TokenKind::MINUS:
			case TokenKind::STAR:
			case TokenKind::SLASH:
			case TokenKind::PERCENT:
			{
				if (!is_numeric(left->built_in) || !is_numeric(right->built_in))
				{
					break;
				}

				BuiltIn common_built_in = get_common_numeric_type(left->built_in, right->built_in);
				ASTTypePrimitive* result_type = new ASTTypePrimitive(common_built_in);
				node->left = insert_cast_to(node->left, left, result_type);
				node->right = insert_cast_to(node->right, right, result_type);
				node->type = result_type;
				return;
			}
			case TokenKind::KW_AND:
			case TokenKind::KW_OR:
			{
				if (!is_logic(left->built_in) || !is_logic(right->built_in))
				{
					break;
				}

				node->type = new ASTTypePrimitive(BuiltIn::T_BOOL);
				return;
			}
			case TokenKind::AMPERSAND:
			case TokenKind::CARET:
			case TokenKind::VERTICAL_BAR:
			case TokenKind::LEFT_ANGLE_LEFT_ANGLE:
			case TokenKind::RIGHT_ANGLE_RIGHT_ANGLE:
			{
				if (!is_integral(left->built_in) || !is_integral(right->built_in))
				{
					break;
				}

				BuiltIn common_built_in = get_common_numeric_type(left->built_in, right->built_in);
				ASTTypePrimitive* result_type = new ASTTypePrimitive(common_built_in);
				node->left = insert_cast_to(node->left, left, result_type);
				node->right = insert_cast_to(node->right, right, result_type);
				node->type = result_type;
				return;
			}
			case TokenKind::LEFT_ANGLE:
			case TokenKind::LEFT_ANGLE_EQUAL:
			case TokenKind::RIGHT_ANGLE:
			case TokenKind::RIGHT_ANGLE_EQUAL:
			{
				if (!is_numeric(left->built_in) || !is_numeric(right->built_in))
				{
					break;
				}

				BuiltIn common_built_in = get_common_numeric_type(left->built_in, right->built_in);
				ASTTypePrimitive* result_type = new ASTTypePrimitive(common_built_in);
				node->left = insert_cast_to(node->left, left, result_type);
				node->right = insert_cast_to(node->right, right, result_type);
				result_type->built_in = BuiltIn::T_BOOL; // recycle?
				node->type = result_type;
				return;
			}
			case TokenKind::EQUAL:
			case TokenKind::NOT_EQUAL:
			{
				if (!is_numeric(left->built_in) || !is_numeric(right->built_in))
				{
					break;
				}

				BuiltIn common_built_in = get_common_numeric_type(left->built_in, right->built_in);
				ASTTypePrimitive* result_type = new ASTTypePrimitive(common_built_in);
				node->left = insert_cast_to(node->left, left, result_type);
				node->right = insert_cast_to(node->right, right, result_type);
				result_type->built_in = BuiltIn::T_BOOL; // recycle?
				node->type = result_type;
				return;
			}
			default:
			{
				pt_unreachable();
			}
		}
	}
	else if (node->left->type->kind == NodeKind::TYPE_REF &&
		node->right->type->kind == NodeKind::TYPE_REF)
	{
		ASTTypeReference* left = (ASTTypeReference*)node->left->type;
		ASTTypeReference* right = (ASTTypeReference*)node->right->type;

		switch (node->op.kind)
		{
			case TokenKind::KW_IS:
			{
				ASTTypePrimitive* result_type = new ASTTypePrimitive(BuiltIn::T_BOOL);
				node->type = result_type;
				return;
			}
			default:
			{
				pt_unreachable();
			}
		}
	}

	pt_not_implemented();
}

void SemanticAnalyzer::visit(ASTExpressionAssign* node)
{
	node->assignee->accept(this);
	node->expr->accept(this);

	node->expr =
		insert_cast_to
		(
			node->expr,
			(ASTTypePrimitive*)node->expr->type,
			(ASTTypePrimitive*)node->assignee->type->clone()
		);

	node->type = (ASTType*)node->assignee->type->clone();
}

void SemanticAnalyzer::visit(ASTExpressionName* node)
{
	node->name->accept(this);

	node->type =
		(ASTType*)((SymbolVariable*)node->name->symbol)->type->clone();
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
		symbol->var_kind = SymbolVariable::LOCAL;
		crt_scope->var_index++;
		crt_scope->add_symbol(symbol, node->name.buffer);
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

void SemanticAnalyzer::visit(ASTDeclarationFunction* node)
{
	if (pass_type == PassType::Declare)
	{
		Symbol* symbol = new SymbolFunction(node->name);

	}
}

void SemanticAnalyzer::visit(ASTStatementExpression* node)
{
	if (pass_type == PassType::Resolve)
	{
		node->expr->accept(this);
	}
}

void SemanticAnalyzer::visit(ASTStatementBlock* node)
{
	if (pass_type == PassType::Declare)
	{
		node->scope = new Scope(Scope::ScopeKind::BLOCK);
	}

	push_scope(node->scope);

	for (size_t i = 0; i < node->declarations.size(); i++)
	{
		node->declarations[i]->accept(this);
	}

	pop_scope();
}