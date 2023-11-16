#include "parser.h"
#include "ast.h"
#include "lexer.h"

#include <algorithm>

Parser::Parser()
{
	token_list = nullptr;
	token_count = 0;
	token_index = 0;
	crt_token = Token();
	// is_panic = false;
	// is_error = false;
}

Token Parser::expect(TokenKind expected_token_kind)
{
	Token token = crt_token;
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
		error
		(
			"expected token type '%s', but got '%s' instead",
			token_kind_to_string(expected_token_kind),
			token_kind_to_string(crt_token.kind)
		);
	}

	return token;
}

ASTNode* Parser::parse(Token* token_list, size_t token_count)
{
	this->token_list = token_list;
	this->token_count = token_count;
	this->token_index = 0;
	this->crt_token = token_list[token_index];

	std::vector<ASTDeclaration*> declarations;
	while (crt_token.kind != TokenKind::END_OF_FILE)
	{
		ASTDeclaration* declaration = parse_decl();
		// if (is_error)
		// {
		// 	continue;
		// }
		declarations.push_back(declaration);
	}
	ASTStatementBlock* node = new ASTStatementBlock(declarations);
	return node;
}

void Parser::error(const char* format, ...)
{
	//if (is_panic)
	//{
	//	return;
	//}
	//is_panic = true;

	fprintf(stderr, "[error](%zd:%zd): ", crt_token.line, crt_token.column);
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");

	// is_error = true;

	exit(0);
}

// void Parser::synchronize()
// {
// 	is_panic = false;

// 	while (crt_token.kind != TokenKind::END_OF_FILE)
// 	{
// 		if (last_token.kind == TokenKind::SEMICOLON)
// 		{
// 			return;
// 		}
// 		switch (crt_token.kind)
// 		{
// 			case TokenKind::KW_VAR:
// 			{
// 				return;
// 			}
// 			default:
// 			{
// 				break;
// 			}
// 		}

// 		(void)expect(crt_token.kind);
// 	}
// }

ASTType* Parser::parse_type()
{
	BuiltIn built_in_type;
	if (!is_token_built_in_type(built_in_type, crt_token.kind))
	{
		pt_log("%s is not a primitive type", crt_token.buffer.data());
		return nullptr;
	}
	(void)expect(TokenKind::IDENTIFIER);
	ASTType* node = new ASTType(built_in_type);
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
		case TokenKind::LITERAL_TRUE:
		case TokenKind::LITERAL_FALSE:
		case TokenKind::LITERAL_CHAR:
		case TokenKind::LITERAL_INT:
		case TokenKind::LITERAL_FLOAT:
		case TokenKind::LITERAL_STRING:
		{
			Token token = expect(crt_token.kind);
			return new ASTExpressionLiteral(token);
		}
		case TokenKind::LEFT_PAREN:
		{
			(void)expect(crt_token.kind);
			ASTExpression* expr = new ASTExpressionGroup(parse_expr());
			(void)expect(TokenKind::RIGHT_PAREN);
			return expr;
		}
		case TokenKind::IDENTIFIER:
		{
			Token token = expect(crt_token.kind);
			ASTName* name = new ASTNameSimple(token);
			ASTExpressionName* node = new ASTExpressionName(name);
			return node;
		}
		case TokenKind::KW_CAST:
		{
			(void)expect(crt_token.kind);
			(void)expect(TokenKind::LEFT_PAREN);
			ASTType* type = parse_type();
			(void)expect(TokenKind::COMMA);
			ASTExpression* expr = parse_expr();
			(void)expect(TokenKind::RIGHT_PAREN);
			return new ASTExpressionCast(expr, type);
		}
		default:
		{
			error("invalid token type '%s' for primary expression", token_kind_to_string(crt_token.kind));
			return nullptr;
		}
	}

}

ASTExpression* Parser::parse_expr_postfix()
{
	ASTExpression* node = parse_expr_primary();
	bool is_recursing = true;
	while (is_recursing)
	{
		switch (crt_token.kind)
		{
			case TokenKind::DOT:
			{
				(void)expect(TokenKind::DOT);
				Token field_name = expect(TokenKind::IDENTIFIER);
				ASTNameSimple* field = new ASTNameSimple(field_name);
				node = new ASTExpressionFieldGet(node, field);
				break;
			}
			case TokenKind::LEFT_BRACKET:
			{
				(void)expect(TokenKind::LEFT_BRACKET);
				ASTExpression* index = parse_expr();
				(void)expect(TokenKind::RIGHT_BRACKET);
				Token field_name = expect(TokenKind::IDENTIFIER);
				ASTNameSimple* field = new ASTNameSimple(field_name);
				node = new ASTExpressionFieldGet(node, field);
				break;
			}
			default:
			{
				is_recursing = false;
				break;
			}
		}
	}
	return node;
}

ASTExpression* Parser::parse_expr_unary()
{
	switch (crt_token.kind)
	{
		case TokenKind::PLUS:
		case TokenKind::MINUS:
		case TokenKind::KW_NOT:
		case TokenKind::TILDE:
		{
			Token op = expect(crt_token.kind);
			ASTExpression* expr = parse_expr_unary();
			return new ASTExpressionUnary(op, expr);
		}
		default:
		{
			return parse_expr_postfix();
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

parse_function(parse_expr_factor, parse_expr_unary, TokenKind::STAR, TokenKind::SLASH, TokenKind::PERCENT)
parse_function(parse_expr_term, parse_expr_factor, TokenKind::PLUS, TokenKind::MINUS)
parse_function(parse_expr_shift, parse_expr_term, TokenKind::LEFT_ANGLE_LEFT_ANGLE, TokenKind::RIGHT_ANGLE_RIGHT_ANGLE)
parse_function(parse_expr_relational, parse_expr_shift, TokenKind::LEFT_ANGLE, TokenKind::LEFT_ANGLE_EQUAL, TokenKind::RIGHT_ANGLE, TokenKind::RIGHT_ANGLE_EQUAL)
parse_function(parse_expr_equality, parse_expr_relational, TokenKind::EQUAL, TokenKind::NOT_EQUAL, TokenKind::KW_IS)
parse_function(parse_expr_bitwise_and, parse_expr_equality, TokenKind::AMPERSAND)
parse_function(parse_expr_bitwise_xor, parse_expr_bitwise_and, TokenKind::CARET)
parse_function(parse_expr_bitwise_or, parse_expr_bitwise_xor, TokenKind::VERTICAL_BAR)
parse_function(parse_expr_logic_and, parse_expr_bitwise_or, TokenKind::KW_AND)
parse_function(parse_expr_logic_or, parse_expr_logic_and, TokenKind::KW_OR)



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
				ASTExpressionAssign* node =
					new ASTExpressionAssign((ASTExpressionName*)assignee, expr);
				return node;
			}
			case NodeKind::EXPR_FIELD_GET:
			{
				ASTExpressionFieldGet* expr_field_get = (ASTExpressionFieldGet*)assignee;
				if (expr_field_get->expr->kind != NodeKind::EXPR_NAME)
				{
					error("field set expression does not have an expression name target");
				}
				ASTExpressionName* expr_field_get_expr = (ASTExpressionName*)expr_field_get->expr;
				expr_field_get->expr = nullptr;
				ASTNameSimple* expr_field_get_field = expr_field_get->field;
				expr_field_get->field = nullptr;

				delete expr_field_get;

				ASTExpressionFieldSet* node =
					new ASTExpressionFieldSet(expr_field_get_expr, expr_field_get_field, expr);
				return node;
			}
			default:
			{
				error("invalid assignment target: %s", node_kind_to_string(assignee->kind));
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

ASTDeclaration* Parser::parse_decl()
{
	ASTDeclaration* node = nullptr;

	switch (crt_token.kind)
	{
		case TokenKind::KW_VAR:		return parse_decl_var();
			// case TokenKind::KW_FUN:		return parse_decl_var();
			// case TokenKind::KW_CLASS:	return parse_decl_var();
		default:					return parse_stmt();
	}
}

ASTDeclaration* Parser::parse_stmt()
{
	switch (crt_token.kind)
	{
		// case TokenKind::KW_IF:		return parse_stmt_conditional();
		// case TokenKind::KW_WHILE:	return parse_stmt_while();
		// case TokenKind::KW_FOR:		return parse_stmt_for();
		// case TokenKind::KW_BREAK:	return parse_stmt_break();
		// case TokenKind::KW_CONTINUE:	return parse_stmt_continue();
		// case TokenKind::KW_RETURN:	return parse_stmt_return();
		case TokenKind::LEFT_BRACE:		return parse_stmt_block();
		default:						return parse_stmt_expr();
	}
}

ASTStatement* Parser::parse_stmt_expr()
{
	ASTExpression* expr = parse_expr();
	(void)expect(TokenKind::SEMICOLON);
	ASTStatementExpression* node = new ASTStatementExpression(expr);
	// if (!is_panic)
	// {
	// 	node = new ASTStatementExpression(expr);
	// }
	return node;
}

ASTStatement* Parser::parse_stmt_block()
{
	std::vector<ASTDeclaration*> declarations;
	(void)expect(TokenKind::LEFT_BRACE);

	while (crt_token.kind != TokenKind::RIGHT_BRACE
		&& crt_token.kind != TokenKind::END_OF_FILE)
	{
		ASTDeclaration* declaration = parse_decl();
		declarations.push_back(declaration);
	}

	(void)expect(TokenKind::RIGHT_BRACE);

	ASTStatementBlock* node = new ASTStatementBlock(declarations);
	return node;
}

ASTDeclaration* Parser::parse_decl_var()
{
	(void)expect(TokenKind::KW_VAR);
	Token var_name = expect(TokenKind::IDENTIFIER);
	ASTExpression* expr = nullptr;
	ASTType* type = nullptr;

	switch (crt_token.kind)
	{
		case TokenKind::ASSIGN_DIRECT:
		{
			(void)expect(crt_token.kind);
			expr = parse_expr();
			(void)expect(TokenKind::SEMICOLON);
			break;
		}
		case TokenKind::COLON:
		{
			(void)expect(crt_token.kind);
			type = parse_type();
			if (crt_token.kind == TokenKind::ASSIGN_DIRECT)
			{
				(void)expect(crt_token.kind);
				expr = parse_expr();
			}
			(void)expect(TokenKind::SEMICOLON);
			break;
		}
		default:
		{
			error("unexpected token '%s'", token_kind_to_string(crt_token.kind));
		}
	}
	ASTDeclaration* node = new ASTDeclarationVariable(var_name, type, expr);

	// if (!is_panic)
	// {
	// 	ASTDeclaration* node = new ASTDeclarationVariable(var_name, type, expr);
	// 	return node;
	// }

	return node;
}

ASTDeclaration* Parser::parse_decl_param()
{
	Token var_name = expect(TokenKind::IDENTIFIER);
	(void)expect(TokenKind::COLON);
	ASTType* type = parse_type();
	return nullptr;
}
