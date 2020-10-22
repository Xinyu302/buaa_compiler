//
// Created by yxy on 2020-10-22.
//
#pragma once

#include <string>
#include <vector>
class SymbolTableItem {
enum ItemReturnType
{
    VOID,
    INT,
    CHAR
};
enum ItemClassType
{
    VAR,
    CONST,
    FUNC,
    ARRAY
};
public:
    SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType);

    SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
                    std::vector<ItemReturnType> paraListTypes);

    SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
                    int vec1);

    SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
                    int vec1,int vec2);
private:
    std::string name;
    ItemClassType classType;
    ItemReturnType returnType;
    std::vector<ItemReturnType> paraListTypes;
    int dimension; // var dimension
    int vec1,vec2;
};


