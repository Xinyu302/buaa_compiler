//
// Created by yxy on 2020-11-10.
//

#pragma once
#include <string>
#include "Token.h"
#include "SymbolTableItem.h"

extern std::vector<Token>* TokenVecPointer;
extern std::vector<SymbolTableItem> OuterSymbolTable;
extern std::map<std::string, std::vector<SymbolTableItem>> allInnerSymbolTable;


void genMips();
void genData();
void genText();
void printMips();
void printMipsCode();


void genSll(const std::string& result,const std::string& from,int imm);
