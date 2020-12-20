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
            regs->push_back("$t" + int2string(i + 2));
        }
    }
    return regs;
}
