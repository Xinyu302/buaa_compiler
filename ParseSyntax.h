#pragma once
#include <vector>
#include "Token.h"
bool Handle_FACTOR(bool show);
bool Handle_RETURN_FUNC_CALL(bool show);
bool Handle_STATEMENT(bool);
bool Handle_CONDITON_TABLE(bool );
bool Handle_CONST_DEFINE(bool);
bool Handle_CONDITON_TABLE(bool show);
bool Handle_CONDITION_CHILD_STATE(bool show);
bool Handle_DEFAULT(bool show);
bool Handle_STATE_LIST(bool show);
bool HandleMAIN(bool show);
class ParseSyntax
{
public:
	ParseSyntax(std::vector<Token>& TokenVec);
	void printAllTokens();
	void parse();
	std::vector<Token>& getTokens();
private:
	std::vector<Token>& TokenVec;
};

