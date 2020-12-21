//
// Created by yxy on 2020/12/20.
//

#include "RegPool.h"
#include "Utils.h"

bool TRegPool::hasFreeReg() {
    return (this->regFree > 0);
}

std::string TRegPool::allocReg(std::string name) {
    for (int i = 0; i < 8;i++) {
        if (this->use[i] == false) {
            this->use[i] = true;
            regPools[i] = name;
            --regFree;
            return "$t" + int2string(i + 2);
        }
    }
}

TRegPool::TRegPool() {
    regFree = 8;
    for (int i = 0; i < 8;i++) {
        use[i] = false;
        this->regPools.push_back("");
    }
}

std::string TRegPool::checkRegByName(std::string name) {
    for (int i = 0; i < 8; i++) {
        if (this->regPools[i] == name && this->use[i] == true) {
            this->regFree++;
            this->use[i] = false;
            return "$t" + int2string(i + 2);
        }
    }
    return "";
}

std::vector<std::string>* TRegPool::reg2store() {
    std::vector<std::string>* regs = new std::vector<std::string>;
    for (int i = 0; i < 8; i++) {
        if (this->use[i]) {
//            printf("%d\n", i);
            regs->push_back("$t" + int2string(i + 2));
        }
    }
    return regs;
}

std::string SRegPool::getReg(const std::string &name,bool f) {
    auto it = name2reg.find(name);
    if (it != name2reg.end()) {
        if (f) {
            name2use[name] = true;
        }
        return regs[(it->second)];
    }
    return "";
}

SRegPool::SRegPool() {
    for (int i = 0; i < 8; i++) {
        regs[i] = "$s" + int2string(i);
    }
    for (int i = 8; i <= 10;i++) {
        regs[i] = "$a" + int2string(i - 7);
    }
}

void SRegPool::setReg(const std::string &name,int index) {
    if (index >= 8) {
        name2use[name] = true;
    } else {
        name2use[name] = false;
    }
    name2reg[name] = index;
}

std::vector<std::string> *SRegPool::reg2store() {
    std::vector<std::string>* regVec = new std::vector<std::string>;
    for (auto it = name2reg.begin();it != name2reg.end();it++) {
//        printf("%s\n", it->first);
//        std::cout << it->first << std::endl;
        if (name2global.find(it->first) != name2global.end()) {
            continue;
        }
        if (name2use[it->first]) {
            regVec->push_back(regs[it->second]);
        }
    }
//    std::cout << std::endl;
    return regVec;
}
