//
// Created by yxy on 2020-10-22.
//

#include "SymbolTableItem.h"
#include <string>

bool SymbolTableItem::compareParaListNum(const std::vector<ItemReturnType> &paraListTypeIn) const {
    int n1 = paraListTypes.size();
    int n2 = paraListTypeIn.size();
    if (n1 != n2) return false;
    return true;
}

bool SymbolTableItem::compareParaList(const std::vector<ItemReturnType>& paraListTypeIn) const {
    int n1 = paraListTypes.size();
    for (int i = 0; i < n1; i++)
    {
        if (paraListTypes[i] != paraListTypeIn[i]) return false;
    }
    return true;
}

bool SymbolTableItem::compareName(const std::string &nameIn) const {
    int l1 = this->name.length();
    int l2 = nameIn.length();
    if (l1 != l2) return false;
    for (int i = 0; i < l2;i++)
        if (this->name[i] != nameIn[i]) return false;
    return true;
}

extern FunctionSymbolTable* globalSymbolTable;

SymbolTableItem::SymbolTableItem() {}

FunctionSymbolTable::FunctionSymbolTable(const std::string& name):funcName(name),varNum(0) {}

void FunctionSymbolTable::appendConst(const std::string &name,int value) {
    if (varInfo.find(name) != varInfo.end())
        varInfo[name].type = CONST;
    else {
        varInfo[name] = itemInfo(CONST,value);
    }
}

void FunctionSymbolTable::appendTempVar(const std::string &name) {
    varInfo[name] = itemInfo(TEMPVAR,4*varNum++);
}

void FunctionSymbolTable::appendLocalVar(const std::string &name) {
    if (varInfo.find(name) != varInfo.end())
    varInfo[name].type = LOCALVAR;
    else
        varInfo[name] = itemInfo(LOCALVAR,4*varNum++);
}

void FunctionSymbolTable::appendGlobalVar(const std::string &name) {
    varInfo[name] = itemInfo(GLOBALVAR,4*varNum++);
}

int FunctionSymbolTable::getOffset(const std::string& name) {
    return varInfo[name].offset;
}

int FunctionSymbolTable::getSubOffset() {
    return 4 * this->varNum;
}

bool FunctionSymbolTable::isConstValue(const std::string& name, int &value) {
    auto it = varInfo.find(name);
    if (it != varInfo.end()) {
        if ((it->second).type == CONST) {
            value = it->second.offset;
            return true;
        } else {
            return false;
        }
    }
    return globalSymbolTable->isConstValue(name,value);
}

