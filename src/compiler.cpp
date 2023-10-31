#include "compiler.h"

#define emit(x, ...) \
	code.insert(code.end(), (uint8_t)(x), __VA_ARGS__)

void Compiler::emit_pop()
{
	emit(Bytecode::pop);
}

void Compiler::emit_dup()
{
	emit(Bytecode::dup);
}

void Compiler::emit_const(uint8_t value)
{
	emit(Bytecode::load_const, value);
}

void Compiler::emit_arith_bin_op_i(TokenKind op)
{
	Bytecode inst;
	switch (op)
	{
		case TokenKind::PLUS:		inst = Bytecode::add_i; break;
		case TokenKind::MINUS:		inst = Bytecode::sub_i; break;
		case TokenKind::STAR:		inst = Bytecode::mul_i; break;
		case TokenKind::SLASH:		inst = Bytecode::div_i; break;
		case TokenKind::PERCENT:	inst = Bytecode::mod_i; break;
		default:					pt_unreachable();
	}
	emit(inst);
}

void Compiler::emit_arith_bin_op_f(TokenKind op)
{
	Bytecode inst;
	switch (op)
	{
		case TokenKind::PLUS:		inst = Bytecode::add_f; break;
		case TokenKind::MINUS:		inst = Bytecode::sub_f; break;
		case TokenKind::STAR:		inst = Bytecode::mul_f; break;
		case TokenKind::SLASH:		inst = Bytecode::div_f; break;
		case TokenKind::PERCENT:	inst = Bytecode::mod_f; break;
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

}

void Compiler::emit_arith_un_op_f(TokenKind op)
{

}

void Compiler::emit_arith_un_op(PrimitiveType primitive_type, TokenKind op)
{

}


void Compiler::process(ASTNode* node)
{
	node->accept(this);
}

void Compiler::disassemble()
{
	for (size_t i = 0; i < code.size(); i++)
	{
		size_t operand_count = bytecode_operand_count((Bytecode)code[i]);
		const char* mnemonic = bytecode_to_string((Bytecode)code[i]);
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

	emit_const(value);
}

void Compiler::visit(ASTExpressionUnary* node)
{
	node->expr->accept(this);
	switch (node->type->primitive_type)
	{
		case PrimitiveType::T_INT:
		{
			switch (node->op.kind)
			{
				case TokenKind::PLUS: break;
				case TokenKind::MINUS: code.push_back((uint8_t)Bytecode::neg_i); break;
				default: break;
			}
			break;
		}
		case PrimitiveType::T_FLOAT:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void Compiler::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);

	switch (node->type->primitive_type)
	{
		case PrimitiveType::T_INT:
		{
			switch (node->op.kind)
			{
				case TokenKind::PLUS: code.push_back((uint8_t)Bytecode::add_i); break;
				case TokenKind::MINUS: code.push_back((uint8_t)Bytecode::sub_i); break;
				case TokenKind::STAR: code.push_back((uint8_t)Bytecode::mul_i); break;
				case TokenKind::SLASH: code.push_back((uint8_t)Bytecode::div_i); break;
				case TokenKind::PERCENT: code.push_back((uint8_t)Bytecode::mod_i); break;
				default: break;
			}
			break;
		}
		case PrimitiveType::T_FLOAT:
		{

			break;
		}
		default:
		{
			break;
		}
	}
}

void Compiler::visit(ASTExpressionAssign* node)
{

}

void Compiler::visit(ASTExpressionName* node)
{

}

void Compiler::visit(ASTDeclarationVariable* node)
{
	node->expr->accept(this);
}