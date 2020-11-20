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
    MipsCode(mipsOperation operation,const std::string &rd,const std::string &rs,const std::string& rt);
    MipsCode(mipsOperation operation,const std::string &rt,const std::string &rs,const int &imm);
    MipsCode(mipsOperation operation,const std::string rd);
    virtual void genMipsFromMidCode(){}  //中间代码生成mips
    void printMipsCode();  //输出mips语句
    void loadValue(const std::string& name, const std::string& regName, bool gene, int& va, bool& get, bool assign);  //取变量name的值到寄存器regName，后边的参数主要涉及到对常值的处理方式
    void storeValue(const std::string &name, const std::string &regName);  //将regName的值存储到name的空间中
private:
    mipsOperation operatoration; // 操作
};

class RTypeMipsCode: public MipsCode {
public:
    RTypeMipsCode(mipsOperation operation,const std::string &rd,const std::string &rs,const std::string& rt);
    void genMipsFromMidCode() override;
private:
    mipsOperation operatoration; // 操作
    std::string rd;     // 结果
    std::string rs;     // 左操作数
    std::string rt;     // 右操作数
};

class ITypeMipsCode: public MipsCode {
public:
    ITypeMipsCode(mipsOperation operation,const std::string &rs,const std::string &rt,const int &imm);
private:
    mipsOperation operatoration; // 操作
    std::string rs;     // 结果
    std::string rt;     // 左操作数
    int imm;     // 右操作数
};

class BLabelTypeMipsCode: public MipsCode {
public:
    BLabelTypeMipsCode(mipsOperation operation,const std::string &rs,const std::string &rt,const std::string label) ;
    BLabelTypeMipsCode(mipsOperation operation,const std::string &rs,const std::string label);
    void genMipsFromMidCode() override;
private:
    std::string rs;     // 左操作数
    std::string rt;     // 右操作数
    std::string label;
};

class JTypeMipsCode: public MipsCode {
public:
    JTypeMipsCode();
private:
    mipsOperation operatoration; // 操作
    std::string to;
};
void genMips();
void genData();
void genText();
void printMips();
void printMipsCode();
