#pragma once
#include <iostream>
#include <fstream>
#include "ast.h"

struct TreePrinter : Visitor
{
	std::ofstream output_file;
	int ident = 0;
	std::string ident_str = "";
	void increase_ident() { ident++; generate_ident_str(); }
	void decrease_ident() { ident--; generate_ident_str(); }
	void generate_ident_str() { ident_str = std::string(ident, '\t'); }

	void process(ASTNode* node) override;

	void visit(ASTNameSimple* node) override {}
	void visit(ASTNameQualified* node) override {}

	void visit(ASTTypePrimitive* node) override {}
	void visit(ASTTypeArray* node) override {}
	void visit(ASTTypeReference* node) override {}
	void visit(ASTTypeFunction* node) override {}
	void visit(ASTTypeParameterized* node) override {}

	void visit(ASTExpressionCast* node) override;
	void visit(ASTExpressionGroup* node) override;
	void visit(ASTExpressionLiteral* node) override;
	void visit(ASTExpressionUnary* node) override;
	void visit(ASTExpressionBinary* node) override;
	void visit(ASTExpressionAssign* node) override;
	void visit(ASTExpressionName* node) override;
	void visit(ASTExpressionFieldGet* node) override;
	void visit(ASTExpressionFieldSet* node) override;
	void visit(ASTExpressionArrayGet* node) override;
	void visit(ASTExpressionArraySet* node) override;

	void visit(ASTDeclarationVariable* node) override;
	void visit(ASTDeclarationFunction* node) override;

	void visit(ASTStatementBlock* node) override;
	void visit(ASTStatementExpression* node) override;
	void visit(ASTStatementReturn* node) override;
	void visit(ASTStatementBreak* node) override;
	void visit(ASTStatementContinue* node) override;
};