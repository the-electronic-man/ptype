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

void SemanticAnalyzer::visit(ASTType* node)
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
			node->type = new ASTType(BuiltIn::T_I32);
			break;
		}
		case TokenKind::LITERAL_FLOAT:
		{
			node->type = new ASTType(BuiltIn::T_F32);
			break;
		}
		case TokenKind::LITERAL_CHAR:
		{
			node->type = new ASTType(BuiltIn::T_CHAR);
			break;
		}
		case TokenKind::LITERAL_TRUE:
		case TokenKind::LITERAL_FALSE:
		{
			node->type = new ASTType(BuiltIn::T_BOOL);
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

	BuiltIn built_in_type = node->expr->type->built_in_type;

	switch (node->op.kind)
	{
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		{
			if (!is_numeric(built_in_type))
			{
				break;
			}

			node->type = new ASTType(built_in_type);
			return;
		}
		case TokenKind::KW_NOT:
		{
			if (!is_logic(built_in_type))
			{
				break;
			}

			node->type = new ASTType(BuiltIn::T_BOOL);
			return;
		}
		case TokenKind::TILDE:
		{
			if (!is_integral(built_in_type))
			{
				break;
			}

			node->type = new ASTType(built_in_type);
			return;
		}
		default:
		{
			pt_unreachable();
		}
	}

	// if the expr child of the unary node is not a
	// primitive type then we have to replace the whole
	// unary node with a function call to the corresponding
	// overloaded operator (if it exists)

	// TODO : replace with a function call
	// node->parent->replace_child(node, nullptr);

	if (!is_reference(built_in_type))
	{
		pt_error
		(
			"invalid argument type '%s' to unary operator '%s'",
			built_in_to_string(built_in_type),
			reserved_operators.at(node->op.kind).data()
		);
	}

	pt_not_implemented();
}

ASTExpression* SemanticAnalyzer::insert_cast_to(ASTExpression* dst_node, ASTType* src_type, ASTType* dst_type)
{
	if (src_type->is_equal(dst_type))
	{
		return dst_node;
	}
	else if (is_implicit_cast(src_type->built_in_type, dst_type->built_in_type))
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

	BuiltIn left = node->left->type->built_in_type;
	BuiltIn right = node->right->type->built_in_type;

	switch (node->op.kind)
	{
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		case TokenKind::STAR:
		case TokenKind::SLASH:
		case TokenKind::PERCENT:
		{
			if (!is_numeric(left) || !is_numeric(right))
			{
				break;
			}

			BuiltIn common_built_in = get_common_numeric_type(left, right);
			ASTType* result_type = new ASTType(common_built_in);
			node->left = insert_cast_to(node->left, node->left->type, result_type);
			node->right = insert_cast_to(node->right, node->right->type, result_type);
			node->type = result_type;
			return;
		}
		case TokenKind::KW_AND:
		case TokenKind::KW_OR:
		{
			if (!is_logic(left) || !is_logic(right))
			{
				break;
			}

			node->type = new ASTType(BuiltIn::T_BOOL);
			return;
		}
		case TokenKind::AMPERSAND:
		case TokenKind::CARET:
		case TokenKind::VERTICAL_BAR:
		{
			if (!is_integral(left) || !is_integral(right))
			{
				break;
			}

			BuiltIn common_built_in = BuiltIn::T_I32;
			ASTType* result_type = new ASTType(common_built_in);
			node->left = insert_cast_to(node->left, node->left->type, result_type);
			node->right = insert_cast_to(node->right, node->right->type, result_type);
			node->type = result_type;
			return;
		}
		case TokenKind::LEFT_ANGLE:
		case TokenKind::LEFT_ANGLE_EQUAL:
		case TokenKind::RIGHT_ANGLE:
		case TokenKind::RIGHT_ANGLE_EQUAL:
		{
			if (!is_numeric(left) || !is_numeric(right))
			{
				break;
			}

			BuiltIn common_built_in = get_common_numeric_type(left, right);
			ASTType* result_type = new ASTType(common_built_in);
			node->left = insert_cast_to(node->left, node->left->type, result_type);
			node->right = insert_cast_to(node->right, node->right->type, result_type);
			result_type->built_in_type = BuiltIn::T_BOOL; // recycle?
			node->type = result_type;
			return;
		}
		case TokenKind::EQUAL:
		case TokenKind::NOT_EQUAL:
		{
			if (!is_numeric(left) || !is_numeric(right))
			{
				break;
			}

			BuiltIn common_built_in = get_common_numeric_type(left, right);
			ASTType* result_type = new ASTType(common_built_in);
			node->left = insert_cast_to(node->left, node->left->type, result_type);
			node->right = insert_cast_to(node->right, node->right->type, result_type);
			result_type->built_in_type = BuiltIn::T_BOOL; // recycle?
			node->type = result_type;
			return;
		}
		case TokenKind::KW_IS:
		{
			if (!is_reference(left) || !is_reference(right))
			{
				break;
			}

			ASTType* result_type = new ASTType(BuiltIn::T_BOOL);
			node->type = result_type;
			return;
		}
		default:
		{
			pt_unreachable();
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
			node->expr->type,
			(ASTType*)node->assignee->type->clone()
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

	for (size_t i = 0; i < node->declarations.size(); i++)
	{
		node->declarations[i]->accept(this);
	}
}