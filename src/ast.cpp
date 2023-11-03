#include "ast.h"

#define ENUM_ITEM(name, str) \
	case NodeKind::name: return str;

const char* node_kind_to_string(NodeKind kind)
{
	switch (kind)
	{
#include "ast_kind.def"
		default: return "undefined";
	}
}

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





ASTNode* ASTTypeReference::clone()
{
	return new ASTTypeReference((ASTName*)name->clone());
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





ASTExpressionFieldGet::ASTExpressionFieldGet(ASTExpression* expr, ASTNameSimple* field)
	: ASTExpression(NodeKind::EXPR_FIELD_GET)
{
	this->expr = expr;
	this->field = field;
}

ASTExpressionFieldGet::~ASTExpressionFieldGet()
{
	delete expr;
}

ASTNode* ASTExpressionFieldGet::clone()
{
	return new ASTExpressionFieldGet((ASTExpression*)expr->clone(), (ASTNameSimple*)field->clone());
}







ASTExpressionFieldSet::ASTExpressionFieldSet(ASTExpression* expr, ASTNameSimple* field, ASTExpression* value)
	: ASTExpression(NodeKind::EXPR_FIELD_SET)
{
	this->expr = expr;
	this->field = field;
	this->value = value;
}

ASTExpressionFieldSet::~ASTExpressionFieldSet()
{
	delete expr;
	delete value;
}

ASTNode* ASTExpressionFieldSet::clone()
{
	return new ASTExpressionFieldSet((ASTExpression*)expr->clone(), (ASTNameSimple*)field->clone(), (ASTExpression*)value->clone());
}





ASTExpressionArrayGet::ASTExpressionArrayGet(ASTExpression* expr, ASTExpression* index)
	: ASTExpression(NodeKind::EXPR_ARRAY_GET)
{
	this->expr = expr;
	this->index = index;
}

ASTExpressionArrayGet::~ASTExpressionArrayGet()
{
	delete expr;
	delete index;
}

ASTNode* ASTExpressionArrayGet::clone()
{
	return new ASTExpressionArrayGet((ASTExpression*)expr->clone(), (ASTExpression*)index->clone());
}





ASTExpressionArraySet::ASTExpressionArraySet(ASTExpression* expr, ASTExpression* index, ASTExpression* value)
	: ASTExpression(NodeKind::EXPR_ARRAY_SET)
{
	this->expr = expr;
	this->index = index;
	this->value = value;
}

ASTExpressionArraySet::~ASTExpressionArraySet()
{
	delete expr;
	delete index;
	delete value;
}

ASTNode* ASTExpressionArraySet::clone()
{
	return new ASTExpressionArraySet((ASTExpression*)expr->clone(), (ASTExpression*)index->clone(), (ASTExpression*)value->clone());
}






ASTStatementBlock::ASTStatementBlock(std::vector<ASTStatement*>& statements)
	: ASTStatement(NodeKind::STMT_BLOCK)
{
	this->scope = nullptr;
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








