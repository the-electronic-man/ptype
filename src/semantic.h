#pragma once
#include "ast.h"
#include "util.h"

struct SemanticAnalyzer : Visitor
{
	enum class PassType
	{
		Declare,
		Resolve
	};

	PassType pass_type;

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
	void visit(ASTTypeReference* node) override;
	void visit(ASTTypeArray* node) override;

	void visit(ASTExpressionCast* node) override;
	void visit(ASTExpressionGroup* node) override;
	void visit(ASTExpressionLiteral* node) override;
	void visit(ASTExpressionUnary* node) override;
	void visit(ASTExpressionBinary* node) override;
	void visit(ASTExpressionAssign* node) override;
	void visit(ASTExpressionName* node) override;

	void visit(ASTDeclarationVariable* node) override;

	void visit(ASTStatementBlock* node) override;

	ASTType* resolve_un_op_primitive_type(TokenKind op, PrimitiveType primitive_type);
	ASTType* resolve_bin_op_primitive_type(TokenKind op, PrimitiveType primitive_type);
};