//
// Created by yxy on 2020-11-19.
//

#include "Utils.h"
static int forId = 0;
static int whileId = 0;
static int elseId = 0;
static int switchId = 0;
static int caseId = 1;
const std::string e = ":";
static std::string switchEndId;
static std::string exp2judge;

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
    static const std::string whilePrefix = "while_";
    std::string&& s = whilePrefix + int2string(whileId);
    return s;
}

std::string getNextForId() {
    static const std::string forPrefix = "for_";
    std::string&& s = forPrefix + int2string(forId);
    return s;
}

std::string getNextWhileEnd() {
    static const std::string whileEndPre = "while_end_";
    std::string&& s = whileEndPre + int2string(whileId++);
    return s;
}

std::string getNextForEnd() {
    static const std::string forEndPre = "for_end_";
    std::string&& s = forEndPre + int2string(forId++);
    return s;
}

std::string getNextElseId() {
    static const std::string elsePre = "else_";
    std::string &&s = elsePre + int2string(elseId);
    return s;
}

std::string getNextElseEndId() {
    static const std::string elseEndPre = "else_end_";
    std::string &&s = elseEndPre + int2string(elseId++);
    return s;
}

std::string getNextCaseId() {
    static const std::string switchPre = "switch_";
    static const std::string casePre = "_case_";
    std::string &&s = switchPre + int2string(switchId) + casePre + int2string(caseId);
    return s;
}

std::string getNextCaseId(bool add) {
    static const std::string switchPre = "switch_";
    static const std::string casePre = "_case_";
    std::string &&s = switchPre + int2string(switchId) + casePre + int2string(caseId++);
    return s;
}


std::string getNextDefault() {
    static const std::string switchPre = "switch_";
    static const std::string defaultEnd = "_default";
    std::string &&s = switchPre + int2string(switchId++) + defaultEnd;
    caseId = 1;
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

std::string changeString(const std::string& s) {
    static char y = '\\';
    char *tmp = new char [s.length() * 2];
    int j = 0;
    for (int i = 0; i < s.length();i++) {
        if (s[i] == y) {
            tmp[j++] = y;
            tmp[j++] = y;
        }
        else {
            tmp[j++] = s[i];
        }
    }
    tmp[j] = 0;
    std::string&& ss = std::string(tmp);
    delete [] tmp;
    return ss;
}

std::string getExp2Judge() {
    return exp2judge;
}

void setExp2Judge(const std::string& s) {
    exp2judge = s;
}

std::string getNextSwitchEnd() {
    static const std::string switchEndPre = "switch_end_";
    std::string &&s = switchEndPre + int2string(switchId);
    switchEndId = s;
    return s;
}

void addSwitchId() {
    switchId++;
    caseId = 1;
}


int log2_B(int value) //非递归判断一个数是2的多少次
{
    int x=0;
    while(value>1)
    {
        value>>=1;
        x++;
    }
    return x;
}

bool judgeByCmpIndex(int value1, int value2, Token::TokenTypeIndex index) {
    if (index == Token::GRE) {
        return (value1 >= value2);
    }
}

bool is_n_two(int value2) {
    return ((value2 & (value2 - 1)) == 0);
}
