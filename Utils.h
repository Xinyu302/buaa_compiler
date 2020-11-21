//
// Created by yxy on 2020-11-19.
//
#pragma once
#include <string>
#include "Token.h"
#include "MidCode.h"
std::string getNextStringId();
std::string getNextTmpValId();
std::string getNextWhileId();
std::string getNextForId();
std::string getNextWhileEnd();
std::string getNextForEnd();
std::string getNextElseId();
std::string getNextElseEndId();
MidCode::MidCodeOperator index2MidCode(Token::TokenTypeIndex index);
MidCode::MidCodeOperator reverseIndex2MidCode(Token::TokenTypeIndex index);