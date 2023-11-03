#include "compiler.h"

#define emit(x, ...) \
	{ \
		uint8_t arr[] = { (uint8_t)(x), __VA_ARGS__ }; \
		code.insert(code.end(), arr, arr + (sizeof(arr) / sizeof(arr[0]))); \
	} \

void Compiler::emit_pop()
{
	emit(Bytecode::POP);
}

void Compiler::emit_dup()
{
	emit(Bytecode::DUP);
}

void Compiler::emit_load_const(uint8_t value)
{
	emit(Bytecode::LOAD_CONST, value);
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
			emit(Bytecode::LOAD_LOCAL, index);
			break;
		}
		case SymbolVariable::MEMBER:
		{
			emit(Bytecode::LOAD_FIELD, index);
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
			emit(Bytecode::STORE_LOCAL, index);
			break;
		}
		case SymbolVariable::MEMBER:
		{
			emit(Bytecode::STORE_FIELD, index);
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

void Compiler::emit_arith_bin_op_i(TokenKind op)
{
	Bytecode inst;
	switch (op)
	{
		case TokenKind::PLUS:		inst = Bytecode::ADD_I; break;
		case TokenKind::MINUS:		inst = Bytecode::SUB_I; break;
		case TokenKind::STAR:		inst = Bytecode::MUL_I; break;
		case TokenKind::SLASH:		inst = Bytecode::DIV_I; break;
		case TokenKind::PERCENT:	inst = Bytecode::MOD_I; break;
		default:					pt_unreachable();
	}
	emit(inst);
}

void Compiler::emit_arith_bin_op_f(TokenKind op)
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

void Compiler::emit_arith_bin_op(PrimitiveType primitive_type, TokenKind op)
{
	switch (primitive_type)
	{
		case PrimitiveType::T_INT:		emit_arith_bin_op_i(op); break;
		case PrimitiveType::T_FLOAT:	emit_arith_bin_op_f(op); break;
		default:						pt_unreachable();
	}
}

void Compiler::emit_arith_un_op_i(TokenKind op)
{
	switch (op)
	{
		case TokenKind::PLUS:		/* nothing to do */ break;
		case TokenKind::MINUS:		emit(Bytecode::NEG_I); break;
		default:					pt_unreachable();
	}
}

void Compiler::emit_arith_un_op_f(TokenKind op)
{
	switch (op)
	{
		case TokenKind::PLUS:		/* nothing to do */ break;
		case TokenKind::MINUS:		emit(Bytecode::NEG_F); break;
		default:					pt_unreachable();
	}
}

void Compiler::emit_arith_un_op(PrimitiveType primitive_type, TokenKind op)
{
	switch (primitive_type)
	{
		case PrimitiveType::T_INT:		emit_arith_un_op_i(op); break;
		case PrimitiveType::T_FLOAT:	emit_arith_un_op_f(op); break;
		default:						pt_unreachable();
	}
}

void Compiler::emit_convert_i2f() { emit(Bytecode::I2F); }

void Compiler::emit_convert_f2i() { emit(Bytecode::F2I); }

void Compiler::emit_convert(PrimitiveType src, PrimitiveType dst)
{
	if (src == dst)
	{
		return;
	}

	switch (src)
	{
		case PrimitiveType::T_FLOAT:	emit_convert_i2f(); break;
		case PrimitiveType::T_INT:		emit_convert_f2i(); break;
		default: pt_unreachable();
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
		printf("%llu\t%s\t", i, mnemonic);
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
	//PrimitiveType src = node->expr->type->primitive_type;
	//PrimitiveType dst = node->type->primitive_type;
	//emit_convert(src, dst);
}

void Compiler::visit(ASTExpressionGroup* node)
{
	node->expr->accept(this);
}

void Compiler::visit(ASTExpressionLiteral* node)
{
	int64_t value = std::stoll(node->token.buffer);

	// TODO :	emit the appropiate instruction
	//			depending on value

	emit_load_const(value);
}

void Compiler::visit(ASTExpressionUnary* node)
{
	node->expr->accept(this);

	//emit_arith_un_op(node->expr->type->primitive_type, node->op.kind);
}

void Compiler::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);
	//emit_arith_bin_op(node->type->primitive_type, node->op.kind);
}

void Compiler::visit(ASTExpressionAssign* node)
{
	//uint8_t index = 0;
	//node->expr->accept(this);
	//emit_dup();
	//emit_store_local(index);
}

void Compiler::visit(ASTExpressionName* node)
{

}

void Compiler::visit(ASTDeclarationVariable* node)
{
	node->expr->accept(this);
}