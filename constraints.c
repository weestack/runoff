#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliary.h"
#include "symbol.h"
#include "ast.h"
#include "phases.h"

static int errors = 0;
static void checkTreeHasSetup(AstNode *tree);
static void checkSwitchHasDefault(AstNode *tree);
static void checkReceiveHasDefault(AstNode *tree);
static void checkNotGlobalVar(AstNode *tree);
static void checkHasMaxOneMessageBlock(AstNode *tree);
static void checkVarInitialized(AstNode *tree);
static void checkAllSpawnsInSetup(AstNode *prog, AstNode *stmts);
static int countSpawns(AstNode *nodes, int recurse);
static AstNode *getDefaultValue(Type *type);

/* The following list of contextual constraints are checked by
   this phase:
   1) Each tree has one setup function with type setup(void)->void
   2) Each switch has exactly one default case
   3) Each receive has exactly one default case
   4) No modification of global variables
   5) Each tree has at most one messages block
   6) All variables are initialized before use, or have a default value
   7) All spawns appear as a statement directly in the setup function
 */
int contextualConstraintsCheck(AstNode *tree){
	AstNode *tmp;
	switch(tree->tag){
	case Prog:
		checkTreeHasSetup(tree);
		checkHasMaxOneMessageBlock(tree);
		break;
	case Switch:
		checkSwitchHasDefault(tree);
		break;
	case Receive:
		checkReceiveHasDefault(tree);
		break;
	case Assignment:
		checkNotGlobalVar(tree->node.Assignment.location);
		break;
	case UnaryOperation:
		if(tree->node.UnaryOperation.operator == edecrement
			|| tree->node.UnaryOperation.operator == eincrement)
			checkNotGlobalVar(tree->node.UnaryOperation.expression);
		break;
	case VariableLocation:
	case StructLocation:
	case ArrayLocation:
		checkVarInitialized(tree);
		return errors;
	}

	/* we cannot use getChildren here, since cannot work with a copy
	   of the tree, as it is modified later. */
#define CHECKCHILD(tag, field) for(tmp=tree->node.tag.field; tmp!=NULL; tmp=tmp->next) {contextualConstraintsCheck(tmp);}

	switch(tree->tag){
	case Prog:
		CHECKCHILD(Prog, toplevels);
		break;
	case DefineFunction:
		CHECKCHILD(DefineFunction, identifier);
		CHECKCHILD(DefineFunction, parameters);
		CHECKCHILD(DefineFunction, type);
		CHECKCHILD(DefineFunction, statements);
		break;
	case DefineTask:
		CHECKCHILD(DefineTask, identifier);
		CHECKCHILD(DefineTask, parameters);
		CHECKCHILD(DefineTask, statements);
		break;
	case DefineStruct:
		CHECKCHILD(DefineStruct, identifier);
		CHECKCHILD(DefineStruct, fields);
		break;
	case DefineMessage:
		CHECKCHILD(DefineMessage, messagesIdentifiers);
		break;
	case IncludeRunoffFile:
		CHECKCHILD(IncludeRunoffFile, identifier);
		break;
	case MessageIdentifier:
		CHECKCHILD(MessageIdentifier, identifier);
		CHECKCHILD(MessageIdentifier, parameters);
		break;
	case StructMember:
		CHECKCHILD(StructMember, type);
		CHECKCHILD(StructMember, identifier);
		break;
	case Parameter:
		CHECKCHILD(Parameter, type);
		CHECKCHILD(Parameter, identifier);
		break;
	case BuiltinType:
		break;
	case StructType:
		CHECKCHILD(StructType, identifier);
		break;
	case ArrayType:
		CHECKCHILD(ArrayType, type);
		CHECKCHILD(ArrayType, int_literal);
		break;
	case While:
		CHECKCHILD(While, expression);
		CHECKCHILD(While, statements);
		break;
	case For:
		CHECKCHILD(For, expressionInit);
		CHECKCHILD(For, expressionTest);
		CHECKCHILD(For, expressionUpdate);
		CHECKCHILD(For, statements);
		break;
	case Switch:
		CHECKCHILD(Switch, expression);
		CHECKCHILD(Switch, cases);
		break;
	case SwitchCase:
		CHECKCHILD(SwitchCase, literal);
		CHECKCHILD(SwitchCase, statements);
		break;
	case If:
		CHECKCHILD(If, expression);
		CHECKCHILD(If, statements);
		CHECKCHILD(If, elsePart);
		break;
	case ElseIf:
		CHECKCHILD(ElseIf, expression);
		CHECKCHILD(ElseIf, statements);
		CHECKCHILD(ElseIf, elsePart);
		break;
	case Else:
		CHECKCHILD(Else, statements);
		break;
	case Receive:
		CHECKCHILD(Receive, cases);
		break;
	case ReceiveCase:
		CHECKCHILD(ReceiveCase, messageName);
		CHECKCHILD(ReceiveCase, dataNames);
		CHECKCHILD(ReceiveCase, statements);
		break;
	case VarDecl:
		CHECKCHILD(VarDecl, type);
		CHECKCHILD(VarDecl, identifier);
		CHECKCHILD(VarDecl, expression);
		break;
	case BinaryOperation:
		CHECKCHILD(BinaryOperation, expression_left);
		CHECKCHILD(BinaryOperation, expression_right);
		break;
	case VariableLocation:
		CHECKCHILD(VariableLocation, identifier);
		break;
	case StructLocation:
		CHECKCHILD(StructLocation, identifier);
		CHECKCHILD(StructLocation, location);
		break;
	case ArrayLocation:
		CHECKCHILD(ArrayLocation, identifier);
		CHECKCHILD(ArrayLocation, indicies);
		break;
	case UnaryOperation:
		CHECKCHILD(UnaryOperation, expression);
		break;
	case FunctionCall:
		CHECKCHILD(FunctionCall, identifier);
		CHECKCHILD(FunctionCall, arguments);
		break;
	case Assignment:
		CHECKCHILD(Assignment, location);
		CHECKCHILD(Assignment, expression);
		break;
	case TernaryOperator:
		CHECKCHILD(TernaryOperator, expressionTest);
		CHECKCHILD(TernaryOperator, expressionTrue);
		CHECKCHILD(TernaryOperator, expressionFalse);
		break;
	case Identifier:
		break;
	case IntLiteral:
		break;
	case FloatLiteral:
		break;
	case BoolLiteral:
		break;
	case MessageLiteral:
		CHECKCHILD(MessageLiteral, identifier);
		CHECKCHILD(MessageLiteral, arguments);
		break;
	case Return:
		CHECKCHILD(Return, expression);
		break;
	case Spawn:
		CHECKCHILD(Spawn, identifier);
		CHECKCHILD(Spawn, arguments);
		break;
	case Send:
		CHECKCHILD(Send, message);
		CHECKCHILD(Send, receiver);
		break;
	case ExprStmt:
		CHECKCHILD(ExprStmt, expression);
		break;
	}
	return errors;
}

/* Look at all the toplevel declarations and see if a function
   with the signature 'setup() -> void' exists */
static void checkTreeHasSetup(AstNode *tree){
	AstNode *toplevels = tree->node.Prog.toplevels;
	AstNode *node;
	for(node = toplevels; node != NULL; node = node->next){
		Symbol *s;
		FunctionTypeDescriptor t;
		if(node->tag != DefineFunction)
			continue;

		s = node->node.DefineFunction.identifier->node.Identifier.symbol;
		if(strcmp("setup", s->name) != 0)
			continue;

		if(s->type->tag != FunctionTypeTag)
			continue;

		t = s->type->tags.typeFunction;
		if(t.arity == 0
			&& t.returnType->tag == BuiltinTypeTag
			&& t.returnType->tags.typeBuiltin.builtinType == builtintype_void){
			checkAllSpawnsInSetup(tree, node->node.DefineFunction.statements);
			return;
		}
	}
	eprintf(1, "No setup function with type 'setup() -> void' found in program\n");
	errors++;
}

/* go through all cases and check if there is exactly one default case */
static void checkSwitchHasDefault(AstNode *tree){
	AstNode *cases = tree->node.Switch.cases;
	AstNode *node;
	int defaultCount = 0;
	for(node = cases; node != NULL; node = node->next){
		if(node->node.SwitchCase.literal == NULL)
			defaultCount++;
	}

	if(defaultCount != 1){
		errors++;
		eprintf(tree->linenum, "Switch statements must have exactly one default case. This one has %d\n", defaultCount);
	}
}

/* go through all cases and check if there is exactly one default case */
static void checkReceiveHasDefault(AstNode *tree){
	AstNode *cases = tree->node.Receive.cases;
	AstNode *node;
	int defaultCount = 0;
	for(node = cases; node != NULL; node = node->next){
		if(node->node.ReceiveCase.messageName == NULL)
			defaultCount++;
	}

	if(defaultCount != 1){
		errors++;
		eprintf(tree->linenum, "Receive statements must have exactly one default case. This one has %d\n", defaultCount);
	}
}

/* Check if the tree is a location, and if so, check that it isn't global */
static void checkNotGlobalVar(AstNode *tree){
	AstNode *id;
	switch(tree->tag){
	case VariableLocation:
		id = tree->node.VariableLocation.identifier;
		break;
	case StructLocation:
		id = tree->node.StructLocation.identifier;
		break;
	case ArrayLocation:
		id = tree->node.ArrayLocation.identifier;
		break;
	default:
		return;
	}

	if(id->node.Identifier.symbol->globalvar){
		eprintf(tree->linenum, "Cannot modify the global variable '%s'\n", id->node.Identifier.symbol->name);
		errors++;
	}
}

/* Check that there is at most one messages block in the program */
static void checkHasMaxOneMessageBlock(AstNode *tree){
	int msgblockCount = 0;
	AstNode *toplevels = tree->node.Prog.toplevels;
	AstNode *node;
	for(node = toplevels; node != NULL; node = node->next){
		if(node->tag == DefineMessage)
			msgblockCount++;
	}

	if(msgblockCount > 1){
		eprintf(1, "At most one messages block is allowed per program. This program has %d\n", msgblockCount);
		errors++;
	}
}

/* check that the variable is either initialized before use, or have a default value */
static void checkVarInitialized(AstNode *tree){
	AstNode *id;
	Symbol *sym;
	switch(tree->tag){
	case VariableLocation:
		id = tree->node.VariableLocation.identifier;
		break;
	case StructLocation:
		id = tree->node.StructLocation.identifier;
		break;
	case ArrayLocation:
		id = tree->node.ArrayLocation.identifier;
		break;
	default:
		return;
	}

	sym = id->node.Identifier.symbol;
	if(!sym->initialized){
		AstNode *expr = getDefaultValue(sym->type);
		if(expr != NULL){
			/* replace the occurence of variable v with v=e where e is the default value.*/
			AstNode *loc = malloc(sizeof(AstNode));
			memcpy(loc, tree, sizeof(AstNode));
			tree->tag = Assignment;
			tree->node.Assignment.location = loc;
			tree->node.Assignment.expression = expr;
			sym->initialized = 1;
			printf("Replaces '%s' with '%s' on line %d\n", prettyprint(loc), prettyprint(tree), tree->linenum);
		}else{
			eprintf(tree->linenum, "Variable '%s' is not initialized when used here\n", sym->name);
			errors++;
		}
	}
}

static AstNode *getDefaultValue(Type *type){
	if(type->tag != BuiltinTypeTag)
		return NULL;

	switch(type->tags.typeBuiltin.builtinType){
	case builtintype_uint8:
	case builtintype_uint16:
	case builtintype_uint32:
	case builtintype_uint64:
	case builtintype_int8:
	case builtintype_int16:
	case builtintype_int32:
	case builtintype_int64:
		return mkIntLiteralNode(0);
	case builtintype_float:
		return mkFloatLiteralNode(0);
	case builtintype_bool:
		return mkBoolLiteralNode(0);
	default:
		return NULL;
	}
}

static int countSpawns(AstNode *nodes, int recurse){
	AstNode *n;
	int count = 0;
	for(n = nodes; n != NULL; n = n->next){
		if(n->tag == Spawn)
			count++;
		if(recurse || n->tag == ExprStmt || n->tag == VarDecl || n->tag == Assignment)
			count += countSpawns(getChildren(n), recurse);
	}
	return count;
}

static void checkAllSpawnsInSetup(AstNode *prog, AstNode *stmts){
	int spawncountall = countSpawns(prog->node.Prog.toplevels, 1);
	int spawncountstmts = countSpawns(stmts, 0);
	prog->node.Prog.spawnCount = spawncountstmts;
	if(spawncountall > spawncountstmts){
		errors++;
		eprintf(1, "It looks like you have spawns located somewhere else than directly in the setup function, which is not allowed.\n");
	}
}
