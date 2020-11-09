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

SymbolTableItem::SymbolTableItem() {

}

