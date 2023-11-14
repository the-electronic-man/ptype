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

	BuiltIn built_in_type = node->expr->type->built_in_type;

	switch (node->op.kind)
	{
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		{
			if (is_numeric(built_in_type))
			{
				node->type = new ASTType(built_in_type);
				return;
			}
			break;
		}
		case TokenKind::KW_NOT:
		{
			if (is_logic(built_in_type))
			{
				node->type = new ASTType(BuiltIn::T_BOOL);
				return;
			}
			break;
		}
		case TokenKind::TILDE:
		{
			if (is_integral(built_in_type))
			{
				node->type = new ASTType(built_in_type);
				return;
			}
			break;
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

	pt_unreachable();
}

BuiltIn SemanticAnalyzer::get_common_numeric_type(BuiltIn src, BuiltIn dst)
{
	//	float32
	//	int32
	//	int16
	//	int8
	//	char
	return (BuiltIn)std::max((int)src, int(dst));
}

bool SemanticAnalyzer::is_implicit_cast(BuiltIn src_type, BuiltIn dst_type)
{
	// TODO
	return false;
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

	// node->type = new ASTTypePrimitive(BuiltIn::T_I);

	BuiltIn left = node->left->type->built_in_type;
	BuiltIn right = node->right->type->built_in_type;

	// handle arithmetic operators here
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
			if (is_reference(left) && is_reference(right))
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

	// handle operator overloading here
	pt_unreachable();

}

void SemanticAnalyzer::visit(ASTExpressionAssign* node)
{
	node->assignee->accept(this);
	node->expr->accept(this);
	// insert a cast node if needed
	if (!node->expr->type->is_equal(node->assignee->type))
	{

	}

	// the node's type will always be the assignee's type
	// node->type = (ASTType*)node->assignee->type->clone();
	node->type = (ASTType*)node->assignee->clone();
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