#pragma once
#include "Token.h"
#include "ErrorInfo.h"
#include <vector>
class ParseToken
{
public:
	void Parse();
	ParseToken(std::vector<Token>& _TokenVec,std::vector<ErrorInfo> _ErrorInfoVEC);

	std::vector<Token>& getTokenVec()
	{
		return this->TokenVec;
	}
private:
	std::vector<Token>& TokenVec;
	std::vector<ErrorInfo>& ErrorInfoVec;
    void errorA();
};

