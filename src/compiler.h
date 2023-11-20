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
	void visit(ASTStatementExpression* node) override;

	void emit_pop();
	void emit_dup();

	void emit_load_const_i(int32_t value);
	void emit_load_const_f(float value);

	void emit_load(SymbolVariable::VariableKind kind, uint8_t index);
	void emit_store(SymbolVariable::VariableKind kind, uint8_t index);

	void emit_bin_op_arith_i(TokenKind op);
	void emit_bin_op_arith_f(TokenKind op);
	void emit_bin_op_arith(BuiltIn built_in, TokenKind op);
	void emit_bin_op_rel_i(TokenKind op);
	void emit_bin_op_rel_f(TokenKind op);
	void emit_bin_op_rel(BuiltIn built_in, TokenKind op);
	void emit_un_op_arith_i(TokenKind op);
	void emit_un_op_arith_f(TokenKind op);
	void emit_un_op_arith(BuiltIn built_in, TokenKind op);
	void emit_convert(BuiltIn src, BuiltIn dst);

	size_t get_code_index();
};