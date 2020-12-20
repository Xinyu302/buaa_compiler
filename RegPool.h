//
// Created by yxy on 2020/12/20.
//

#pragma once
#include <vector>
#include <string>

class TRegPool {
public:
    std::vector<std::string> regPools;
    int regFree;
    bool use[8];
    int tmpReg[8] = {2,3,4,5,6,7,8,9};
    TRegPool();
    bool hasFreeReg();
    std::string allocReg(std::string name);
    std::string checkRegByName(std::string name);

    std::vector<std::string>* reg2store();
};

class SRegPool {
    int regSum = 8;

};
