#include "compiler.h"

#define emit(x, ...) \
	{ \
		uint8_t arr[] = { (uint8_t)(x), __VA_ARGS__ }; \
		code.insert(code.end(), arr, arr + (sizeof(arr) / sizeof(*arr))); \
	} \


void Compiler::emit_pop()
{
	emit(Bytecode::POP);
}

void Compiler::emit_dup()
{
	emit(Bytecode::DUP);
}

void Compiler::emit_load_const_i(int32_t value)
{
	if (value >= -1 && value <= 8)
	{
		emit((Bytecode)((int)Bytecode::LD_I4_0 + (int)value));
		return;
	}

	union
	{
		int32_t xi;
		float xf;
		uint8_t xa[4];
	} convert;
	convert.xi = value;
	emit
	(
		Bytecode::LD_I4,
		convert.xa[0],
		convert.xa[1],
		convert.xa[2],
		convert.xa[3]
	);

}

void Compiler::emit_load_const_f(float value)
{
	if (value == -1.0f)
	{
		emit(Bytecode::LD_F4_M1);
		return;
	}
	else if (value == 0.0f)
	{
		emit(Bytecode::LD_F4_0);
		return;
	}
	else if (value == 1.0f)
	{
		emit(Bytecode::LD_F4_1);
		return;
	}
	union
	{
		int32_t xi;
		float xf;
		uint8_t xa[4];
	} convert;
	convert.xf = value;
	emit
	(
		Bytecode::LD_F4,
		convert.xa[0],
		convert.xa[1],
		convert.xa[2],
		convert.xa[3]
	);
}

void Compiler::emit_load(SymbolVariable::VariableKind kind, uint8_t index)
{
	switch (kind)
	{
		case SymbolVariable::GLOBAL:
		{
			pt_not_implemented();
		}
		case SymbolVariable::LOCAL:
		case SymbolVariable::PARAMETER:
		{
			emit(Bytecode::LD_LOC, index);
			break;
		}
		case SymbolVariable::MEMBER:
		{
			emit(Bytecode::LD_FLD, index);
			break;
		}
		case SymbolVariable::STATIC:
		{
			pt_not_implemented();
		}
		default:
		{
			pt_unreachable();
		}
	}
}

void Compiler::emit_store(SymbolVariable::VariableKind kind, uint8_t index)
{
	switch (kind)
	{
		case SymbolVariable::GLOBAL:
		{
			pt_not_implemented();
		}
		case SymbolVariable::LOCAL:
		case SymbolVariable::PARAMETER:
		{
			emit(Bytecode::ST_LOC, index);
			break;
		}
		case SymbolVariable::MEMBER:
		{
			emit(Bytecode::ST_FLD, index);
			break;
		}
		case SymbolVariable::STATIC:
		{
			pt_not_implemented();
		}
		default:
		{
			pt_unreachable();
		}
	}
}

void Compiler::emit_bin_op_arith_i(TokenKind op)
{
	Bytecode inst;
	switch (op)
	{
		case TokenKind::PLUS:			inst = Bytecode::ADD_I; break;
		case TokenKind::MINUS:			inst = Bytecode::SUB_I; break;
		case TokenKind::STAR:			inst = Bytecode::MUL_I; break;
		case TokenKind::SLASH:			inst = Bytecode::DIV_I; break;
		case TokenKind::PERCENT:		inst = Bytecode::MOD_I; break;
		case TokenKind::AMPERSAND:		inst = Bytecode::AND_I; break;
		case TokenKind::CARET:			inst = Bytecode::XOR_I; break;
		case TokenKind::VERTICAL_BAR:	inst = Bytecode::OR_I; break;
		default:					pt_unreachable();
	}
	emit(inst);
}

void Compiler::emit_bin_op_arith_f(TokenKind op)
{
	Bytecode inst;
	switch (op)
	{
		case TokenKind::PLUS:		inst = Bytecode::ADD_F; break;
		case TokenKind::MINUS:		inst = Bytecode::SUB_F; break;
		case TokenKind::STAR:		inst = Bytecode::MUL_F; break;
		case TokenKind::SLASH:		inst = Bytecode::DIV_F; break;
		case TokenKind::PERCENT:	inst = Bytecode::MOD_F; break;
		default:					pt_unreachable();
	}
	code.push_back((uint8_t)inst);
}

void Compiler::emit_bin_op_arith(BuiltIn built_in, TokenKind op)
{
	switch (built_in)
	{
		case BuiltIn::T_CHAR:
		case BuiltIn::T_I1:
		case BuiltIn::T_I2:
		case BuiltIn::T_I4:
			// case BuiltIn::T_I8:
		{
			emit_bin_op_arith_i(op);
			break;
		}
		case BuiltIn::T_F4:
			// case BuiltIn::T_F8:
		{
			emit_bin_op_arith_f(op);
			break;
		}
		default:
		{
			pt_unreachable();
		}
	}
}

void Compiler::emit_bin_op_rel_i(TokenKind op)
{
	switch (op)
	{
		case TokenKind::LEFT_ANGLE:				emit(Bytecode::CLT); break;
		case TokenKind::LEFT_ANGLE_EQUAL:		emit(Bytecode::CLE); break;
		case TokenKind::RIGHT_ANGLE:			emit(Bytecode::CGT); break;
		case TokenKind::RIGHT_ANGLE_EQUAL:		emit(Bytecode::CGE); break;
		case TokenKind::EQUAL:					emit(Bytecode::CEQ); break;
		case TokenKind::NOT_EQUAL:				emit(Bytecode::CNE); break;
		default:								pt_unreachable();
	}
}

void Compiler::emit_bin_op_rel_f(TokenKind op)
{

}

void Compiler::emit_bin_op_rel(BuiltIn built_in, TokenKind op)
{
	switch (built_in)
	{
		case BuiltIn::T_CHAR:
		case BuiltIn::T_I1:
		case BuiltIn::T_I2:
		case BuiltIn::T_I4:
			// case BuiltIn::T_I8:
		{
			emit_bin_op_rel_i(op);
			break;
		}
		case BuiltIn::T_F4:
			// case BuiltIn::T_F8:
		{
			emit_bin_op_rel_f(op);
			break;
		}
		default:
		{
			pt_unreachable();
		}
	}
}

void Compiler::emit_un_op_arith_i(TokenKind op)
{
	switch (op)
	{
		case TokenKind::PLUS:		/* nothing to do */ break;
		case TokenKind::MINUS:		emit(Bytecode::NEG_I); break;
		case TokenKind::TILDE:		emit(Bytecode::COMPL_I); break;
		default:					pt_unreachable();
	}
}

void Compiler::emit_un_op_arith_f(TokenKind op)
{
	switch (op)
	{
		case TokenKind::PLUS:		/* nothing to do */ break;
		case TokenKind::MINUS:		emit(Bytecode::NEG_F); break;
		default:					pt_unreachable();
	}
}

void Compiler::emit_un_op_arith(BuiltIn built_in, TokenKind op)
{
	switch (built_in)
	{
		case BuiltIn::T_CHAR:
		case BuiltIn::T_I1:
		case BuiltIn::T_I2:
		case BuiltIn::T_I4:
			// case BuiltIn::T_I8:
		{
			emit_un_op_arith_i(op);
			break;
		}
		case BuiltIn::T_F4:
		{
			emit_un_op_arith_f(op);
			break;
		}
		default:
		{
			pt_unreachable();
		}
	}
}

void Compiler::emit_convert(BuiltIn src, BuiltIn dst)
{
	if (src == dst)
	{
		return;
	}

	switch (src)
	{
		case BuiltIn::T_F4:
		{
			emit(Bytecode::I2F);
			break;
		}
		case BuiltIn::T_BOOL:
		case BuiltIn::T_CHAR:
		case BuiltIn::T_I1:
		case BuiltIn::T_I2:
		case BuiltIn::T_I4:
			// case BuiltIn::T_I8:
		{
			emit(Bytecode::F2I);
			break;
		}
		default:
		{
			pt_unreachable();
		}
	}
}

void Compiler::process(ASTNode* node)
{
	node->accept(this);
}

void Compiler::disassemble()
{
	for (size_t i = 0; i < code.size(); i++)
	{
		Bytecode inst = (Bytecode)code[i];
		size_t operand_count = bytecode_operand_count(inst);
		const char* mnemonic = bytecode_to_string(inst);
		printf("%04llu\t%s\t", i, mnemonic);
		for (size_t j = 0; j < operand_count; j++)
		{
			printf("%d\t", code[i + 1 + j]);
		}
		printf("\n");
		i += operand_count;
	}
}

void Compiler::visit(ASTExpressionCast* node)
{
	node->expr->accept(this);
	//BuiltIn src = node->expr->type->built_in;
	//BuiltIn dst = node->type->built_in;
	//emit_convert(src, dst);
}

void Compiler::visit(ASTExpressionGroup* node)
{
	node->expr->accept(this);
}

void Compiler::visit(ASTExpressionLiteral* node)
{
	assert(node->type->kind == NodeKind::TYPE_PRIM);
	switch (((ASTTypePrimitive*)node->type)->built_in)
	{
		case BuiltIn::T_BOOL:
		{
			int32_t value = node->token.kind == TokenKind::LIT_TRUE;
			emit_load_const_i(value);
			break;
		}
		case BuiltIn::T_CHAR:
		{
			int32_t value = (int32_t)node->token.buffer[0];
			emit_load_const_i(value);
			break;
		}
		case BuiltIn::T_I1:
		case BuiltIn::T_I2:
		case BuiltIn::T_I4:
			// case BuiltIn::T_I8:
		{
			int32_t value = std::stoi(node->token.buffer);
			emit_load_const_i(value);
			break;
		}
		case BuiltIn::T_F4:
		{
			float value = std::stof(node->token.buffer);
			emit_load_const_f(value);
			break;
		}
		// case BuiltIn::T_F8:
		// {
		// 	double value = std::stod(node->token.buffer);
		// 	emit_load_const_f(value);
		// 	break;
		// }
	}
}

void Compiler::visit(ASTExpressionUnary* node)
{
	node->expr->accept(this);
	BuiltIn built_in = ((ASTTypePrimitive*)node->type)->built_in;
	emit_un_op_arith(built_in, node->op.kind);
}

void Compiler::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);
	BuiltIn built_in = ((ASTTypePrimitive*)node->type)->built_in;
	emit_bin_op_arith(built_in, node->op.kind);
}

void Compiler::visit(ASTExpressionAssign* node)
{
	SymbolVariable* var_symbol = (SymbolVariable*)node->assignee->name->symbol;
	node->expr->accept(this);
	emit_dup();
	emit_store(var_symbol->var_kind, var_symbol->index);
}

void Compiler::visit(ASTExpressionName* node)
{
	SymbolVariable* var_symbol = (SymbolVariable*)node->name->symbol;
	emit_load(var_symbol->var_kind, var_symbol->index);
}

void Compiler::visit(ASTDeclarationVariable* node)
{
	if (node->expr)
	{
		node->expr->accept(this);
	}
}

void Compiler::visit(ASTStatementBlock* node)
{
	for (size_t i = 0; i < node->declarations.size(); i++)
	{
		node->declarations[i]->accept(this);
	}
}

void Compiler::visit(ASTStatementExpression* node)
{
	node->expr->accept(this);
	if (!is_void(((ASTTypePrimitive*)node->expr->type)->built_in))
	{
		// we can emit a warning for discarded value here
		emit_pop();
	}
}

size_t Compiler::get_code_index()
{
	return code.size();
}