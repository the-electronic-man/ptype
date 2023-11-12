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
			node->type = new ASTTypePrimitive(BuiltIn::T_I32);
			break;
		}
		case TokenKind::LITERAL_FLOAT:
		{
			node->type = new ASTTypePrimitive(BuiltIn::T_F32);
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

bool SemanticAnalyzer::is_numeric(BuiltIn built_in_type)
{
	return
		is_integral(built_in_type) ||
		is_decimal(built_in_type);
}

bool SemanticAnalyzer::is_integral(BuiltIn built_in_type)
{
	return
		is_integer(built_in_type) ||
		is_char(built_in_type);
}

bool SemanticAnalyzer::is_char(BuiltIn built_in_type)
{
	return built_in_type == BuiltIn::T_CHAR;
}

bool SemanticAnalyzer::is_integer(BuiltIn built_in_type)
{
	return
		built_in_type == BuiltIn::T_I8 ||
		built_in_type == BuiltIn::T_I16 ||
		built_in_type == BuiltIn::T_I32;
}

bool SemanticAnalyzer::is_decimal(BuiltIn built_in_type)
{
	return
		built_in_type == BuiltIn::T_F32;
}

bool SemanticAnalyzer::is_logic(BuiltIn built_in_type)
{
	return
		built_in_type == BuiltIn::T_BOOL;
}

bool SemanticAnalyzer::is_void(BuiltIn built_in_type)
{
	return
		built_in_type == BuiltIn::T_VOID;
}

bool SemanticAnalyzer::is_reference(BuiltIn built_in_type)
{
	return built_in_type == BuiltIn::T_REF;
}

bool SemanticAnalyzer::is_array(BuiltIn built_in_type)
{
	return built_in_type == BuiltIn::T_ARR;
}


void SemanticAnalyzer::visit(ASTExpressionUnary* node)
{
	node->expr->accept(this);

	if (node->expr->type->kind == NodeKind::TYPE_PRIMITIVE)
	{
		BuiltIn built_in_type =
			((ASTTypePrimitive*)node->expr->type)->built_in_type;

		switch (node->op.kind)
		{
			case TokenKind::PLUS:
			case TokenKind::MINUS:
			{
				if (is_numeric(built_in_type))
				{
					node->type = new ASTTypePrimitive(built_in_type);
					return;
				}
				break;
			}
			case TokenKind::KW_NOT:
			{
				if (is_logic(built_in_type))
				{
					node->type = new ASTTypePrimitive(BuiltIn::T_BOOL);
					return;
				}
				break;
			}
			case TokenKind::TILDE:
			{
				if (is_integral(built_in_type))
				{
					node->type = new ASTTypePrimitive(built_in_type);
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

BuiltIn SemanticAnalyzer::get_common_numeric_type(BuiltIn src, BuiltIn dst)
{
	// convesion to the same type
	if (src == dst)
	{
		return src;
	}

	// any integral to decimal is allowed
	if (is_integral(src))
	{
		if (is_decimal(dst))
		{
			return dst;
		}
		else if (is_integral(dst))
		{

		}
	}


	return (BuiltIn)std::max((int)src, int(dst));
}

void SemanticAnalyzer::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);

	// node->type = new ASTTypePrimitive(BuiltIn::T_I);

	if (node->left->type->kind == NodeKind::TYPE_PRIMITIVE &&
		node->right->type->kind == NodeKind::TYPE_PRIMITIVE)
	{
		BuiltIn left = ((ASTTypePrimitive*)node->left->type)->built_in_type;
		BuiltIn right = ((ASTTypePrimitive*)node->right->type)->built_in_type;

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
					if (left != right)
					{
						pt_error
						(
							"implicit conversion not allowed between %s and %s",
							built_in_to_string(left),
							built_in_to_string(right)
						);
					}
					ASTType* result_type = new ASTTypePrimitive(left);
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
					node->type = new ASTTypePrimitive(BuiltIn::T_BOOL);
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
					if (left != right)
					{
						pt_error
						(
							"implicit conversion not allowed between %s and %s",
							built_in_to_string(left),
							built_in_to_string(right)
						);
					}
					ASTType* result_type = new ASTTypePrimitive(left);
					node->type = result_type;
					return;
				}
				break;
			}
			case TokenKind::LEFT_ANGLE:
			case TokenKind::LEFT_ANGLE_EQUAL:
			case TokenKind::RIGHT_ANGLE:
			case TokenKind::RIGHT_ANGLE_EQUAL:
			{
				if (is_numeric(left) && is_numeric(right))
				{
					if (left != right)
					{
						pt_error
						(
							"implicit conversion not allowed between %s and %s",
							built_in_to_string(left),
							built_in_to_string(right)
						);
					}
					ASTType* result_type = new ASTTypePrimitive(left);
					node->type = result_type;
					return;
				}
				break;
			}
			case TokenKind::EQUAL:
			case TokenKind::NOT_EQUAL:
			{
				if (is_numeric(left) && is_numeric(right))
				{
					if (left != right)
					{
						pt_error
						(
							"implicit conversion not allowed between %s and %s",
							built_in_to_string(left),
							built_in_to_string(right)
						);
					}
					ASTType* result_type = new ASTTypePrimitive(left);
					node->type = result_type;
					return;
				}
				else if (is_reference(left) && is_reference(right))
				{

				}
				else if (is_array(left) && is_array(right))
				{

				}
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