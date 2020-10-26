#pragma once
#include "Token.h"
#include "ErrorInfo.h"
#include <vector>
class ParseToken
{
public:
	void Parse();
	ParseToken(std::vector<Token>& _TokenVec);

	std::vector<Token>& getTokenVec()
	{
		return this->TokenVec;
	}
private:
	std::vector<Token>& TokenVec;
    void errorA();
};

