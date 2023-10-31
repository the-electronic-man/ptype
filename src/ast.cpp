#pragma once
#include "ast.h"


ASTNode::ASTNode(NodeKind kind)
{
	this->kind = kind;
}

ASTExpression::ASTExpression(NodeKind kind) : ASTNode(kind) {}
ASTExpression::~ASTExpression()
{
	delete type;
}

ASTStatement::ASTStatement(NodeKind kind) : ASTNode(kind) {}
ASTDeclaration::ASTDeclaration(NodeKind kind) : ASTStatement(kind) {}
ASTName::ASTName(NodeKind kind) : ASTNode(kind) {}



ASTNameSimple::ASTNameSimple(Token token)
	: ASTName(NodeKind::NAME_SIMPLE)
{
	this->token = token;
}

ASTNameSimple::~ASTNameSimple() {}

ASTNode* ASTNameSimple::clone()
{
	return new ASTNameSimple(token);
}




ASTType::ASTType(PrimitiveType primitive_type) : ASTNode(NodeKind::TYPE)
{
	this->primitive_type = primitive_type;
}

ASTType::ASTType(ASTType* element_type) : ASTNode(NodeKind::TYPE)
{
	this->primitive_type = PrimitiveType::T_ARRAY;
	this->element_type = element_type;
}

ASTType::ASTType(ASTName* reference_name) : ASTNode(NodeKind::TYPE)
{
	this->primitive_type = PrimitiveType::T_REF;
	this->reference_name = reference_name;
}

ASTType::~ASTType()
{
	switch (primitive_type)
	{
		case PrimitiveType::T_VOID:
		case PrimitiveType::T_BOOL:
		case PrimitiveType::T_CHAR:
		case PrimitiveType::T_INT:
		case PrimitiveType::T_FLOAT:
		{
			break;
		}
		case PrimitiveType::T_REF:
		{
			delete reference_name;
			break;
		}
		case PrimitiveType::T_ARRAY:
		{
			delete element_type;
			break;
		}
		default:
		{
			break;
		}
	}
}

void ASTType::accept(Visitor* visitor)
{
	visitor->visit(this);
}

ASTNode* ASTType::clone()
{
	switch (primitive_type)
	{
		case PrimitiveType::T_VOID:
		case PrimitiveType::T_BOOL:
		case PrimitiveType::T_CHAR:
		case PrimitiveType::T_INT:
		case PrimitiveType::T_FLOAT:
		{
			return new ASTType(primitive_type);
		}
		case PrimitiveType::T_ARRAY:
		{
			return new ASTType((ASTType*)element_type->clone());
		}
		case PrimitiveType::T_REF:
		{
			return new ASTType((ASTName*)reference_name->clone());
		}
		default:
		{
			return nullptr;
		}
	}
}




ASTNameQualified::ASTNameQualified(ASTName* qualifier, ASTNameSimple* name)
	: ASTName(NodeKind::NAME_QUALIFIED)
{
	this->name = name;
	this->qualifier = qualifier;
}

ASTNameQualified::~ASTNameQualified()
{
	delete name;
	delete qualifier;
}

ASTNode* ASTNameQualified::clone()
{
	return 
		new ASTNameQualified
		(
			(ASTName*)qualifier->clone(),
			(ASTNameSimple*)name->clone()
		);
}





ASTExpressionLiteral::ASTExpressionLiteral(Token token)
	: ASTExpression(NodeKind::EXPR_LITERAL)
{
	this->token = token;
}

ASTExpressionLiteral::~ASTExpressionLiteral() {}

ASTNode* ASTExpressionLiteral::clone()
{
	return new ASTExpressionLiteral(token);
}




ASTExpressionUnary::ASTExpressionUnary(Token op, ASTExpression* expr)
	: ASTExpression(NodeKind::EXPR_UNARY)
{
	this->op = op;
	this->expr = expr;
}

ASTExpressionUnary::~ASTExpressionUnary()
{
	delete expr;
}

ASTNode* ASTExpressionUnary::clone()
{
	ASTExpressionUnary* node = 
		new ASTExpressionUnary
		(
			op, 
			(ASTExpression*)expr->clone()
		);
	return node;
}





ASTExpressionBinary::ASTExpressionBinary(Token op, ASTExpression* left, ASTExpression* right)
	: ASTExpression(NodeKind::EXPR_BINARY)
{
	this->op = op;
	this->left = left;
	this->right = right;
}

ASTExpressionBinary::~ASTExpressionBinary()
{
	delete left;
	delete right;
}

ASTNode* ASTExpressionBinary::clone()
{
	ASTExpressionBinary* node =
		new ASTExpressionBinary
		(
			op,
			(ASTExpression*)left->clone(),
			(ASTExpression*)right->clone()
		);
	return node;
}




ASTExpressionGroup::ASTExpressionGroup(ASTExpression* expr)
	: ASTExpression(NodeKind::EXPR_GROUP)
{
	this->expr = expr;
}

ASTExpressionGroup::~ASTExpressionGroup()
{
	delete expr;
}

ASTNode* ASTExpressionGroup::clone()
{
	ASTExpressionGroup* node =
		new ASTExpressionGroup((ASTExpression*)expr->clone());
	return node;
}






ASTExpressionCast::ASTExpressionCast(ASTExpression* expr, ASTType* dst_type)
	: ASTExpression(NodeKind::EXPR_CAST)
{
	this->expr = expr;
	this->dst_type = dst_type;
}

ASTExpressionCast::~ASTExpressionCast()
{
	delete expr;
	delete dst_type;
}

ASTNode* ASTExpressionCast::clone()
{
	ASTExpressionCast* node =
		new ASTExpressionCast
		(
			(ASTExpression*)expr->clone(),
			(ASTType*)dst_type->clone()
		);
	return node;
}



ASTExpressionAssign::ASTExpressionAssign(ASTExpression* assignee, ASTExpression* expr)
	: ASTExpression(NodeKind::EXPR_ASSIGN)
{
	this->assignee = assignee;
	this->expr = expr;
}

ASTExpressionAssign::~ASTExpressionAssign()
{
	delete assignee;
	delete expr;
}

ASTNode* ASTExpressionAssign::clone()
{
	return new ASTExpressionAssign
	(
		(ASTExpression*)assignee->clone(),
		(ASTExpression*)expr->clone()
	);
}





ASTExpressionName::ASTExpressionName(ASTName* name)
	: ASTExpression(NodeKind::EXPR_NAME)
{
	this->name = name;
}

ASTExpressionName::~ASTExpressionName()
{
	delete name;
}

ASTNode* ASTExpressionName::clone()
{
	return new ASTExpressionName((ASTName*)name->clone());
}




ASTStatementBlock::ASTStatementBlock(std::vector<ASTStatement*>& statements)
	: ASTStatement(NodeKind::STMT_BLOCK)
{
	this->scope = new Scope(Scope::ScopeKind::Block);
	this->statements = statements;
}

ASTStatementBlock::~ASTStatementBlock()
{
	delete scope;
	for (size_t i = 0; i < statements.size(); i++)
	{
		delete statements[i];
	}
}

ASTNode* ASTStatementBlock::clone()
{
	return nullptr;
}




ASTStatementExpression::ASTStatementExpression(ASTExpression* expr)
	: ASTStatement(NodeKind::STMT_EXPR)
{
	this->expr = expr;
}

ASTStatementExpression::~ASTStatementExpression()
{
	delete expr;
}

ASTNode* ASTStatementExpression::clone()
{
	return new ASTStatementExpression((ASTExpression*)expr->clone());
}




ASTDeclarationVariable::ASTDeclarationVariable(Token name, ASTType* type, ASTExpression* expr)
	: ASTDeclaration(NodeKind::DECL_VAR)
{
	this->name = name;
	this->type = type;
	this->expr = expr;
}

ASTDeclarationVariable::~ASTDeclarationVariable()
{
	// delete name;
	delete type;
	delete expr;
}

ASTNode* ASTDeclarationVariable::clone()
{
	ASTDeclarationVariable* node =
		new ASTDeclarationVariable
		(
			name,
			(ASTType*)type->clone(),
			(ASTExpression*)expr->clone()
		);
	return node;
}








