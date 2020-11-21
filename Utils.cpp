//
// Created by yxy on 2020-11-19.
//

#include "Utils.h"
static int forId = 0;
static int whileId = 0;
static int elseId = 0;
const std::string e = ":";
std::string int2string(int num) {
    char tmpNo[20];
    snprintf(tmpNo,20,"%d",num);
    return std::string(tmpNo);
}
std::string getNextStringId() {
    static int num = 0;
    static const std::string tmpStringPrefix = "string";
    std::string&& s = tmpStringPrefix + int2string(num++);
    return s;
}

std::string getNextTmpValId() {
    static int valId = 0;
    static const std::string tmpPrefix = "@temp";
    std::string&& s = tmpPrefix + int2string(valId++);
    return s;
}

std::string getNextWhileId() {
    static const std::string whilePrefix = "@while_";
    std::string&& s = whilePrefix + int2string(whileId);
    return s;
}

std::string getNextForId() {
    static const std::string forPrefix = "@for_";
    std::string&& s = forPrefix + int2string(forId);
    return s;
}

std::string getNextWhileEnd() {
    static const std::string whileEndPre = "@while_end_";
    std::string&& s = whileEndPre + int2string(whileId++);
    return s;
}

std::string getNextForEnd() {
    static const std::string forEndPre = "@for_end_";
    std::string&& s = forEndPre + int2string(forId++);
    return s;
}

std::string getNextElseId() {
    static const std::string elsePre = "@else_";
    std::string &&s = elsePre + int2string(elseId);
    return s;
}

std::string getNextElseEndId() {
    static const std::string elseEndPre = "@else_end_";
    std::string &&s = elseEndPre + int2string(elseId++);
    return s;
}

MidCode::MidCodeOperator index2MidCode(Token::TokenTypeIndex index) {
    static std::map<Token::TokenTypeIndex, MidCode::MidCodeOperator> map = {{Token::EQL, MidCode::EQL},
                                                                            {Token::NEQ, MidCode::NEQ},
                                                                            {Token::GEQ, MidCode::GEQ},
                                                                            {Token::GRE, MidCode::GRE},
                                                                            {Token::LSS, MidCode::LSS},
                                                                            {Token::LEQ, MidCode::LEQ}};
    return map[index];
}

MidCode::MidCodeOperator reverseIndex2MidCode(Token::TokenTypeIndex index) {
    static std::map<Token::TokenTypeIndex, MidCode::MidCodeOperator> map = {{Token::EQL, MidCode::NEQ},
                                                                            {Token::NEQ, MidCode::EQL},
                                                                            {Token::GEQ, MidCode::LSS},
                                                                            {Token::GRE, MidCode::LEQ},
                                                                            {Token::LSS, MidCode::GEQ},
                                                                            {Token::LEQ, MidCode::GRE}};
    return map[index];
}