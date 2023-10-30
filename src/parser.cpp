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

Token Parser::expect(TokenKind expected_token_kind, const char* msg)
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

ASTNode* Parser::parse(Token* token_list, size_t token_count)
{
	this->token_list = token_list;
	this->token_count = token_count;
	this->token_index = 0;
	this->crt_token = token_list[token_index];

	return parse_decl_var();
}

ASTType* Parser::parse_type()
{
	PrimitiveType type;
	if (!is_string_a_primitive_type(type, crt_token.buffer))
	{
		pt_log("%s is not a primitive type", crt_token.buffer.data());
		return nullptr;
	}
	(void)expect(TokenKind::IDENTIFIER);
	ASTType* node = new ASTTypePrimitive(type);
	return node;
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
		case TokenKind::IDENTIFIER:
		{
			ASTName* name = new ASTNameSimple(expect(crt_token.kind));
			ASTExpressionName* node = new ASTExpressionName(name);
			return node;
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

#define parse_function(function_name, other_function_name, ...) \
ASTExpression* Parser::function_name() \
{ \
	ASTExpression* node = other_function_name(); \
	while (Match(crt_token.kind, { __VA_ARGS__ })) \
	{ \
		Token op = expect(crt_token.kind); \
		node = new ASTExpressionBinary(op, node, function_name()); \
	} \
	return node; \
} \

parse_function
(
	parse_expr_factor,
	parse_expr_unary, 
	TokenKind::STAR, TokenKind::SLASH, TokenKind::PERCENT
)

parse_function
(
	parse_expr_term,
	parse_expr_factor,
	TokenKind::PLUS, TokenKind::MINUS
)

parse_function
(
	parse_expr_shift,
	parse_expr_term,
	TokenKind::BIT_SHL, TokenKind::BIT_SHR
)

parse_function
(
	parse_expr_relational,
	parse_expr_shift,
	TokenKind::LESS_THAN, TokenKind::LESS_EQUAL, TokenKind::GREATER_THAN, TokenKind::GREATER_EQUAL
)

parse_function
(
	parse_expr_equality,
	parse_expr_relational,
	TokenKind::EQUALS, TokenKind::NOT_EQUALS
)

parse_function
(
	parse_expr_bitwise_and,
	parse_expr_equality,
	TokenKind::BIT_AND
)

parse_function
(
	parse_expr_bitwise_xor,
	parse_expr_bitwise_and,
	TokenKind::BIT_XOR
)

parse_function
(
	parse_expr_bitwise_or,
	parse_expr_bitwise_xor,
	TokenKind::BIT_OR
)

parse_function
(
	parse_expr_logic_and,
	parse_expr_bitwise_or,
	TokenKind::KW_AND
)

parse_function
(
	parse_expr_logic_or,
	parse_expr_logic_and,
	TokenKind::KW_OR
)

ASTExpression* Parser::parse_expr_assignment()
{
	ASTExpression* assignee = parse_expr_logic_or();
	if (crt_token.kind == TokenKind::ASSIGN_DIRECT)
	{
		Token token = expect(crt_token.kind);
		ASTExpression* expr = parse_expr_assignment();

		switch (assignee->kind)
		{
			case NodeKind::EXPR_NAME:
			{
				ASTExpressionAssign* node = new ASTExpressionAssign(assignee, expr);
				return node;
			}
			// TODO: field get
			// TODO: array get
			default:
			{
				pt_error("invalid assignment target");
			}
		}
	}
	return assignee;
}

ASTExpression* Parser::parse_expr()
{
	ASTExpression* node = parse_expr_assignment();
	return node;
}

ASTStatement* Parser::parse_stmt()
{
	switch (crt_token.kind)
	{
		case TokenKind::GROUP_LEFT_BRACE:
		{
			return parse_stmt_block();
		}
		default:
		{
			return parse_stmt_expr();
		}
	}
}

ASTStatement* Parser::parse_stmt_expr()
{
	ASTExpression* expr = parse_expr();
	ASTStatementExpression* node = new ASTStatementExpression(expr);
	(void)expect(TokenKind::PUNCT_SEMICOLON, "expected ';' after expression statement");
	return node;
}

ASTStatement* Parser::parse_stmt_block()
{
	std::vector<ASTStatement*> statements;
	(void)expect(TokenKind::GROUP_LEFT_BRACE);

	while (crt_token.kind != TokenKind::GROUP_RIGHT_BRACE
		&& crt_token.kind != TokenKind::END_OF_FILE)
	{
		ASTStatement* statement = parse_stmt();
		statements.push_back(statement);
	}

	(void)expect(TokenKind::GROUP_RIGHT_BRACE, "expected '}' after block statement");

	ASTStatementBlock* node = new ASTStatementBlock(statements);
	return node;
}

ASTDeclaration* Parser::parse_decl_var()
{
	// ----------------------------------------------------------------
	// var <name> ('=' <expr> | ':' <type> ('=' <expr>)? )
	// ----------------------------------------------------------------

	(void)expect(TokenKind::KW_VAR);
	Token var_name = expect(TokenKind::IDENTIFIER, "expected variable name");
	ASTExpression* expr = nullptr;
	ASTType* type = nullptr;

	switch (crt_token.kind)
	{
		case TokenKind::ASSIGN_DIRECT:
		{
			(void)expect(crt_token.kind);
			expr = parse_expr();
			(void)expect(TokenKind::PUNCT_SEMICOLON, "expected ';' after variable declaration");
			break;
		}
		case TokenKind::PUNCT_COLON:
		{
			(void)expect(crt_token.kind);
			type = parse_type();
			if (crt_token.kind == TokenKind::ASSIGN_DIRECT)
			{
				(void)expect(crt_token.kind);
				expr = parse_expr();
			}
			(void)expect(TokenKind::PUNCT_SEMICOLON, "expected ';' after variable declaration");
			break;
		}
		default:
		{
			pt_error("unexpected token '%s'", token_kind_to_string(crt_token.kind));
		}
	}

	ASTDeclaration* node = new ASTDeclarationVariable(var_name, type, expr);
	return node;
}