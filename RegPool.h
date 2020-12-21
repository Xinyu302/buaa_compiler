//
// Created by yxy on 2020/12/20.
//

#pragma once
#include <vector>
#include <string>
#include <map>

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
public:
    SRegPool();
    std::map<int,std::string> regs;
    std::map<std::string, int> name2reg;

    bool hasReg(const std::string &name);

    std::string getReg(const std::string &name);

    void setReg(const std::string &name, int index);
    std::vector<std::string>* reg2store();
};
