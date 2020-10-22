#pragma once
#include <iostream>
#include <string>
class Token
{
public:
	enum TokenTypeIndex {
		NIL,
		IDENFR,
		INTCON,
		CHARCON,
		STRCON,
		CONSTTK,
		INTTK,
		CHARTK,
		VOIDTK,
		MAINTK,
		IFTK,
		ELSETK,
		SWITCHTK,
		CASETK,
		DEFAULTTK,
		WHILETK,
		FORTK,
		SCANFTK,
		PRINTFTK,
		RETURNTK,
		PLUS,
		MINU,
		MULT,
		DIV,
		LSS,
		LEQ,
		GRE,
		GEQ,
		EQL,
		NEQ,
		COLON,
		ASSIGN,
		SEMICN,
		COMMA,
		LPARENT,
		RPARENT,
		LBRACK,
		RBRACK,
		LBRACE,
		RBRACE
	};
	
	Token();
	Token(std::string str,int type,int line);
	std::string TokenPrintStr();
	Token::TokenTypeIndex getIndex();
	std::string getTokenStr();
	
private:
	std::string TokenStr;
	TokenTypeIndex index;
	int line;
	void handleLenOne(std::string& s);
	void handleLetters(std::string& s);
	void handleLenTwo(std::string& str);
};

