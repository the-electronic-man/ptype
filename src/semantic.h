#pragma once
#include "ast.h"

struct SemanticAnalyzer : Visitor
{
	SymbolNamespace* global_scope = nullptr;
	Scope* crt_scope = nullptr;

	SemanticAnalyzer()
	{
		global_scope = new SymbolNamespace(Token());
		crt_scope = global_scope;
	}

	~SemanticAnalyzer()
	{
		delete global_scope;
	}

	void process(ASTNode* node) override;

	void visit(ASTNameSimple* node) override;
	void visit(ASTNameQualified* node) override;

	void visit(ASTTypePrimitive* node) override;

	void visit(ASTExpressionCast* node) override;
	void visit(ASTExpressionGroup* node) override;
	void visit(ASTExpressionLiteral* node) override;
	void visit(ASTExpressionUnary* node) override;
	void visit(ASTExpressionBinary* node) override;
	void visit(ASTExpressionAssign* node) override;

	void visit(ASTDeclarationVariable* node) override;
};