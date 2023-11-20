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

	void push_scope(Scope* scope);
	void pop_scope();

	void process(ASTNode* node) override;

	void visit(ASTNameSimple* node) override;
	void visit(ASTNameQualified* node) override;

	// void visit(ASTTypePrimitive* node) override;
	// void visit(ASTTypeArray* node) override;
	// void visit(ASTTypeReference* node) override;
	// void visit(ASTTypeFunction* node) override;

	void visit(ASTExpressionCast* node) override;
	void visit(ASTExpressionGroup* node) override;
	void visit(ASTExpressionLiteral* node) override;
	void visit(ASTExpressionUnary* node) override;
	void visit(ASTExpressionBinary* node) override;
	void visit(ASTExpressionAssign* node) override;
	void visit(ASTExpressionName* node) override;

	void visit(ASTDeclarationVariable* node) override;
	void visit(ASTDeclarationFunction* node) override;

	void visit(ASTStatementBlock* node) override;
	void visit(ASTStatementExpression* node) override;

	//ASTType* resolve_un_op_built_in_type(TokenKind op, BuiltIn built_in_type);
	//ASTType* resolve_bin_op_built_in_type(TokenKind op, BuiltIn built_in_type);



	ASTExpression* insert_cast_to(ASTExpression* dst_node, ASTTypePrimitive* src_type, ASTTypePrimitive* dst_type);
};