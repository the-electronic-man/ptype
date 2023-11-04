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
		pt_error("%s [%d:%d]", msg, crt_token.line, crt_token.column);
	}

	return last_token;
}

ASTNode* Parser::parse(Token* token_list, size_t token_count)
{
	this->token_list = token_list;
	this->token_count = token_count;
	this->token_index = 0;
	this->crt_token = token_list[token_index];

	std::vector<ASTStatement*> statements;
	while (crt_token.kind != TokenKind::END_OF_FILE)
	{
		ASTStatement* statement = parse_stmt();
		statements.push_back(statement);
	}
	ASTStatementBlock* node = new ASTStatementBlock(statements);
	return node;
}

ASTType* Parser::parse_type()
{
	PrimitiveType primitive_type;
	if (!is_string_a_primitive_type(primitive_type, crt_token.buffer))
	{
		pt_log("%s is not a primitive type", crt_token.buffer.data());
		return nullptr;
	}
	(void)expect(TokenKind::IDENTIFIER);
	ASTTypePrimitive* node = new ASTTypePrimitive(primitive_type);
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
		case TokenKind::LEFT_PAREN:
		{
			(void)expect(TokenKind::LEFT_PAREN);
			ASTExpression* expr = new ASTExpressionGroup(parse_expr());
			(void)expect(TokenKind::RIGHT_PAREN);
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

ASTExpression* Parser::parse_expr_cast()
{
	if (crt_token.kind == TokenKind::KW_CAST)
	{
		(void)expect(TokenKind::KW_CAST);
		(void)expect(TokenKind::LEFT_PAREN, "expected '(' for cast expression");
		ASTType* type = parse_type();
		(void)expect(TokenKind::COMMA, "expected ',' after type");
		ASTExpression* expr = parse_expr();
		(void)expect(TokenKind::RIGHT_PAREN, "expected ')' after cast expression");
		return new ASTExpressionCast(expr, type);
	}
	else
	{
		return parse_expr_unary();
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
				(void)expect(crt_token.kind);
				Token field_name = expect(TokenKind::IDENTIFIER, "expected identifier as field name");
				ASTNameSimple* field = new ASTNameSimple(field_name);
				if (crt_token.kind == TokenKind::ASSIGN_DIRECT)
				{
					(void)expect(crt_token.kind);
					ASTExpression* value = parse_expr_assignment();
					node = new ASTExpressionFieldSet(node, field, value);
				}
				else
				{
					node = new ASTExpressionFieldGet(node, field);
				}
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
			ASTExpression* expr = parse_expr_cast();
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
parse_function(parse_expr_equality, parse_expr_relational, TokenKind::EQUAL, TokenKind::NOT_EQUAL)
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

		if (assignee->kind == NodeKind::EXPR_NAME)
		{
			ASTExpressionAssign* node = new ASTExpressionAssign(assignee, expr);
			return node;
		}
		else
		{
			pt_error("invalid assignment target");
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
		case TokenKind::KW_VAR:		return parse_decl_var();
		case TokenKind::LEFT_BRACE:	return parse_stmt_block();
		default:					return parse_stmt_expr();
	}
}

ASTStatement* Parser::parse_stmt_expr()
{
	ASTExpression* expr = parse_expr();
	ASTStatementExpression* node = new ASTStatementExpression(expr);
	(void)expect(TokenKind::SEMICOLON, "expected ';' after expression statement");
	return node;
}

ASTStatement* Parser::parse_stmt_block()
{
	std::vector<ASTStatement*> statements;
	(void)expect(TokenKind::LEFT_BRACE);

	while (crt_token.kind != TokenKind::RIGHT_BRACE
		&& crt_token.kind != TokenKind::END_OF_FILE)
	{
		ASTStatement* statement = parse_stmt();
		statements.push_back(statement);
	}

	(void)expect(TokenKind::RIGHT_BRACE, "expected '}' after block statement");

	ASTStatementBlock* node = new ASTStatementBlock(statements);
	return node;
}

ASTDeclaration* Parser::parse_decl_var()
{
	// ----------------------------------------------------------------
	// var <name> ('=' <expr> | ':' <type> ('=' <expr>)? )
	// 
	// var int a = 3, b = 5;
	// var k = 4, j = 5;
	// 
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
			(void)expect(TokenKind::SEMICOLON, "expected ';' after variable declaration");
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
			(void)expect(TokenKind::SEMICOLON, "expected ';' after variable declaration");
			break;
		}
		default:
		{
			pt_error("unexpected token '%s'", token_kind_to_string(crt_token.kind));
		}
	}

	ASTDeclaration* node = new ASTDeclarationVariable(new ASTNameSimple(var_name), type, expr);
	return node;
}

ASTDeclaration* Parser::parse_decl_param()
{
	Token var_name = expect(TokenKind::IDENTIFIER, "expected variable name");
	(void)expect(TokenKind::COLON, "expected ':' after parameter name");
	ASTType* type = parse_type();
	return nullptr;
}
