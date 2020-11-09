//
// Created by yxy on 2020-10-22.
//
#pragma once

#include <string>
#include <vector>
#include <map>
#include "Token.h"

class SymbolTableItem {
public:
    SymbolTableItem();

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
        FUNC
    };

    SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
                                     std::vector<ItemReturnType> paraListTypes):name(name),classType(classType),returnType(returnType),paraListTypes(paraListTypes){}

    SymbolTableItem(std::string name, ItemClassType classType, ItemReturnType returnType,
                                     int dimension = 0, int x = 0, int y = 0) : name(name), classType(classType),
                                                                                returnType(returnType),
                                                                                dimension(dimension), vec1(x),
                                                                                vec2(y) {}

    bool compareParaList(const std::vector<ItemReturnType>& paraListTypeIn) const;
    bool compareParaListNum(const std::vector<ItemReturnType>& paraListTypeIn) const;

    bool compareName(const std::string& nameIn) const;

    ItemReturnType& getReturnType()
    {
        return returnType;
    }

    ItemClassType& getClassType()
    {
        return classType;
    }
private:
    std::string name;
    ItemClassType classType;
    ItemReturnType returnType;
    std::vector<ItemReturnType> paraListTypes;
    int dimension; // var dimension
    int vec1,vec2;
};

extern std::vector<Token>* TokenVecPointer;
extern std::vector<SymbolTableItem> OuterSymbolTable;
extern std::map<std::string, std::vector<SymbolTableItem>> allInnerSymbolTable;

