#pragma once
#include "ast.h"
#include "bytecode.h"

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
};