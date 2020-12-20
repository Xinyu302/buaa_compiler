//
// Created by yxy on 2020-11-10.
//

#pragma once
#include <string>
#include "Token.h"
#include "SymbolTableItem.h"

extern std::vector<Token>* TokenVecPointer;

void genMips();
void genData();
void genText();
void printMips();
void printMipsCode();

void genSaveTReg();
void genRestoreTReg();

void genSll(const std::string& result,const std::string& from,int imm);
void genSrl(const std::string& result,const std::string& from,int imm);
