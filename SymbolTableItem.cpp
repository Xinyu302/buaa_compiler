//
// Created by yxy on 2020-10-22.
//

#include "SymbolTableItem.h"
#include <string>

SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType)
{

}

SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
std::vector<ItemReturnType> paraListTypes);

SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
int vec1);

SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
int vec1,int vec2);