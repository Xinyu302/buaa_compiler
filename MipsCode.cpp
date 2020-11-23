//
// Created by yxy on 2020-11-10.
//

#include "MipsCode.h"
#include "Utils.h"
#include <vector>
#include <fstream>
#define midCodeVec (*nowMidCodeVec)

extern FunctionSymbolTable* globalSymbolTable;
//extern FunctionSymbolTable* nowFuncSymbolTable;
std::vector<MidCode*>* nowMidCodeVec;
extern std::vector<std::vector<MidCode*>*> FunctionMidCodeVec;
extern std::map<std::string,std::string> stringMap;
extern std::vector<std::string> FunctionNames;

std::ofstream mipstream("mips.txt");
std::vector<std::string> mipscodes;
const std::string syscall = "syscall";
const std::string tab = "\t";
extern std::map<std::string,FunctionSymbolTable*> FunctionSymbolTableMap;
FunctionSymbolTable* nowFuncSymbolTable;
FunctionSymbolTable* calledFuncSymbolTable;

inline void genLi(const std::string& regTo,int imm) {
    mipscodes.push_back("li" + tab + regTo + "," + tab + int2string(imm));
}

inline void genSyscallByNum(int num) {
//    mipscodes.push_back("li" + tab + "$v0" + "," + tab + int2string(num));
    genLi("$v0",num);
    mipscodes.push_back("syscall");
}

inline void genLw(const std::string& regTo, const std::string& regBase, int offset) {
    mipscodes.push_back("lw" + tab + regTo + "," + tab + int2string(offset) + "(" + regBase + ")");
}

inline void genSw(const std::string& regFrom,const std::string& regBase, int offset) {
    mipscodes.push_back("sw" + tab + regFrom + "," + tab + int2string(offset) + "(" + regBase + ")");
}

inline void genMove(const std::string& regTo,const std::string& regFrom) {
    mipscodes.push_back("move" + tab + regTo + "," + tab +regFrom);
}

inline void genMoveVarToMem(const std::string& name,const std::string& reg) {
    int offset = nowFuncSymbolTable->getOffset(name);
    if (nowFuncSymbolTable == globalSymbolTable) {
        genSw(reg, "$gp", offset);
        return;
    }
    if (offset < 0) {
        offset = globalSymbolTable->getOffset(name);
        genSw(reg, "$gp", offset);
    }
    else {
        genSw(reg,"$sp",offset);
    }

}

inline void genFetchVarFromMem(const std::string& name,const std::string& reg) {
    int offset = nowFuncSymbolTable->getOffset(name);
    if (nowFuncSymbolTable == globalSymbolTable) {
        genLw(reg, "$gp", offset);
        return;
    }
    if (offset < 0) {
        offset = globalSymbolTable->getOffset(name);
        genLw(reg, "$gp", offset);
    }
    else {
        genLw(reg,"$sp",offset);
    }
}

inline void genThreeRegInstr(const std::string& instr,const std::string& result,const std::string& reg1,const std::string& reg2) {
    mipscodes.push_back(instr + tab + result + "," + tab + reg1 + "," + tab + reg2);
}

inline void genTwoRegInstr(const std::string &instr, const std::string &reg1, const std::string &reg2) {
    mipscodes.push_back(instr + tab+ reg1 + "," + tab + reg2);
}

inline void enterFunc(const std::string& funcName) {
//    nowFuncSymbolTable = FunctionSymbolTableMap[funcName];
    nowFuncSymbolTable = FunctionSymbolTableMap[funcName];
    mipscodes.push_back(funcName + ":");
    if (nowFuncSymbolTable != globalSymbolTable) {
        genMoveVarToMem("$ra", "$ra");
    }
//    if (nowFuncSymbolTable == globalSymbolTable) {
//        ;;
//    }
//    else {
////        genLi("$t0",nowFuncSymbolTable->getSubOffset());
////        genThreeRegInstr("subu","$sp","$sp","$t0");
//    }
}

inline void outFunc(const std::string &expName = "") {
    if (expName.size()) {
        int value;
        if (nowFuncSymbolTable->isConstValue(expName, value)) {
            genLi("$v0", value);
        }
        else {
            genFetchVarFromMem(expName,"$v0");
        }

    }
    genLw("$ra", "$sp", nowFuncSymbolTable->getOffset("$ra"));
    genThreeRegInstr("addi", "$sp", "$sp", int2string(nowFuncSymbolTable->getSubOffset()));
    mipscodes.push_back("jr" + tab + "$ra");

}

void genCalMips(CalMidCode* calMidCode) {
    static std::map<MidCode::MidCodeOperator, std::string> op2string = {{MidCode::PLUS,  "addu"},
                                                                              {MidCode::MINUS, "subu"},
                                                                              {MidCode::MULTI, "mult"},
                                                                              {MidCode::DIV,   "div"}};
    int value1 = 0;
    int value2 = 0;
    if (nowFuncSymbolTable->isConstValue(calMidCode->left,value1)) {
        genLi("$t1", value1);
    } else {
        genFetchVarFromMem(calMidCode->left,"$t1");
    }
    if (nowFuncSymbolTable->isConstValue(calMidCode->right,value2)) {
        if (calMidCode->getMidCodeOperator() == MidCode::PLUS) {
            genThreeRegInstr("addi","$t0","$t1",int2string(value2));
            goto genCalMipsEnd;
        }
        else if (calMidCode->getMidCodeOperator() == MidCode::MINUS) {
            genThreeRegInstr("addi","$t0","$t1",int2string(-value2));
            goto genCalMipsEnd;
        } else {
            genLi("$t2", value2);
        }
    }
    else {
        genFetchVarFromMem(calMidCode->right, "$t2");
    }
    if (calMidCode->getMidCodeOperator() == MidCode::PLUS || calMidCode->getMidCodeOperator() == MidCode::MINUS) {
        genThreeRegInstr(op2string[calMidCode->getMidCodeOperator()],"$t0","$t1","$t2");
    }
    if (calMidCode->getMidCodeOperator() == MidCode::MULTI || calMidCode->getMidCodeOperator() == MidCode::DIV) {
        genTwoRegInstr(op2string[calMidCode->getMidCodeOperator()],"$t1","$t2");
        mipscodes.push_back("mflo" + tab + "$t0");
    }
    genCalMipsEnd:
    genMoveVarToMem(calMidCode->result,"$t0");
}

void genScanMips(ReadMidCode* readMidCode)
{
    // TODO:
    /*
     * li $v0 5
     * syscall
     */
    if (readMidCode->getReadType() == ReadMidCode::INT) {
        genSyscallByNum(5);
    }

    if (readMidCode->getReadType() == ReadMidCode::CHAR) {
        genSyscallByNum(12);
    }
    genMoveVarToMem(readMidCode->result, "$v0");
}

void genEnter() {
    mipscodes.push_back("la"+tab+"$a0,"+tab+"stringEnter");
    genSyscallByNum(4);
}

void genLabel(LabelMidCode* labelMidCode) {
    mipscodes.push_back(labelMidCode->label + ":");
}

void genJump(JumpMidCode* jumpMidCode) {
    std::string op;
    if (jumpMidCode->getMidCodeOperator() == MidCode::JAL) {
        op = "jal";
        genThreeRegInstr("addi", "$sp", "$sp", int2string(-calledFuncSymbolTable->getSubOffset()));
        mipscodes.push_back(op + tab + jumpMidCode->label);
//        genMoveVarToMem("$ra", "$ra");
    }
    else if (jumpMidCode->getMidCodeOperator() == MidCode::J) {
        op = "j";
        mipscodes.push_back(op + tab + jumpMidCode->label);
    }
}

void genPrintMips(WriteMidCode* writeMidCode) {
    if ((writeMidCode->str).length()) {
        mipscodes.push_back("la" + tab + "$a0," + tab + writeMidCode->str);
        genSyscallByNum(4);
    }
    if ((writeMidCode->num).length()) {
        int value;
        if (nowFuncSymbolTable->isConstValue(writeMidCode->num,value)) {
            genLi("$a0",value);
        }
        else {
            genFetchVarFromMem(writeMidCode->num,"$a0");
        }
        if (writeMidCode->getWriteType() == WriteMidCode::INT) {
            genSyscallByNum(1);
        }
        if (writeMidCode->getWriteType() == WriteMidCode::CHAR) {
            genSyscallByNum(11);
        }
    }
    genEnter();
}

void genAssignMips(AssignMidCode* assignMidCode) {
    if (assignMidCode->getAssignType() == AssignMidCode::IMMEDIATE) {
        genLi("$t0",assignMidCode->leftImmediate);
        genMoveVarToMem(assignMidCode->result,"$t0");
    }
    if (assignMidCode->getAssignType() == AssignMidCode::VAL) {
        if (assignMidCode->leftVal == "$v0") { // drectly load $v0 to temp var result
            genMoveVarToMem(assignMidCode->result,"$v0");
            return;
        }
        else {
            genFetchVarFromMem(assignMidCode->leftVal,"$t1");
            genMove("$t0","$t1");
            genMoveVarToMem(assignMidCode->result,"$t0");
        }
    }
}

void genCompareMips(CompareMidCode* compareMidCode) {
    std::string cmpOp;
    static std::map<MidCode::MidCodeOperator, std::string> opMap = {{MidCode::EQL, "beq"},
                                                                    {MidCode::NEQ, "bne"},
                                                                    {MidCode::GRE, "bgt"},
                                                                    {MidCode::GEQ, "bge"},
                                                                    {MidCode::LSS, "blt"},
                                                                    {MidCode::LEQ, "ble"}};
    cmpOp = opMap[compareMidCode->getMidCodeOperator()];
    int value;
    std::cout << compareMidCode->right << std::endl;
    if (nowFuncSymbolTable->isConstValue(compareMidCode->left,value)) {
        genLi("$t0", value);
    } else {
        genFetchVarFromMem(compareMidCode->left, "$t0");
    }
    if (nowFuncSymbolTable->isConstValue(compareMidCode->right,value)) {
        genLi("$t1", value);
    } else {
        genFetchVarFromMem(compareMidCode->right, "$t1");
    }
    genThreeRegInstr(cmpOp, "$t0", "$t1", compareMidCode->result);
}

void genCallMips(CallMidCode* callMidCode) {
    calledFuncSymbolTable = FunctionSymbolTableMap[callMidCode->label];
}

void genPushMips(PushMidCode* pushMidCode) {
    int value;
    if (nowFuncSymbolTable->isConstValue(pushMidCode->exp,value)) {
        genLi("$t0", value);
    } else {
        genFetchVarFromMem(pushMidCode->exp, "$t0");
    }
    genSw("$t0", "$sp",
          calledFuncSymbolTable->getParaOffsetByIndex(pushMidCode->index) - calledFuncSymbolTable->getSubOffset());
}

void genHandleFuncMips(HandleFuncMidCode* handleFuncMidCode) {
    if (handleFuncMidCode->operate == HandleFuncMidCode::OUT) {
        outFunc();
    } else {
        enterFunc(handleFuncMidCode->funcName);
    }
}

void genRetMips(RetMidCode* retMidCode) {
    outFunc(retMidCode->expName);
}

inline void genMips() {
    genData();
//    enterFunc("main");
    genText();
}

void genData() {
    const std::string quotation = "\"";

    const std::string colon = ":";
    const std::string asciiz = ".asciiz";

    mipscodes.push_back(".data");
    mipscodes.push_back("stringEnter:\t.asciiz" + tab + quotation + "\\n" + quotation);
    for (std::map<std::string,std::string>::iterator it = stringMap.begin();it != stringMap.end();it++) {
        mipscodes.push_back(it->first + colon + tab + asciiz + tab + quotation + changeString(it->second) + quotation);
    }
    /*
    for (int i = 0; i < allStringList.size();i++)
    {
        mipscodes.push_back(allStringList[i].first + colon + asciiz + tab + quotation + allStringList[i].second + quotation);
    }
    */
}

void genText() {
    mipscodes.push_back(".text");
    for (int i = 0; i < FunctionMidCodeVec.size();i++) {
        nowMidCodeVec = FunctionMidCodeVec[i];
//        enterFunc(FunctionNames[i]);
        for (int j = 0; j < midCodeVec.size();j++) {
            midCodeVec[j]->displayMidCode();
            switch (midCodeVec[j]->getMidCodeClass()) {
                case MidCode::CALMIDCODE:
                {
                    genCalMips((CalMidCode *)(midCodeVec[j]));
                    break;
                }
                case MidCode::READMIDCODE:
                {
                    genScanMips((ReadMidCode *)(midCodeVec[j]));
                    break;
                }
                case MidCode::WRITEMIDCODE:
                {
                    genPrintMips((WriteMidCode *)(midCodeVec[j]));
                    break;
                }
                case MidCode::ASSIGNMIDCODE:
                {
                    genAssignMips((AssignMidCode *)(midCodeVec[j]));
                    break;
                }
                case MidCode::LABELMIDCODE:
                {
                    genLabel((LabelMidCode*)midCodeVec[j]);
                    break;
                }
                case MidCode::JUMPMIDCODE:
                {
                    genJump((JumpMidCode *) midCodeVec[j]);
                    break;
                }
                case MidCode::COMPAREMIDCODE:
                {
                    genCompareMips((CompareMidCode *) midCodeVec[j]);
                    break;
                }
                case MidCode::PUSHMIDCODE:
                {
                    genPushMips((PushMidCode*) midCodeVec[j]);
                    break;
                }
                case MidCode::CALLMIDCODE:
                {
                    genCallMips((CallMidCode*) midCodeVec[j]);
                    break;
                }
                case MidCode::HANDLEFUNCMIDCODE:
                {
                    genHandleFuncMips((HandleFuncMidCode *) midCodeVec[j]);
                    break;
                }
                case MidCode::RETMIDCODE:
                {
                    genRetMips((RetMidCode *) midCodeVec[j]);
                    break;
                }
                default:
                    fprintf(stderr,"fuck!,dipatch wrong on %d",midCodeVec[i]->getMidCodeClass());
            }
        }
//        outFunc(FunctionNames[i]);
    }
}

void printMips()
{
    for (int i = 0; i < mipscodes.size();i++)
    {
        mipstream << mipscodes[i] << std::endl;
    }
}

void printMipsCode() {
    genMips();
    printMips();
}
