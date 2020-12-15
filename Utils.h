//
// Created by yxy on 2020-11-19.
//
#pragma once
#include <string>
#include "Token.h"
#include "MidCode.h"
std::string int2string(int num);
std::string getNextStringId();
std::string getNextTmpValId();
std::string getNextWhileId();
std::string getNextForId();
std::string getNextWhileEnd();
std::string getNextForEnd();
std::string getNextElseId();
std::string getNextElseEndId();
std::string getNextCaseId();
std::string getNextCaseId(bool add);
std::string getNextDefault();
MidCode::MidCodeOperator index2MidCode(Token::TokenTypeIndex index);
MidCode::MidCodeOperator reverseIndex2MidCode(Token::TokenTypeIndex index);
std::string changeString(const std::string& s);
std::string getSwitchId();
std::string getExp2Judge();

bool judgeByCmpIndex(int value1, int value2, Token::TokenTypeIndex index);
void setExp2Judge(const std::string& s);
std::string getNextSwitchEnd();
void addSwitchId();

int log2_B(int value) ;