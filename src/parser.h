#pragma once
#include "ast.h"
#include "lexer.h"
#include "util.h"

struct Parser
{
	Token crt_token;
	Token* token_list;
	size_t token_count;
	size_t token_index;

	Parser();

	Token expect(TokenKind expected_token_kind);

	ASTNode* parse(Token* token_list, size_t token_count);

	void error(const char* format, ...);
	//void synchronize();

	ASTType* parse_type();
	//ASTType* parse_type_array();
	//ASTType* parse_type_function();
	ASTType* parse_type_name();

	ASTName* parse_name();

	ASTExpression* parse_expr_primary();
	ASTExpression* parse_expr_postfix();
	ASTExpression* parse_expr_unary();
	ASTExpression* parse_expr_factor();
	ASTExpression* parse_expr_term();
	ASTExpression* parse_expr_shift();
	ASTExpression* parse_expr_relational();
	ASTExpression* parse_expr_equality();
	ASTExpression* parse_expr_bitwise_and();
	ASTExpression* parse_expr_bitwise_xor();
	ASTExpression* parse_expr_bitwise_or();
	ASTExpression* parse_expr_logic_and();
	ASTExpression* parse_expr_logic_or();
	ASTExpression* parse_expr_assignment();
	ASTExpression* parse_expr_call();
	ASTExpression* parse_expr();

	ASTDeclaration* parse_stmt();
	ASTStatement* parse_stmt_expr();
	ASTStatement* parse_stmt_return();
	ASTStatement* parse_stmt_break();
	ASTStatement* parse_stmt_continue();
	ASTStatement* parse_stmt_block();

	ASTDeclaration* parse_decl();
	ASTDeclarationVariable* parse_decl_var();
	ASTDeclarationFunction* parse_decl_fun();
	ASTDeclarationClass* parse_decl_class();
	ASTDeclarationVariable* parse_decl_param();
};