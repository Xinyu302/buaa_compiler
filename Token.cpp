#include "Token.h"
#include <iostream>

static const std::string TokenIndex2Str[] = {
	"NIL",
	"IDENFR",
	"INTCON",
	"CHARCON",
	"STRCON",
	"CONSTTK",
	"INTTK",
	"CHARTK",
	"VOIDTK",
	"MAINTK",
	"IFTK",
	"ELSETK",
	"SWITCHTK",
	"CASETK",
	"DEFAULTTK",
	"WHILETK",
	"FORTK",
	"SCANFTK",
	"PRINTFTK",
	"RETURNTK",
	"PLUS",
	"MINU",
	"MULT",
	"DIV",
	"LSS",
	"LEQ",
	"GRE",
	"GEQ",
	"EQL",
	"NEQ",
	"COLON",
	"ASSIGN",
	"SEMICN",
	"COMMA",
	"LPARENT",
	"RPARENT",
	"LBRACK",
	"RBRACK",
	"LBRACE",
	"RBRACE"
};
std::string stringToLowerCase(std::string& s)
{
	std::string ans;
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] >= 'A' && s[i] <= 'Z')
		{
			char tmp = s[i] + 32;
			ans += tmp;
		}
		else
		{
			ans += s[i];
		}
	}
	return ans;
}

void Token::handleLenOne(std::string& s)
{
	char c = s[0];
	if (c == '+')
	{
		this->index = Token::PLUS;
	}
	if (c == '-')
	{
		this->index = Token::MINU;
	}
	if (c == '*')
	{
		this->index = Token::MULT;
	}
	if (c == '/')
	{
		this->index = Token::DIV;
	}
	if (c == '>')
	{
		this->index = Token::GRE;
	}
	if (c == '<')
	{
		this->index = Token::LSS;
	}
	if (c == '=')
	{
		this->index = Token::ASSIGN;
	}
	if (c == ';')
	{
		this->index = Token::SEMICN;
	}
	if (c == ',')
	{
		this->index = Token::COMMA;
	}
	if (c == '(')
	{
		this->index = Token::LPARENT;
	}
	if (c == ')')
	{
		this->index = Token::RPARENT;
	}
	if (c == '[')
	{
		this->index = Token::LBRACK;
	}
	if (c == ']')
	{
		this->index = Token::RBRACK;
	}
	if (c == '{')
	{
		this->index = Token::LBRACE;
	}
	if (c == '}')
	{
		this->index = Token::RBRACE;
	}
	if (c == ':')
	{
		this->index = Token::COLON;
	}
}

void Token::handleLetters(std::string& s)
{
	std::string str = stringToLowerCase(s);
	if (str == "const")
	{
		this->index = Token::CONSTTK;
	}
	else if (str == "int")
	{
		this->index = Token::INTTK;
	}
	else if (str == "char")
	{
		this->index = Token::CHARTK;
	}
	else if (str == "void")
	{
		this->index = Token::VOIDTK;
	}
	else if (str == "main")
	{
		this->index = Token::MAINTK;
	}
	else if (str == "if")
	{
		this->index = Token::IFTK;
	}
	else if (str == "else")
	{
		this->index = Token::ELSETK;
	}
	else if (str == "switch")
	{
		this->index = Token::SWITCHTK;
	}
	else if (str == "case")
	{
		this->index = Token::CASETK;
	}
	else if (str == "default")
	{
		this->index = Token::DEFAULTTK;
	}
	else if (str == "while")
	{
		this->index = Token::WHILETK;
	}
	else if (str == "for")
	{
		this->index = Token::FORTK;
	}
	else if (str == "scanf")
	{
		this->index = Token::SCANFTK;
	}
	else if (str == "printf")
	{
		this->index = Token::PRINTFTK;
	}
	else if (str == "return")
	{
		this->index = Token::RETURNTK;
	}
	else 
	{
		this->index = Token::IDENFR;
	}
}

void Token::handleLenTwo(std::string& str)
{
	if (str == ">=")
	{
		this->index = Token::GEQ;
	}
	if (str == "!=")
	{
		this->index = Token::NEQ;
	}
	if (str == "<=")
	{
		this->index = Token::LEQ;
	}
	if (str == "==")
	{
		this->index = Token::EQL;
	}
}

Token::Token()
{
	this->index = Token::NIL;
	this->TokenStr = "";
}

Token::Token(std::string TokenStr,int type,int line)
{
	this->TokenStr = TokenStr;
	this->line = line;
	if (type == 0)
	{
		handleLetters(TokenStr);
	} 
	else if (type == 1)
	{
		this->index = Token::INTCON;
	}
	else if (type == 2)
	{
		this->index = Token::CHARCON;
	}
	else if (type == 3)
	{
		this->index = Token::STRCON;
	}
	else if (type == 4)
	{
		if (TokenStr.length() == 1)
		{
			handleLenOne(TokenStr);
		}
		else
		{
			handleLenTwo(TokenStr);
		}
	}
}

std::string Token::TokenPrintStr() {
	std::string ret;
	ret += TokenIndex2Str[this->index];
	ret += ' ';
	ret += this->TokenStr;
	ret += '\n';
	return ret;
}

Token::TokenTypeIndex Token::getIndex()
{
	return this->index;
}

std::string Token::getTokenStr()
{
	return this->TokenStr;
}

int& Token::getLine()
{
    return this->line;
}