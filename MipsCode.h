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

extern std::vector<MidCode*> midCodeVec;
extern std::map<std::string,std::string> stringMap;

class MipsCode {
public:
    enum mipsOperation {
        add,
        addi,
        sub,
        mult,
        mul,
        divop,
        mflo,
        mfhi,
        sll,
        beq,
        bne,
        bgt, //扩展指令 相当于一条ALU类指令+一条branch指令
        bge, //扩展指令 相当于一条ALU类指令+一条branch指令
        blt, //扩展指令 相当于一条ALU类指令+一条branch指令
        ble, //扩展指令 相当于一条ALU类指令+一条branch指令
        blez,  //一条branch
        bgtz,  //一条branch
        bgez,  //一条branch
        bltz,  //一条branch
        j,
        jal,
        jr,
        lw,
        sw,
        syscall,
        li,
        la,
        moveop,
        dataSeg,  //.data
        textSeg,  //.text
        asciizSeg,  //.asciiz
        globlSeg,  //.globl
        label,  //产生标号
    };

    void genMipsFromMidCode();  //中间代码生成mips
    void printMipsCode();  //输出mips语句
    void loadValue(const std::string& name, const std::string& regName, bool gene, int& va, bool& get, bool assign);  //取变量name的值到寄存器regName，后边的参数主要涉及到对常值的处理方式
    void storeValue(const std::string &name, const std::string &regName);  //将regName的值存储到name的空间中
private:
    mipsOperation operatoration; // 操作
    std::string result;     // 结果
    std::string left;     // 左操作数
    std::string right;     // 右操作数
    int immediate;     // 立即数
};
void genMips();
void genData();
void genText();
void printMips();
void printMipsCode();
