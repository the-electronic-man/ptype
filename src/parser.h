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
	ASTStatement* parse_stmt_block();

	ASTDeclaration* parse_decl();
	ASTDeclaration* parse_decl_var();
	ASTDeclaration* parse_decl_param();
};