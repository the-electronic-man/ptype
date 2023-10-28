#include "parser.h"

#pragma once
#include "ast.h"
#include "lexer.h"

#include <algorithm>

Parser::Parser()
{
	token_list = nullptr;
	token_count = 0;
	token_index = 0;
	crt_token = Token();
}

ASTNode* Parser::parse(Token* token_list, size_t token_count)
{
	this->token_list = token_list;
	this->token_count = token_count;
	this->token_index = 0;
	this->crt_token = token_list[token_index];

	return parse_decl_var();
}

Token Parser::expect(TokenKind expected_token_kind, const char* msg = "")
{
	Token last_token = crt_token;
	if (crt_token.kind == expected_token_kind)
	{
		token_index++;
		if (token_index < token_count)
		{
			crt_token = token_list[token_index];
		}
	}
	else
	{
		pt_error(msg);
	}

	return last_token;
}

ASTExpression* Parser::parse_expr_primary()
{
	// literals
	// 'this'
	// '(' EXPRESSION ')'

	switch (crt_token.kind)
	{
		case TokenKind::LITERAL_NULL:
		case TokenKind::LITERAL_BOOL:
		case TokenKind::LITERAL_CHAR:
		case TokenKind::LITERAL_INT:
		case TokenKind::LITERAL_FLOAT:
		case TokenKind::LITERAL_STRING:
		{
			Token token = expect(crt_token.kind);
			return new ASTExpressionLiteral(token);
		}
		case TokenKind::GROUP_LEFT_PAREN:
		{
			(void)expect(TokenKind::GROUP_LEFT_PAREN);
			ASTExpression* expr = new ASTExpressionGroup(parse_expr());
			(void)expect(TokenKind::GROUP_RIGHT_PAREN);
			return expr;
		}
		default:
		{
			return nullptr;
		}
	}

}

ASTExpression* Parser::parse_expr_unary()
{
	switch (crt_token.kind)
	{
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		case TokenKind::KW_NOT:
		case TokenKind::PUNCT_TILDE:
		{
			Token op = expect(crt_token.kind);
			ASTExpression* expr = parse_expr_primary();
			return new ASTExpressionUnary(op, expr);
		}
		default:
		{
			return parse_expr_primary();
		}
	}
}

ASTExpression* Parser::parse_expr_factor()
{
	ASTExpression* node = parse_expr_unary();

	while (Match(crt_token.kind,
		{ TokenKind::STAR, TokenKind::SLASH, TokenKind::PERCENT }))
	{
		Token op = expect(crt_token.kind);
		node = new ASTExpressionBinary(op, node, parse_expr_factor());
	}

	return node;
}

ASTExpression* Parser::parse_expr_term()
{
	ASTExpression* node = parse_expr_factor();

	while (Match(crt_token.kind,
		{ TokenKind::PLUS, TokenKind::MINUS }))
	{
		Token op = expect(crt_token.kind);
		node = new ASTExpressionBinary(op, node, parse_expr_term());
	}

	return node;
}

ASTExpression* Parser::parse_expr_shift()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_relational()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_equality()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_bitwise_and()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_bitwise_xor()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_bitwise_or()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_logic_and()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_logic_or()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr_assignment()
{
	ASTExpression* node = nullptr;
	return node;
}

ASTExpression* Parser::parse_expr()
{
	ASTExpression* node = parse_expr_term();
	return node;
}

ASTDeclaration* Parser::parse_decl_var()
{
	(void)expect(TokenKind::KW_VAR);
	Token var_name = expect(TokenKind::IDENTIFIER, "expected variable name");
	ASTExpression* expr = nullptr;

	switch (crt_token.kind)
	{
		case TokenKind::ASSIGN_DIRECT:
		{
			(void)expect(TokenKind::ASSIGN_DIRECT);
			expr = parse_expr();
			break;
		}
		case TokenKind::PUNCT_SEMICOLON:
		{
			(void)expect(TokenKind::PUNCT_SEMICOLON);
			// TODO : type decl
			break;
		}
		default:
		{
			pt_error("unexpected token '%s'", token_kind_to_string(crt_token.kind));
		}
	}

	ASTNode* node = new ASTDeclarationVariable(var_name, nullptr, expr);
	return node;
}
