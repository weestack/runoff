#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "auxiliary.h"

char *processBlock(AstNode *, char *, int);
char *buildArrayDeclIndices(AstNode *);
char *generateParametersFromStructFields(AstNode *);
char *generatePassByValue(AstNode *);
char *constructMessageEnum(AstNode *);
char *constructMessageStruct(AstNode *);
char *constructMessageUnionStruct(AstNode *);
char *mkStructsFromSpawns(AstNode *);
void changeParamNames(AstNode *);
char *assignParamsToStruct(AstNode *);
char *generateReceiveCaseData(AstNode *);

/* Must be the same order as the BuiltinTypes enum */
char *builtinTypeNames[] = {
	"byte",
	"unsigned int",
	"unsigned long",
	"uint64_t",
	"char",
	"int",
	"long",
	"int64_t",
	"int",
	"float",
	"void",
	"bool",
	"int",
	"int",
	"char",
};

char *codegen(AstNode *tree) {
	char *id = NULL;
	char *type = NULL;
	char *params = NULL;
	char *stmts = NULL;
	char *intlit = NULL;
	char *expr = NULL;
	char *elsepart = NULL;
	char *location = NULL;
	char *indicies = NULL;
	char *extraCode = NULL;
	char *preCodeGen = NULL;

	char *result = NULL;

	if (tree == NULL) return smprintf("");

	switch (tree->tag) {
		case Prog:
			/* Special case include helper functions! */
			preCodeGen = smprintf("%s\n#define numberOfSpawns %d\nTaskHandle_t handlers[numberOfSpawns];\n/*End of preCodeGen*/\n", readFile("arduino_helpers.ino"), tree->node.Prog.spawnCount);
			params = processBlock(tree->node.Prog.toplevels, "\n", 0);
			result = smprintf("%s\nQueueHandle_t Mailbox[%d];\n%s\n", preCodeGen, tree->node.Prog.spawnCount,params);
			break;
		case DefineFunction:
			id = codegen(tree->node.DefineFunction.identifier);
			intlit = codegen(tree->node.DefineFunction.identifier);
			if(strcmp(intlit, "setup") == 0)
				preCodeGen = mkStructsFromSpawns(tree->node.DefineFunction.statements);
			else 
				preCodeGen = smprintf("");
			type = codegen(tree->node.DefineFunction.type);
			params = processBlock(tree->node.DefineFunction.parameters, ", ", 0);
			extraCode = generatePassByValue( tree->node.DefineFunction.parameters );
			stmts = processBlock(tree->node.DefineFunction.statements, "\n", 1);
			result = smprintf("%s%s %s(%s) {%s %s}", preCodeGen, type, id, params, extraCode, stmts);
			break;
		case DefineTask:
			changeParamNames(tree->node.DefineTask.parameters);
			id = codegen(tree->node.DefineTask.identifier);
			params = processBlock(tree->node.DefineTask.parameters, ";\n", 1);
			expr = generateParametersFromStructFields(tree->node.DefineTask.parameters);
			extraCode = smprintf("struct %s *struct_args = (struct %s *)args;\n%s",
				id, id, expr
			);
			stmts = processBlock(tree->node.DefineTask.statements, "\n", 1);
			result = smprintf("struct %s{char self;%s};\nvoid %s(void *args) {%s %s}", 
				id, params, id, extraCode, stmts
			);
			break;
		case DefineStruct:
			result = smprintf(
				"struct %s {%s};",
				codegen(tree->node.DefineStruct.identifier),
				processBlock(tree->node.DefineStruct.fields, "", 0)
			);
			break;
		case DefineMessage:
			extraCode = constructMessageEnum(tree->node.DefineMessage.messagesIdentifiers);
			preCodeGen = constructMessageStruct(tree->node.DefineMessage.messagesIdentifiers);
			intlit = constructMessageUnionStruct(tree->node.DefineMessage.messagesIdentifiers);
			result = smprintf("enum messages{%s};\n%s\n%s",
							extraCode,
							preCodeGen,
							intlit
							);
			break;
		case StructMember:
			type = codegen(tree->node.StructMember.type);
			id = codegen(tree->node.StructMember.identifier);
			result = smprintf(
				"%s %s;",
				type,
				id
			);
			break;
		case Parameter:
			type = codegen(tree->node.Parameter.type);

			if (tree->node.Parameter.type->tag == ArrayType) {
				preCodeGen = codegen(tree->node.Parameter.identifier);
				id = smprintf("%s_original",preCodeGen);
			}else {
				id = codegen(tree->node.Parameter.identifier);
			}
			intlit = tree->node.Parameter.type->tag == ArrayType ? buildArrayDeclIndices(tree->node.Parameter.type) : smprintf("");
			result = smprintf("%s %s%s", type, id, intlit);
			break;
		case ElseIf:
			expr = codegen(tree->node.ElseIf.expression);
			stmts = processBlock(tree->node.ElseIf.statements, "", 1);
			elsepart = codegen(tree->node.ElseIf.elsePart);
			result = smprintf("else if (%s) {%s} %s", expr, stmts, elsepart);
			break;
		case If:
			expr = codegen(tree->node.If.expression);
			stmts = processBlock(tree->node.If.statements, "", 1);
			elsepart = codegen(tree->node.If.elsePart);
			result = smprintf("if (%s) {%s} %s", expr, stmts, elsepart);
			break;
		case Else:
			stmts = processBlock(tree->node.Else.statements, "", 1);
			result = smprintf("else {%s}", stmts);
			break;
		case Receive:
			stmts = processBlock(tree->node.Receive.cases, ";", 0);
			result = smprintf("struct Message m;\nxQueueReceive(Mailbox[self], &m, portMAX_DELAY);\nswitch(m.Tag){%s}", stmts);
			break;
		case ReceiveCase:
			stmts = codegen(tree->node.ReceiveCase.statements);
			id = codegen(tree->node.ReceiveCase.messageName);
			if(tree->node.ReceiveCase.messageName == NULL){
				result = smprintf("default: if(1){%s}\nbreak;", stmts);
			} else {
				if(tree->node.ReceiveCase.dataNames == NULL){
					result = smprintf("case %s: if(1){%s}\nbreak;", id, stmts);	
				} else {
					extraCode = generateReceiveCaseData(tree);
					result = smprintf("case %s: if(1){%s%s}\nbreak;", id, extraCode, stmts); 
				}
			}
			break;
		case Spawn:
			id = codegen(tree->node.Spawn.identifier);	
			params = assignParamsToStruct(tree);			
			result = smprintf("%s\nrunoff_createTask(%s, (void *)&%s_%d)", 
				params,	id, id, tree->node.Spawn.taskId
			);
			break;
		case BuiltinType:
			result = smprintf("%s", builtinTypeNames[tree->node.BuiltinType.type]);
			break;
		case StructType:
			id = codegen(tree->node.StructType.identifier);
			result = smprintf("struct %s", id);
			break;
		case ArrayType:
			type = codegen(tree->node.ArrayType.type);
			result = smprintf("%s", type);
			break;
		case While:
			expr = codegen(tree->node.While.expression);
			stmts = processBlock(tree->node.While.statements, "", 0);
			result = smprintf("while(%s) {%s}", expr, stmts);
		 	break;
		case For:
			type = codegen(tree->node.For.expressionInit);
			id = codegen(tree->node.For.expressionTest);
			expr = codegen(tree->node.For.expressionUpdate);
			preCodeGen = processBlock(tree->node.For.statements, "", 0);
			result = smprintf(
				"for(%s;%s;%s){%s}",
				type, id, expr, preCodeGen
			);
			break;
		case Switch:
			expr = codegen(tree->node.Switch.expression);
			stmts = processBlock(tree->node.Switch.cases, "", 0);
			result = smprintf("switch(%s){%s}",
				expr, stmts
			);
			break;
		case SwitchCase:
			stmts = processBlock(tree->node.SwitchCase.statements, "", 0);
			intlit = codegen(tree->node.SwitchCase.literal);
			if (tree->node.SwitchCase.literal != NULL) {
				result = smprintf("case %s: if(1){%s} break;",
					intlit, stmts
				);
			}else {
				result = smprintf("default: if(1){%s} break;", stmts);
			}
		break;
		case Identifier:
			result = smprintf("%s", tree->node.Identifier.symbol->name);
			break;
		case ExprStmt:
			expr = codegen(tree->node.ExprStmt.expression);
			result = smprintf("%s;", expr);
			break;
		case Assignment:
			if(tree->node.Assignment.expression != NULL && tree->node.Assignment.expression->tag == Spawn){
				id = codegen(tree->node.Assignment.location);
				expr = codegen(tree->node.Assignment.expression);
				result = smprintf("%s = %d; %s", id, 
					tree->node.Assignment.expression->node.Spawn.taskId, expr
				);
			} else {
				id = codegen(tree->node.Assignment.location);
				expr = processBlock(tree->node.Assignment.expression, "", 0);
				result = smprintf("(%s = %s)", id, expr);
			}
			break;
		case TernaryOperator:
			expr = codegen(tree->node.TernaryOperator.expressionTest);
			stmts = codegen(tree->node.TernaryOperator.expressionTrue);
			elsepart = codegen(tree->node.TernaryOperator.expressionFalse);
			result = smprintf("(%s) ? %s: %s",
				expr, stmts, elsepart
			);
			break;
		case VariableLocation:
			result = codegen(tree->node.VariableLocation.identifier);
			break;
		case StructLocation:
			id = codegen(tree->node.StructLocation.identifier);
			location = codegen(tree->node.StructLocation.location);
			result = smprintf("%s.%s", id, location);
			break;
		case ArrayLocation:
			id = codegen(tree->node.ArrayLocation.identifier);
			indicies = processBlock(tree->node.ArrayLocation.indicies, "][", 0);
			result = smprintf("%s[%s]", id, indicies);
			break;
		case UnaryOperation:
			expr = codegen(tree->node.UnaryOperation.expression);
			if (tree->node.UnaryOperation.fix == Postfix) {
				result= smprintf("%s%s", expr, operatorNames[tree->node.UnaryOperation.operator]);
			}else {
				result= smprintf("%s%s", operatorNames[tree->node.UnaryOperation.operator], expr);
			}
			break;
		case Return:
			expr = codegen(tree->node.Return.expression);
			result = smprintf("return %s;", expr);
			break;
		case FunctionCall:
			id = codegen(tree->node.FunctionCall.identifier);
			params = processBlock(tree->node.FunctionCall.arguments, ",", 0);
			result = smprintf("%s(%s)", id, params);
			break;
		case VarDecl:
			if(tree->node.VarDecl.expression != NULL && tree->node.VarDecl.expression->tag == Spawn){
				id = codegen(tree->node.VarDecl.identifier);
				expr = codegen(tree->node.VarDecl.expression);
				result = smprintf("char %s = %d;%s", id, 
					tree->node.VarDecl.expression->node.Spawn.taskId, 
					expr
				);
			} else {
				type = smprintf("%s%s", tree->node.VarDecl.toplevel == 1 ? "const " : "", codegen(tree->node.VarDecl.type));
				id = codegen(tree->node.VarDecl.identifier);
				intlit = tree->node.VarDecl.type->tag == ArrayType ? buildArrayDeclIndices(tree->node.VarDecl.type) : smprintf("");
				expr = tree->node.VarDecl.expression != NULL ? smprintf(" = %s", codegen(tree->node.VarDecl.expression)) : smprintf("");
				result = smprintf("%s %s%s%s%s", type, id, intlit, expr, tree->node.VarDecl.toplevel == 1 ? ";" : "");
			}
			break;
		case BinaryOperation:
			expr = codegen(tree->node.BinaryOperation.expression_left);
			type = codegen(tree->node.BinaryOperation.expression_right);
			result = smprintf("(%s %s %s)",
			expr, operatorNames[tree->node.BinaryOperation.operator],type);
			break;
		case IntLiteral:
			result = smprintf("%d", tree->node.IntLiteral.value);
			break;
		case PinLiteral:
			result = smprintf("%d", tree->node.PinLiteral.value);
			break;
		case FloatLiteral:
			result = smprintf("%lf", tree->node.FloatLiteral.value);
			break;
		case BoolLiteral:
			result = smprintf("%s", tree->node.BoolLiteral.value ? "true" : "false");
			break;
		default:
			result = smprintf("");
	}

	free(id);
	free(type);
	free(params);
	free(stmts);
	free(intlit);
	free(expr);
	free(elsepart);
	free(location);
	free(indicies);
	free(extraCode);
	free(preCodeGen);

	return result;
}

char *buildArrayDeclIndices(AstNode *node) {
	char *buffer = smprintf("[%s]", codegen(node->node.ArrayType.int_literal));
	while (node->node.ArrayType.type->tag == ArrayType) {
		char *tmp = buffer;
		node = node->node.ArrayType.type;
		buffer = smprintf("[%s]%s", codegen(node->node.ArrayType.int_literal), buffer);
		free(tmp);
	}
	return buffer;
}
/* prettyprinterlist copy :joyd: */
char *processBlock(AstNode *node, char *sep, int end){
	char *result;
	char *prev;
	char *childstr;
	AstNode *child;

	if(node == NULL)
		return smprintf("");

	child = node->next;
	result = codegen(node);

	while(child != NULL){
		prev = result;
		childstr = codegen(child);
		result = smprintf("%s%s%s", prev, sep, childstr);
		free(prev);
		free(childstr);
		child = child->next;
	}
	if(end){
		prev = result;
		result = smprintf("%s%s", prev, sep);
		free(prev);
	}
	return result;
}

char *generateParametersFromStructFields(AstNode *tree){
	AstNode *child = tree;
	char *result;
	char *old;
	char *id;
	char *type;
	char *struct_id;
	char *struct_type;
	if(tree == NULL) return smprintf("");
	
	result = smprintf("char self = struct_args->self;\n");
	while(child != NULL){
		old = result;
		id = codegen(child->node.Parameter.identifier);
		type = codegen(child->node.Parameter.type);
		if(child->node.Parameter.type->tag == ArrayType){
			struct_id = smprintf("%s_original", id);
			struct_type = smprintf("%s *", type);
		} else {
			struct_id = smprintf("%s", id); 
			struct_type = smprintf("%s", type);
		}
		result = smprintf("%s %s = struct_args->%s;\n%s",
			struct_type, id, struct_id, result
		);
		child=child->next;
		free(struct_type);
		free(struct_id);
		free(old);
		free(id);
		free(type);
	}

	return result;
}

char *generatePassByValue(AstNode *tree) {
	char *result = NULL;
	char *prev;
	char *childstr;
	AstNode *child;

	if (tree == NULL) return smprintf("");

	child = tree;
	result = smprintf("");
	while(child != NULL){
		prev = smprintf("%s", result);
		switch (child->node.Parameter.type->tag) {
			case ArrayType:
				childstr = smprintf("%s %s%s; memcpy(%s, %s, sizeof(%s));",
						codegen(child->node.Parameter.type->node.ArrayType.type),
						codegen(child->node.Parameter.identifier),
						buildArrayDeclIndices(tree->node.Parameter.type),
						codegen(child->node.Parameter.identifier),
						smprintf("%s_original",codegen(child->node.Parameter.identifier)),
						codegen(child->node.Parameter.identifier)
				);
				break;
			default:
				childstr = smprintf("");
		}
		result = smprintf("%s%s", prev, childstr);
		free(prev);
		free(childstr);
		child = child->next;
	}

	return result;
}


char *constructMessageEnum(AstNode *tree){
	char *result = smprintf("");
	char *currentChild;
	char *old;
	AstNode *child = tree;
	int counter = 0;

	while(child != NULL){
		currentChild = codegen(child->node.MessageIdentifier.identifier);
		old = result;
		result = smprintf("%s%s\n%s",
				result, (counter++ != 0) ? "," : "", currentChild
				);
		free(old);
		free(currentChild);
		child = child->next;
	}
	return result;
}

char *constructMessageStruct(AstNode *tree){
	char *currentChildType;
	AstNode *child = tree;
	AstNode *paramChild;
	char *codeBlock = smprintf("");
	char *currentStruct;
	char *old;
	int i=0;
	while(child != NULL){
		paramChild = child->node.MessageIdentifier.parameters;
		currentStruct = smprintf("");
		while(paramChild != NULL){
			currentChildType = codegen(paramChild->node.Parameter.type);
			old = currentStruct;
			currentStruct = smprintf("%s arg_%d;\n%s",
					currentChildType, i, currentStruct
					);
			free(old);
			free(currentChildType);
			paramChild = paramChild->next;
			i++;
		}
		old = codeBlock;
		codeBlock = smprintf("struct %s{%s};\n%s",
			codegen(child->node.MessageIdentifier.identifier),
			currentStruct,
			codeBlock
		);

		free(old);
		free(currentStruct);
		child = child->next;
	}
	return codeBlock;
}

char *constructMessageUnionStruct(AstNode *tree){
	char *result;
	char *structNames = smprintf("");
	char *old;
	AstNode *child = tree;

	while(child != NULL){
		old = structNames;
		structNames = smprintf("struct %s %s; %s",
				codegen(child->node.MessageIdentifier.identifier),
				codegen(child->node.MessageIdentifier.identifier),
				structNames);
		free(old);
		child = child->next;
	}
	result = smprintf("struct Message { int Tag; union {%s} data;};", structNames);

	free(structNames);
	return result;
}

char *mkStructsFromSpawns(AstNode *tree){
	char *result = smprintf("");
	char *old;
	char *id;
	AstNode *child;
	if(tree == NULL) return smprintf("");
	child = tree;
	while(child != NULL){
		if(child->tag == Spawn){
			id = codegen(child->node.Spawn.identifier);
			old = result;
			result = smprintf("struct %s %s_%d;\n", 
				id, id, child->node.Spawn.taskId, child->node.Spawn.taskId);
			free(id);
		} else {
			AstNode *children = child->children;
			old = result;
			result = smprintf("%s %s", result, mkStructsFromSpawns(children));
			/* Renember to free the children! */
		}
		child=child->chain;
		free(old);
	}
	return result;
}

void changeParamNames(AstNode *tree){
	AstNode *child;
	int i = 0;
	child = tree;
	while(child != NULL){
		free(child->node.Parameter.identifier->node.Identifier.symbol->name);
		child->node.Parameter.identifier->node.Identifier.symbol->name = smprintf("arg_%d", i++);
		child = child->next;
	}
}

char *assignParamsToStruct(AstNode *spawnNode){
	char *result;
	char *old;
	int i = 0;
	char *structName = smprintf("%s_%d", 
		spawnNode->node.Spawn.identifier->node.Identifier.identifier, 
		spawnNode->node.Spawn.taskId);
	AstNode *child;
	child = spawnNode->node.Spawn.arguments;
	
	result = smprintf("Mailbox[%d] = xQueueCreate(messageQueueSize, sizeof(Message));\n%s.self = %d;\n",spawnNode->node.Spawn.taskId,structName, spawnNode->node.Spawn.taskId);

	while(child != NULL){
		char *expr = codegen(child);
		char *field = smprintf("arg_%d", i);
		Type *t = typeOf(child);
		old = result;
		if(t->tag == ArrayTypeTag){
			result = smprintf("%smemcpy(%s.%s_original, %s, sizeof(%s.%s_original));\n",
				result,
				structName,
				field,
				expr,
				structName,
				field
			);
		} else {
			result = smprintf("%s%s.%s = %s;\n", result, structName, field, expr);
		}
		child = child->next;
		i++;
		free(expr);
		free(old);
		free(field);
	}
	return result;
}

char *generateReceiveCaseData(AstNode *ReceiveCaseNode){
	char *result = smprintf("");
	char *decl = smprintf("");
	char *idBOSS = codegen(ReceiveCaseNode->node.ReceiveCase.messageName);
	char *old;
	AstNode *parameter = ReceiveCaseNode->node.ReceiveCase.dataNames;
	AstNode *msgIdentifier = ReceiveCaseNode->node.ReceiveCase.messageName->node.Identifier.symbol->first->node.MessageIdentifier.parameters;
	int i = 0;
	while(parameter != NULL){
		char *type = builtinTypeNames[msgIdentifier->node.Parameter.type->node.BuiltinType.type];
		char *old_decl = decl;
		char *id = codegen(parameter);
		decl = smprintf("%s %s;\n%s", type, id, decl);

		old = result;
		result = smprintf("%s%s = m.data.%s.arg_%d;\n", result, id, idBOSS, i);
		parameter = parameter->next;
		msgIdentifier = msgIdentifier->next;
		free(old_decl);
		free(old);
		free(id);
		i++;
	}
	old = result;
	result = smprintf("%s%s", decl, result);
	free(old);
	return result;
}