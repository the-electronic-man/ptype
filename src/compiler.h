#pragma once
#include "ast.h"
#include "bytecode.h"
#include "debug.h"

struct Compiler : Visitor
{
	std::vector<uint8_t> code;

	void disassemble();

	void process(ASTNode* node) override;

	void visit(ASTExpressionCast* node) override;
	void visit(ASTExpressionGroup* node) override;
	void visit(ASTExpressionLiteral* node) override;
	void visit(ASTExpressionUnary* node) override;
	void visit(ASTExpressionBinary* node) override;
	void visit(ASTExpressionAssign* node) override;
	void visit(ASTExpressionName* node) override;

	void visit(ASTDeclarationVariable* node) override;

	void visit(ASTStatementBlock* node) override;

	void emit_pop();
	void emit_dup();

	void emit_load_const(uint8_t value);

	void emit_load(SymbolVariable::VariableKind kind, uint8_t index);
	void emit_store(SymbolVariable::VariableKind kind, uint8_t index);

	void emit_arith_bin_op_i(TokenKind op);
	void emit_arith_bin_op_f(TokenKind op);
	void emit_arith_bin_op(BuiltIn built_in_type, TokenKind op);
	void emit_arith_un_op_i(TokenKind op);
	void emit_arith_un_op_f(TokenKind op);
	void emit_arith_un_op(BuiltIn built_in_type, TokenKind op);
	void emit_convert(BuiltIn src, BuiltIn dst);
};