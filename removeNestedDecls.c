#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

#define HANDLE_CHILDREN(tag, field) (pushed = append_node(pushed, handleChildren(node->node.tag.field, toplevel)))
#define REPLACE_SUBTREE(tag, subtree, toplevel) (tree->node.tag.subtree = handleList(tree->node.tag.subtree, toplevel))

static AstNode *handleNode(AstNode *, int, int);
static AstNode *handleList(AstNode *, int);
static AstNode *handleChildren(AstNode *, int);

/* This function changes replaces blocks with fixed blocks. Should include a case for each node that defines a new scope/block */
void removeNestedDecls(AstNode *tree){
	switch(tree->tag){
	case Prog:
		REPLACE_SUBTREE(Prog, toplevels, 1);
		break;
	case DefineFunction:
		REPLACE_SUBTREE(DefineFunction, statements, 0);
		break;
	case DefineTask:
		REPLACE_SUBTREE(DefineTask, statements, 0);
		break;
	case While:
		REPLACE_SUBTREE(While, statements, 0);
		break;
	case SwitchCase:
		REPLACE_SUBTREE(SwitchCase, statements, 0);
		break;
	case If:
		REPLACE_SUBTREE(If, statements, 0);
		REPLACE_SUBTREE(If, elsePart, 0);
		break;
	case ElseIf:
		REPLACE_SUBTREE(ElseIf, statements, 0);
		REPLACE_SUBTREE(ElseIf, elsePart, 0);
		break;
	case Else:
		REPLACE_SUBTREE(Else, statements, 0);
		break;
	case ReceiveCase:
		REPLACE_SUBTREE(ReceiveCase, statements, 0);
		break;
	}
}


/* This function loops over each child in the children list, and if it is
   a variable declaration, it creates a new declaration and changes the old
   to a variableLocation. Returns the newly created declaration if any.
   It also calls handleNode on each of the children in the list, to
   traverse the whole tree. */
static AstNode *handleChildren(AstNode *children, int toplevel){
	AstNode *tmp;
	AstNode *pushed = NULL;
	AstNode *tmpres = NULL;

	for(tmp = children; tmp != NULL; tmp = tmp->next){
		tmpres = handleNode(tmp, toplevel, 0);
		if(tmp->tag == VarDecl){
			AstNode *res = malloc(sizeof(AstNode));
			memcpy(res, tmp, sizeof(AstNode));
			res->next = NULL;
			res->node.VarDecl.type = malloc(sizeof(AstNode));
			memcpy(res->node.VarDecl.type, tmp->node.VarDecl.type, sizeof(AstNode));
			res->node.VarDecl.identifier = malloc(sizeof(AstNode));
			memcpy(res->node.VarDecl.identifier, tmp->node.VarDecl.identifier, sizeof(AstNode));
			if(res->node.VarDecl.expression){
				res->node.VarDecl.expression = malloc(sizeof(AstNode));
				memcpy(res->node.VarDecl.expression, tmp->node.VarDecl.expression, sizeof(AstNode));
			}
			res->node.VarDecl.toplevel = toplevel;
		
			tmp->tag = VariableLocation;
			tmp->node.VariableLocation.identifier = tmp->node.VarDecl.identifier;
			pushed = append_node(pushed, res);
		}
		pushed = append_node(tmpres, pushed);
	}
	return pushed;
}

/* This function returns runs handleChildren on sub parts of the node itself, which may have issues. Returns the pushed out code,
   and modifies the tree in-place. */
static AstNode *handleNode(AstNode *node, int toplevel, int wrap){
	AstNode *pushed = NULL;
	switch(node->tag){
	case Prog:
		HANDLE_CHILDREN(Prog, toplevels);
		break;
	case While:
		HANDLE_CHILDREN(While, expression);
		break;
	case For:
		HANDLE_CHILDREN(For, expressionInit);
		HANDLE_CHILDREN(For, expressionTest);
		HANDLE_CHILDREN(For, expressionUpdate);
		break;
	case Switch:
		HANDLE_CHILDREN(Switch, expression);
		break;
	case If:
		HANDLE_CHILDREN(If, expression);
		break;
	case VarDecl:
		HANDLE_CHILDREN(VarDecl, expression);
		break;
	case BinaryOperation:
		HANDLE_CHILDREN(BinaryOperation, expression_left);
		HANDLE_CHILDREN(BinaryOperation, expression_right);
		break;
	case UnaryOperation:
		HANDLE_CHILDREN(UnaryOperation, expression);
		break;
	case ArrayLocation:
		HANDLE_CHILDREN(ArrayLocation, indicies);
		break;
	case FunctionCall:
		HANDLE_CHILDREN(FunctionCall, arguments);
		break;
	case Assignment:
		HANDLE_CHILDREN(Assignment, expression);
		break;
	case TernaryOperator:
		HANDLE_CHILDREN(TernaryOperator, expressionTest);
		HANDLE_CHILDREN(TernaryOperator, expressionTrue);
		HANDLE_CHILDREN(TernaryOperator, expressionFalse);
		break;
	case MessageLiteral:
		HANDLE_CHILDREN(MessageLiteral, arguments);
		break;
	case Return:
		HANDLE_CHILDREN(Return, expression);
		break;
	case Spawn:
		HANDLE_CHILDREN(Spawn, arguments);
		break;
	case Send:
		HANDLE_CHILDREN(Send, message);
		HANDLE_CHILDREN(Send, receiver);
		break;
	case ExprStmt:
		pushed = handleNode(node->node.ExprStmt.expression, toplevel, 0);
		break;
	}
	
	/* Be sure any sub-blocks are fixed */
	removeNestedDecls(node);

	if(wrap){
		AstNode *prev = NULL;
		AstNode *tmp;
		for(tmp = pushed; tmp != NULL; tmp = tmp->next){
			AstNode *next = tmp->next;
			tmp = mkExprStmtNode(tmp);
			tmp->next = next;
			if(prev)
				prev->next = tmp;
			else
				pushed = tmp;
			prev = tmp;
		}
	}
	return pushed;
}

/* This function runs handleNode on each node in the list, and inserts the pushed out declarations if any. */
static AstNode *handleList(AstNode *list, int toplevel){
	AstNode *result = NULL;
	AstNode *tmp;
	AstNode *n;
	AstNode *pushed;
	n = list;
	while(n != NULL){
		pushed = handleNode(n, toplevel, !toplevel);
		tmp = n;
		n = n->next;
		tmp->next = NULL;
		if(pushed)
			result = append_node(result, pushed);
		result = append_node(result, tmp);
	}
	return result;
}
