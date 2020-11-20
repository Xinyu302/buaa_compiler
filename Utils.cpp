//
// Created by yxy on 2020-11-19.
//

#include "Utils.h"
static int forId = 0;
static int whileId = 0;
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