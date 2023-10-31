#include "compiler.h"

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

}

void Compiler::visit(ASTExpressionUnary* node)
{

}

void Compiler::visit(ASTExpressionBinary* node)
{
	
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