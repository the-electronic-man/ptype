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

	void visit(ASTType* node) override;

	void visit(ASTExpressionCast* node) override;
	void visit(ASTExpressionGroup* node) override;
	void visit(ASTExpressionLiteral* node) override;
	void visit(ASTExpressionUnary* node) override;
	void visit(ASTExpressionBinary* node) override;
	void visit(ASTExpressionAssign* node) override;
	void visit(ASTExpressionName* node) override;

	void visit(ASTDeclarationVariable* node) override;

	void visit(ASTStatementBlock* node) override;

	//ASTType* resolve_un_op_built_in_type(TokenKind op, BuiltIn built_in_type);
	//ASTType* resolve_bin_op_built_in_type(TokenKind op, BuiltIn built_in_type);

	bool is_un_op_arith(TokenKind op);
	bool is_un_op_logic(TokenKind op);
	bool is_un_op_bitwise(TokenKind op);
	bool is_numeric(BuiltIn built_in_type);
	bool is_char(BuiltIn built_in_type);
	bool is_integer(BuiltIn built_in_type);
	bool is_integral(BuiltIn built_in_type);
	bool is_decimal(BuiltIn built_in_type);
	bool is_logic(BuiltIn built_in_type);
	bool is_void(BuiltIn built_in_type);
	bool is_reference(BuiltIn built_in_type);
	bool is_array(BuiltIn built_in_type);
	bool is_primitive(BuiltIn built_in_type);

	bool is_implicit_cast(BuiltIn src_type, BuiltIn dst_type);

	BuiltIn get_common_numeric_type(BuiltIn src, BuiltIn dst);

	ASTExpression* insert_cast_to(ASTExpression* dst_node, ASTType* src_type, ASTType* dst_type);
};