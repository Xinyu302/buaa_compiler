#pragma once
#include <vector>
#include "Token.h"
#include "ErrorInfo.h"
#include <stdlib.h>
bool Handle_FACTOR(bool show,int &,std::string &);
bool Handle_RETURN_FUNC_CALL(bool show,int &);
bool Handle_STATEMENT(bool);
bool Handle_CONDITON_TABLE(bool ,const int &);
bool Handle_CONST_DEFINE(bool);
bool Handle_CONDITION_CHILD_STATE(bool show,const int& type);
bool Handle_DEFAULT(bool show);
bool Handle_STATE_LIST(bool show);
bool Handle_MAIN(bool show);

//const std::string tmpVarPrefix = "@temp_";
//int curTmpNo;
std::string getNextStringId();
//std::string genNextTmpVar()
//{
//    char tmpNo[20];
//    itoa(curTmpNo++,tmpNo,10);
//    std::string s = tmpNo;
//    return (tmpVarPrefix + s);
//}
//
//void resetTmpNo()
//{
//    curTmpNo = 0;
//}

class ParseSyntax
{
public:
	ParseSyntax(std::vector<Token>& _TokenVec);

    void parse();
	std::vector<Token>& getTokens();
private:
	std::vector<Token>& TokenVec;
};

