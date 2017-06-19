#include "interpreter.h"

using std::cout;
using std::cin;

/*Syntax analyser*/

tokenName syntax(treeNode *leaf){
	if (leaf == NULL)
		return tn_null;

	switch (leaf->token.name){
	case tn_print:
		return syntaxOutput(leaf);
	case tn_input:
		return syntaxInput(leaf);
	case tn_assign:
	case tn_lpoint:
	case tn_rpoint:
		return syntaxAssignAndPoint(leaf);
	case tn_lshift:
	case tn_rshift:
		return syntaxShift(leaf);
	case tn_minus:
	case tn_plus:
		return syntaxPlusMinus(leaf);
	case tn_mod:
	case tn_mult:
	case tn_div:
		return syntaxBinary(leaf);
	case tn_lvalue:
		return syntaxThis(leaf);
	case tn_rvalue:
		return syntaxConst(leaf);
	case tn_error:
		return tn_error;
	}
}

tokenName syntaxShift(treeNode *leaf){
	tokenName tlhs = syntax(leaf->left);
	tokenName trhs = syntax(leaf->right);

	if (tlhs == tn_lvalue && 
		(trhs == tn_lvalue || trhs == tn_rvalue)){
		return tn_lvalue;
	}
	else {
		syntaxError("<< and >> require lvalue on lhs");
		return tn_error;
	}
}

tokenName syntaxAssignAndPoint(treeNode *leaf){
	tokenName tlhs = syntax(leaf->left);
	tokenName trhs = syntax(leaf->right);

	if (tlhs == tn_lvalue &&
		(trhs == tn_lvalue || trhs == tn_rvalue)){
		return tn_rvalue;
	}
	else {
		syntaxError("=, >, and < require lvalue on lhs");
		return tn_error;
	}
}

tokenName syntaxInput(treeNode *leaf){
	tokenName trhs = syntax(leaf->right);

	if (leaf->left == NULL && 
		(leaf->right == NULL || trhs == tn_lvalue)){
		return tn_null;
	}
	else {
		syntaxError("input requires lvalue on rhs");
		return tn_error;
	}
}

tokenName syntaxOutput(treeNode *leaf){
	tokenName trhs = syntax(leaf->right);

	if (leaf->left == NULL && 
		(trhs==tn_lvalue || trhs==tn_rvalue)){
		return tn_null;
	}
	else {
		syntaxError("print requires integer on rhs");
		return tn_error;
	}
}

tokenName syntaxBinary(treeNode *leaf){
	//for *, / , %, and, or
	tokenName tlhs = syntax(leaf->left);
	tokenName trhs = syntax(leaf->right);

	if ((tlhs == tn_lvalue || tlhs == tn_rvalue) &&
		(trhs == tn_lvalue || trhs == tn_rvalue)){
		return tn_rvalue;
	}
	else {
		syntaxError("check operands");
		return tn_error;
	}
}

tokenName syntaxPlusMinus(treeNode *leaf){
	tokenName tlhs = syntax(leaf->left);
	tokenName trhs = syntax(leaf->right);

	if ((tlhs == tn_null 
		|| tlhs == tn_lvalue
		|| tlhs == tn_rvalue) &&
		(trhs == tn_lvalue
		|| trhs == tn_rvalue)){
		return tn_rvalue;
	}
	else {
		syntaxError("check operands");
		return tn_error;
	}
}

tokenName syntaxThis(treeNode *leaf){
	if (leaf->left == NULL && leaf->right == NULL)
		return tn_lvalue;
	else {
		syntaxError("check operands");
		return tn_error;
	}
}
		

tokenName syntaxConst(treeNode *leaf){
	if (leaf->left == NULL && leaf->right == NULL)
		return tn_rvalue;
	else {
		syntaxError("check operands");
		return tn_error;
	}
}
	

/*Virtual memory functions*/

int Memory::readVal(int steps){
	//default steps is 0, can be used to read offset from cur index
	return tape[index+steps];
}
void Memory::writeVal(int data){
	tape[index] = data;
}
void Memory::movePointer(int steps){
	index += steps;
	index %= TAPE_SIZE;
}
inline int Memory::currentIndex(){
	return index;
}
inline void Memory::writeAtLocation(int idx, int data){
	tape[idx] = data;
}
void Memory::printTape(int start, int end){
	for (int i = start; i <= end; i++)
		cout<<tape[i]<<"  ";
	cout<<'\n';
}

int Memory::tape[TAPE_SIZE] = {0};
Memory memory;	

/*Solving parse tree*/

extern Token tokNull;
extern Token tokErr;

/*Assume parse tree is syntactically sound*/
//make a syntax analyser!

Token solve(treeNode *leaf){
	if (leaf == NULL)
		return tokNull;

	switch (leaf->token.name) {
	
	case tn_print:
		return funcPrint(leaf);
	case tn_input:
		return funcInput(leaf);
	case tn_assign:
		return funcAssign(leaf);
	case tn_and:
		return funcAnd(leaf);
	case tn_or:
		return funcOr(leaf);
	case tn_minus:
		return funcSub(leaf);
	case tn_plus:
		return funcAdd(leaf);
	case tn_mod:
		return funcMod(leaf);
	case tn_mult:
		return funcMult(leaf);
	case tn_div:
		return funcDiv(leaf);

	case tn_lpoint:
		return funcPoint(leaf, left);
	case tn_rpoint:
		return funcPoint(leaf, right);
	case tn_lshift:
		return funcShift(leaf, left);
	case tn_rshift:
		return funcShift(leaf, right);
	case tn_lvalue:
		return funcThis();
	case tn_rvalue:
		return leaf->token;

	case tn_error:
		return tokErr;

	}
}
/* Input output*/
Token funcInput(treeNode *leaf){
	Token trhs = solve(leaf->right);
	int n;
	cin>>n;
	memory.writeVal(n);
	Token result;
	result.setConstVal(n);
	return result;
}

Token funcPrint(treeNode *leaf){
	Token trhs = solve(leaf->right);
	cout<<(char)trhs.val;
	return tokNull;		//hmmmmm
}

/*Operators related to this (pointer)*/
Token funcAssign(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	int tempIndex = memory.currentIndex();
	Token trhs = solve(leaf->right);

	memory.writeAtLocation(tempIndex, trhs.val);

	Token result;
	result.setConstVal(trhs.val);
	return result;	//HAS to return const value
}	

Token funcThis(){
	Token result;
	result.setThisVal(memory.readVal());
	return result;
}

Token funcShift(treeNode *leaf, Side side){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	if (side == right)
		memory.movePointer(trhs.val);
	else if (side == left)
		memory.movePointer(-tlhs.val);

	Token result;
	result.setThisVal(memory.readVal());
	return result;
}

Token funcPoint(treeNode *leaf, Side side){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	if (side == left)
		trhs.val *= -1;	//okay to modify?

	Token result;
	result.setConstVal(memory.readVal(trhs.val));
	return result;
}

/*Arithmetic operations*/	

Token funcAnd(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	Token result;
	result.setConstVal(tlhs.val & trhs.val);
	return result;
}

Token funcOr(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	Token result;
	result.setConstVal(tlhs.val | trhs.val);
	return result;
}

Token funcDiv(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	if (trhs.val == 0){
		execError("Division by zero");
		return tokErr;
	}

	Token result;
	result.setConstVal(tlhs.val / trhs.val);
	return result;
}

Token funcMult(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	Token result;
	result.setConstVal(tlhs.val * trhs.val);
	return result;
}

Token funcMod(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	if (trhs.val == 0){
		execError("Modulo by 0");
		return tokErr;
	}
	Token result;
	result.setConstVal(tlhs.val % trhs.val);
	return result;
}

Token funcAdd(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	Token result;
	result.setConstVal(tlhs.val + trhs.val);
	return result;
}

Token funcSub(treeNode *leaf){
	Token tlhs = solve(leaf->left);
	Token trhs = solve(leaf->right);

	Token result;
	result.setConstVal(tlhs.val - trhs.val);
	return result;
}
