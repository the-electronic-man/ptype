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
ASTType::ASTType(NodeKind kind) : ASTNode(kind) {}
ASTName::ASTName(NodeKind kind) : ASTNode(kind) {}



ASTNameSimple::ASTNameSimple(Token token)
	: ASTName(NodeKind::NAME_SIMPLE)
{
	this->token = token;
}

ASTNameSimple::~ASTNameSimple() {}

void ASTNameSimple::accept(Visitor* visitor)
{
	visitor->visit(this);
}

ASTNode* ASTNameSimple::clone()
{
	return new ASTNameSimple(token);
}




ASTTypePrimitive::ASTTypePrimitive(PrimitiveType primitive_type)
	: ASTType(NodeKind::TYPE_PRIMITIVE)
{
	this->primitive_type = primitive_type;
}
ASTTypePrimitive::~ASTTypePrimitive() {}

void ASTTypePrimitive::accept(Visitor* visitor)
{
	visitor->visit(this);
}

ASTNode* ASTTypePrimitive::clone()
{
	return new ASTTypePrimitive(primitive_type);
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

void ASTNameQualified::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTExpressionLiteral::accept(Visitor* visitor)
{
	visitor->visit(this);
}

ASTNode* ASTExpressionLiteral::clone()
{
	ASTExpressionLiteral* node = new ASTExpressionLiteral(token);
	return node;
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

void ASTExpressionUnary::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTExpressionBinary::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTExpressionGroup::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTExpressionCast::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTExpressionAssign::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTExpressionName::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTStatementBlock::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTStatementExpression::accept(Visitor* visitor)
{
	visitor->visit(this);
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

void ASTDeclarationVariable::accept(Visitor* visitor)
{
	visitor->visit(this);
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








