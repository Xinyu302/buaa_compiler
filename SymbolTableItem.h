//
// Created by yxy on 2020-10-22.
//
#pragma once

#include <string>
#include <vector>
#include <map>
#include "Token.h"
#include "MidCode.h"
#define InnerSymbolTable (*innerSymbolTablePtr)

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

class FunctionSymbolTable {
public:
    enum itemType{
        CONST,
        LOCALVAR,
        PARA,
        GLOBALVAR,
        TEMPVAR,
        LOCALARRAY1,
        LOCALARRAY2
    };
    struct itemInfo{
        itemType type;
        int offset;
        itemInfo(itemType _type,int _offset):type(_type),offset(_offset){}
        itemInfo(){}
    };

    struct arrayItemInfo{
        itemType type;
        int x;
        int y;
        arrayItemInfo(itemType _type,int _x,int _y = 0): type(_type),x(_x),y(_y) {}
    };

    FunctionSymbolTable(const std::string& name);
    void appendPara(const std::string& name);
    void appendConst(const std::string& name,int value);
    void appendTempVar(const std::string& name);
    void appendLocalVar(const std::string& name);
    void appendLocalVar(const std::string& name,int );
    void appendLocalVar(const std::string& name,int,int);
//    void appendGlobalVar(const std::string& name);
    int getOffset(const std::string& name);
    int getOffset(const std::string& name,int x);
    int getOffset(const std::string &name, int x, int y);
    int getParaOffsetByIndex(int index);
    int getSubOffset();
    bool isConstValue(const std::string& name,int &value);
    int getRetOffset();

private:
    int varNum;
    std::string funcName;
    std::map<std::string,itemInfo> varInfo;
    std::map<std::string,arrayItemInfo> arrayInfo;
    std::vector<itemInfo> paraInfo;
};


class SyntaxSymbolTable {
public:
    SyntaxSymbolTable() {
        innerSymbolTablePtr = new std::map<std::string, SymbolTableItem *>;
    }

    ~SyntaxSymbolTable() {
        for (auto it = allInnerSymbolTable.begin();it != allInnerSymbolTable.end();it++) {
            delete it->second;
        }
    }

    void InsertPtrIntoTable(int inner, const std::string &name, SymbolTableItem *itemPtr) {
        if (inner == INNER) {
            InnerSymbolTable[name] = itemPtr;
        } else {
            OuterSymbolTable[name] = itemPtr;
        }
    }

    bool insertIntoSymbolTableVar(int inner, const std::string& name, Token::TokenTypeIndex index, int dimension = 0, int x = 0, int y = 0)
    {
        SymbolTableItem* itemPtr;
        if (index == Token::INTTK)
        {
            itemPtr = new SymbolTableItem(name,SymbolTableItem::VAR,SymbolTableItem::INT,dimension,x,y);
        }
        else if (index == Token::CHARTK)
        {
            itemPtr = new SymbolTableItem(name,SymbolTableItem::VAR,SymbolTableItem::CHAR,dimension,x,y);
        }
        InsertPtrIntoTable(inner, name, itemPtr);
        return true;
    }

    bool insertIntoSymbolTableConst(int inner, const std::string& name, Token::TokenTypeIndex index)
    {
        SymbolTableItem* itemPtr;
        if (index == Token::INTTK)
        {
            itemPtr = new SymbolTableItem(name,SymbolTableItem::CONST,SymbolTableItem::INT);
        }
        else if (index == Token::CHARTK)
        {
            itemPtr = new SymbolTableItem(name, SymbolTableItem::CONST, SymbolTableItem::CHAR);
        }
        InsertPtrIntoTable(inner, name, itemPtr);
        return true;
    }

    bool insertIntoSymbolTableFunc(int inner, const std::string& name, Token::TokenTypeIndex index,const std::vector<SymbolTableItem::ItemReturnType>& s)
    {
        SymbolTableItem* itemPtr;
        if (index == Token::INTTK)
        {
            itemPtr = new SymbolTableItem(name,SymbolTableItem::FUNC,SymbolTableItem::INT,s);
        }
        else if (index == Token::CHARTK)
        {
            itemPtr = new SymbolTableItem(name, SymbolTableItem::FUNC, SymbolTableItem::CHAR,s);
        }
        else if (index == Token::VOIDTK)
        {
            itemPtr = new SymbolTableItem(name, SymbolTableItem::FUNC, SymbolTableItem::VOID,s);
        }
        InsertPtrIntoTable(inner, name, itemPtr);
        return true;
    }

    SymbolTableItem* findSymbolTableItem(int inner, const std::string& name)
    {
        if (inner == INNER)
        {
            if (InnerSymbolTable.find(name) == InnerSymbolTable.end()) {
                return nullptr;
            }
            return InnerSymbolTable[name];
        }
        else
        {
            if (OuterSymbolTable.find(name) == OuterSymbolTable.end()) {
                return nullptr;
            }
            return OuterSymbolTable[name];
        }
        return nullptr;
    }

    SymbolTableItem* findSymbolTableItem(const std::string& name)
    {
        SymbolTableItem* ptr = nullptr;
        if (isInner)
        {
            ptr = findSymbolTableItem(INNER,name);
            if (ptr != nullptr) {
                return ptr;
            }
            ptr = findSymbolTableItem(OUTER,name);
            return ptr;
        }
        return findSymbolTableItem(OUTER,name);
    }

    bool notFindSymbolTableItem(const std::string& name) {
        return findSymbolTableItem(name) == nullptr;
    }

    void setInner(int inner) {
        isInner = inner;
    }

    bool defineCheckMulti(int Inner,const std::string& name) {
        return (findSymbolTableItem(Inner,name) != nullptr);
    }

    void clearInnerSymbolTable(const std::string& idName)
    {
        allInnerSymbolTable[idName] = innerSymbolTablePtr;
        innerSymbolTablePtr = new std::map<std::string, SymbolTableItem *>;
    }

    std::map<std::string, SymbolTableItem *>* innerSymbolTablePtr;
    std::map<std::string, std::map<std::string, SymbolTableItem *>*> allInnerSymbolTable;

private:
     const int INNER = 1;
     const int OUTER = 0;
     int isInner;
     std::map<std::string, SymbolTableItem *> OuterSymbolTable;
};

