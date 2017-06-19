#include "token.h"

using std::cout;

//supported tokens...

Token tokNull = {tn_null, tp_null, 0};	  //Null pointer in BPT
Token tokErr = {tn_error, tp_error, 0};	 //ohfuk error

Token tokPrint = {tn_print, tp_print, 0};
Token tokInput = {tn_input, tp_input, 0};	

Token tokThis = {tn_lvalue, tp_lvalue, 0};  //this
Token tokAssign = {tn_assign, tp_assign, 0};	//=
Token tokLShift = {tn_lshift, tp_lshift, 0};	//<<
Token tokRShift = {tn_rshift, tp_rshift, 0};	//>>
Token tokLPoint = {tn_lpoint, tp_lpoint, 0};	//<
Token tokRPoint = {tn_rpoint, tp_rpoint, 0};	//>

Token tokOr = {tn_or, tp_or, 0};
Token tokAnd = {tn_and, tp_and, 0};

Token tokMinus = {tn_minus, tp_minus, 0}; 
Token tokPlus = {tn_plus, tp_plus, 0};
Token tokMod = {tn_mod, tp_mod, 0};
Token tokMult = {tn_mult, tp_mult, 0};
Token tokDiv = {tn_div, tp_div, 0};

/*Token list functions*/

Token *createTokenList(Lexeme *lexlist){
	if (lexlist == NULL) return NULL;

	Token *toklist = new Token [256];
	int i;
	for (i = 0; lexlist[i].text[0] != '\0' &&  i < 255; i++){
		toklist[i] = convertLexemeToToken(lexlist[i]);
	}

	toklist[i] = tokNull;	//delimiter for list of tokens
	return toklist;
}

/*The tokenizer*/

Token convertLexemeToToken(Lexeme lexeme){
	Token result = tokNull;	//default return is tokNull

	if (strcmp(lexeme.text, "<<") == 0)
		result = tokLShift;
	else if (strcmp(lexeme.text, ">>") == 0)
		result = tokRShift;
	else if (strcmp(lexeme.text, "+") == 0)
		result = tokPlus;
	else if (strcmp(lexeme.text, "-") == 0)
		result = tokMinus;
	else if (strcmp(lexeme.text, "*") == 0)
		result = tokMult;
	else if (strcmp(lexeme.text, "/") == 0)
		result = tokDiv;
	else if (strcmp(lexeme.text, "%") == 0)
		result = tokMod;
	else if (strcmp(lexeme.text, "=") == 0)
		result = tokAssign;
	else if (strcmp(lexeme.text, "this") == 0)
		result = tokThis;
	else if (strcmp(lexeme.text, "print") == 0)
		result = tokPrint;
	else if (strcmp(lexeme.text, "input") == 0)
		result = tokInput;
	else if (strcmp(lexeme.text, "and") == 0)
		result = tokAnd;
	else if (strcmp(lexeme.text, "or") == 0)
		result = tokOr;
	else if (strcmp(lexeme.text, "<") == 0)
		result = tokLPoint;
	else if (strcmp(lexeme.text, ">") == 0)
		result = tokRPoint;

	else if (lexeme.isValidInt()){
		int val = atoi(lexeme.text);
		result.setConstVal(val);
	}
	else {
		tokenError("unrecognized token");
		return tokErr;
	}
	
	
	return result;
}

void printTokenList(Token *toklist){
	for (int i = 0; toklist[i].isValid(); i++)
		toklist[i].print();
}

void destroyTokenList(Token *toklist){
	delete[] toklist;
}

/*Token Class*/

void Token::setConstVal(int data){
	val = data;
	name = tn_rvalue;
	priority = tp_rvalue;
}

void Token::setThisVal(int data){
	val = data;
	name = tn_lvalue;
	priority = tp_lvalue;
}

void Token::print(){
	switch(name){
	case tn_print: cout<<"print"; break;
	case tn_assign: cout<<'='; break;
	case tn_minus: cout<<'-'; break;
	case tn_plus: cout<<'+'; break;
	case tn_mult: cout<<'*'; break;
	case tn_div: cout<<'/'; break;
	case tn_lshift: cout<< "<<"; break;
	case tn_rshift: cout<< ">>"; break;
	case tn_lvalue:
		cout<<'l';
	case tn_rvalue:
		cout<<val;
		break;

	case tn_null: break;
	case tn_error: cout<<"Error"; break;
	default:
		cout<<"Fix the compiler m8";
		break;
	}
	cout<<'\n';
}

int Token::isValid(){
	if (name == tn_null || name == tn_error)
		return 0;   //not valid
	else
		return 1;   //valid
}	 
