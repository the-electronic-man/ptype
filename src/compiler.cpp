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

void Compiler::emit_arith_bin_op(BuiltIn built_in_type, TokenKind op)
{
	switch (built_in_type)
	{
		case BuiltIn::T_CHAR:
		case BuiltIn::T_I8:
		case BuiltIn::T_I16:
		case BuiltIn::T_I32:
		{
			emit_arith_bin_op_i(op);
			break;
		}
		case BuiltIn::T_F32:
		{
			emit_arith_bin_op_f(op);
			break;
		}
		default:
		{
			pt_unreachable();
		}
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

void Compiler::emit_arith_un_op(BuiltIn built_in_type, TokenKind op)
{
	switch (built_in_type)
	{
		case BuiltIn::T_I8:
		case BuiltIn::T_I16:
		case BuiltIn::T_I32:
		{
			emit_arith_un_op_i(op);
			break;
		}
		case BuiltIn::T_F32:
		{
			emit_arith_un_op_f(op);
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
		case BuiltIn::T_F32:
		{
			emit(Bytecode::I2F);
			break;
		}
		case BuiltIn::T_I8:
		case BuiltIn::T_I16:
		case BuiltIn::T_I32:
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
	//BuiltIn src = node->expr->type->built_in_type;
	//BuiltIn dst = node->type->built_in_type;
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
	BuiltIn built_in_type = node->type->built_in_type;
	emit_arith_un_op(built_in_type, node->op.kind);
}

void Compiler::visit(ASTExpressionBinary* node)
{
	node->left->accept(this);
	node->right->accept(this);
	BuiltIn built_in_type = node->type->built_in_type;
	emit_arith_bin_op(built_in_type, node->op.kind);
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
	for (size_t i = 0; i < node->statements.size(); i++)
	{
		node->statements[i]->accept(this);
	}
}

void Compiler::visit(ASTStatementExpression* node)
{
	node->expr->accept(this);
	if (!is_void(node->expr->type->built_in_type))
	{
		// we can emit a warning for discarded value here
		emit_pop();
	}
}