//
// Created by yxy on 2020-11-10.
//

#include "MipsCode.h"
#include "Utils.h"
#include "RegPool.h"
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
TRegPool* tRegPool;
std::map<std::string, TRegPool *> TRegPoolCache;


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

inline void genSaveTReg() {
    std::vector<std::string> *regs = tRegPool->reg2store();
    for (int i = 0; i < regs->size();i++) {
        genMoveVarToMem((*regs)[i], (*regs)[i]);
    }
}

inline void genRestoreTReg() {
    std::vector<std::string> *regs = tRegPool->reg2store();
    for (int i = 0; i < regs->size();i++) {
        genLw((*regs)[i], "$sp", nowFuncSymbolTable->getOffset((*regs)[i]));
        genMoveVarToMem((*regs)[i], (*regs)[i]);
    }
}

inline void enterFunc(const std::string& funcName) {
//    nowFuncSymbolTable = FunctionSymbolTableMap[funcName];
    nowFuncSymbolTable = FunctionSymbolTableMap[funcName];
    mipscodes.push_back(funcName + ":");
    if (nowFuncSymbolTable != globalSymbolTable && !nowFuncSymbolTable->getIsLeaf()) {
        genMoveVarToMem("$ra", "$ra");
    }
    if (nowFuncSymbolTable != globalSymbolTable) {
        genSaveTReg();
    }
    tRegPool = new TRegPool();
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
            std::string reg = tRegPool->checkRegByName(expName);
            if (reg.length()) {
                genMove("$v0", reg);
            } else {
                genFetchVarFromMem(expName,"$v0");
            }
        }

    }
    if (!nowFuncSymbolTable->getIsLeaf()) {
        genLw("$ra", "$sp", nowFuncSymbolTable->getOffset("$ra"));
    }
    genRestoreTReg();
    genThreeRegInstr("addiu", "$sp", "$sp", int2string(nowFuncSymbolTable->getSubOffset()));
    mipscodes.push_back("jr" + tab + "$ra");

}

void genCalMips(CalMidCode* calMidCode) {
    static std::map<MidCode::MidCodeOperator, std::string> op2string = {{MidCode::PLUS,  "addu"},
                                                                              {MidCode::MINUS, "subu"},
                                                                              {MidCode::MULTI, "mult"},
                                                                              {MidCode::DIV,   "div"}};
    int value1 = 0;
    int value2 = 0;
    int constValueIndex = 0;
    int const4Value = 0;
    bool isValue1const = false;
    std::string regLeft;
    std::string regRight;
    std::string regResult = /*(tRegPool->hasFreeReg()) ? tRegPool->allocReg(calMidCode->result) :*/ "$t0";
    std::string l = "$t0";
    std::string r = "$t1";
    if (nowFuncSymbolTable->isTmpValue(calMidCode->result) && tRegPool->hasFreeReg()) {
        regResult = tRegPool->allocReg(calMidCode->result);
    }
    MidCode::MidCodeOperator anOperator = calMidCode->getMidCodeOperator();
    if (nowFuncSymbolTable->isConstValue(calMidCode->left, value1)) {
        isValue1const = true;
        if ((value1 & (value1 - 1)) == 0 && anOperator == MidCode::MULTI) {
            constValueIndex = 1;
            const4Value = value1;
        }
        else {
            if (anOperator != MidCode::PLUS) {
                genLi("$t0", value1);
            }
        }
    } else {
        regLeft = tRegPool->checkRegByName(calMidCode->left);
        if (regLeft.length()) {
            l = regLeft;
        }
        else {
            genFetchVarFromMem(calMidCode->left,"$t0");
        }
    }

    if (nowFuncSymbolTable->isConstValue(calMidCode->right,value2)) {
        if (anOperator == MidCode::PLUS) {
            genThreeRegInstr("addiu",regResult,l,int2string(value2));
            goto genCalMipsEnd;
        }
        else if (anOperator == MidCode::MINUS) {
            genThreeRegInstr("addiu",regResult,l,int2string(-value2));
            goto genCalMipsEnd;
        } else {
            if ((value2 & (value2 - 1))== 0 && anOperator == MidCode::MULTI) {
                constValueIndex = 2;
                const4Value = value2;
            } else {
                genLi("$t1", value2);
            }
        }
    }
    else {
        regRight = tRegPool->checkRegByName(calMidCode->result);
        if (regRight.length()) {
            r = regRight;
        }
        else {
            genFetchVarFromMem(calMidCode->right,"$t1");
        }
        if (isValue1const && anOperator == MidCode::PLUS) {
            genThreeRegInstr("addiu",regRight,r,int2string(value1));
            goto genCalMipsEnd;
        }
    }
    if (anOperator == MidCode::PLUS || anOperator == MidCode::MINUS) {
        genThreeRegInstr(op2string[anOperator],regResult,l,r);
    }
    if (anOperator == MidCode::MULTI || anOperator == MidCode::DIV) {
        if (constValueIndex != 0 && anOperator == MidCode::MULTI) {
            if (constValueIndex == 1) {
                genSll(regResult, r, log2_B(const4Value)); // ok
            } else {
                genSll(regResult, l, log2_B(const4Value));
            }
        } else {
            genTwoRegInstr(op2string[anOperator],l,r);
            mipscodes.push_back("mflo" + tab + regResult);
        }
    }
    genCalMipsEnd:
    if (regResult == "$t0") {
        genMoveVarToMem(calMidCode->result,"$t0");
    }
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
        genThreeRegInstr("addiu", "$sp", "$sp", int2string(-calledFuncSymbolTable->getSubOffset()));
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
            std::string reg = tRegPool->checkRegByName(writeMidCode->num);
            if (reg.length()) {
                genMove("$a0", reg);
            }
            else {
                genFetchVarFromMem(writeMidCode->num,"$a0");
            }
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
    std::string l = "$t0";
    std::string r = "$t1";
    if (nowFuncSymbolTable->isConstValue(compareMidCode->left,value)) {
        genLi("$t0", value);
    } else {
        std::string reg = tRegPool->checkRegByName(compareMidCode->left);
        if (reg.length()) {
            l = reg;
        } else {
            genFetchVarFromMem(compareMidCode->left, "$t0");
        }
    }
    if (nowFuncSymbolTable->isConstValue(compareMidCode->right,value)) {
        genLi("$t1", value);
    } else {
        std::string reg = tRegPool->checkRegByName(compareMidCode->right);
        if (reg.length()) {
            r = reg;
        } else {
            genFetchVarFromMem(compareMidCode->right, "$t1");
        }
    }
    genThreeRegInstr(cmpOp, l, r, compareMidCode->result);
}

void genCallMips(CallMidCode* callMidCode) {
    calledFuncSymbolTable = FunctionSymbolTableMap[callMidCode->label];
}

void genPushMips(PushMidCode* pushMidCode) {
    int value;
    std::string reg2push = "$t0";
    if (nowFuncSymbolTable->isConstValue(pushMidCode->exp,value)) {
        genLi("$t0", value);
    } else {
        std::string reg = tRegPool->checkRegByName(pushMidCode->exp);
        if (reg.length()) {
            reg2push = reg;
        } else {
            genFetchVarFromMem(pushMidCode->exp, "$t0");
        }
    }
    genSw(reg2push, "$sp",
          calledFuncSymbolTable->getParaOffsetByIndex(pushMidCode->index) - calledFuncSymbolTable->getSubOffset());
}

void genHandleFuncMips(HandleFuncMidCode* handleFuncMidCode) {
    if (handleFuncMidCode->operate == HandleFuncMidCode::OUT) {
        outFunc();
    } else {
        enterFunc(handleFuncMidCode->funcName);
    }
}

void genAssignRegFromExp(const std::string& reg,const std::string& exp) {
    int value;
    if (nowFuncSymbolTable->isConstValue(exp,value)) {
        genLi(reg, value);
    } else {
        std::string reg1 = tRegPool->checkRegByName(exp);
        if (reg.length()) {
            genMove(reg, reg1);
        } else {
            genFetchVarFromMem(exp, reg);
        }
    }
}

void genRetMips(RetMidCode* retMidCode) {
    outFunc(retMidCode->expName);
}

void genSll(const std::string& result,const std::string& from,int imm) {
    genThreeRegInstr("sll",result, from, int2string(imm));
}

void genArrayEleOff(const std::string &reg, const std::string &arrayName, const std::string &x, const std::string &y = "") {
    bool global = false;
    int offset;
    int value_x;
    int value_y;
    bool x_const = false;
    bool y_const = false;
    std::string base;

    if (y.length()) {
        int y_num = nowFuncSymbolTable->getArrayY(arrayName);
        if (nowFuncSymbolTable->isConstValue(x,value_x)) {
            x_const = true;
        } else{
            genAssignRegFromExp("$t0",x);
            genSll("$t0", "$t0", 2);
        }

        if (y_num < 0 || nowFuncSymbolTable == globalSymbolTable) {
            global = true;
            y_num = globalSymbolTable->getArrayY(arrayName);

        }
        offset = (global) ? globalSymbolTable->getOffset(arrayName) : nowFuncSymbolTable->getOffset(arrayName);
        base = (global) ? "$gp" : "$sp";
        int x_offset = -1;
        int y_offset = -1;
        if (x_const) {
            x_offset = value_x * y_num * 4;
        } else {
            if (is_n_two(y_num)) {
                genSll("$t0", "$t0", log2_B(y_num));
            }
            else {
                genLi("$t1", y_num);
                mipscodes.push_back("mult" + tab + "$t0" + tab + "$t1");
                mipscodes.push_back("mflo" + tab + "$t0");
            }
        }
        if (nowFuncSymbolTable->isConstValue(y,value_y)) {
            y_const = true;
        } else {
            genAssignRegFromExp("$t1",y);
            genSll("$t1", "$t1", 2);
        }
        if (y_const) {
            if (x_const) {
//                x_offset += 4 * value_y;
//                genLi("$t1", offset + x_offset + 4 * value_y);
                genThreeRegInstr("addi", "$t1", base, int2string(offset + x_offset + 4 * value_y));
            } else {
                genThreeRegInstr("addi", "$t1", "$t0", int2string(offset + 4 * value_y));
                genThreeRegInstr("addu", "$t1", "$t1", base);
            }
        } else {
            if (x_const) {
                genThreeRegInstr("addi", "$t1", "$t1", int2string(offset + x_offset));
                genThreeRegInstr("addu", "$t1", "$t1", base);
            }
            else {
                genThreeRegInstr("addi", "$t1", "$t1", int2string(offset));
                genThreeRegInstr("addu","$t1","$t1","$t0");
                genThreeRegInstr("addu", "$t1", "$t1", base);
            }
        }


    } else {
        if (nowFuncSymbolTable->isConstValue(x,value_x)) {
            x_const = true;
        } else{
            genAssignRegFromExp("$t0",x);
            genSll("$t0", "$t0", 2);
        }
        offset = nowFuncSymbolTable->getOffset(arrayName);
        if (offset < 0 || nowFuncSymbolTable == globalSymbolTable) {
            offset = globalSymbolTable->getOffset(arrayName);
            global = true;
        }
        base = (global) ? "$gp" : "$sp";
        if (x_const) {
            genThreeRegInstr("addi", "$t1", base, int2string(offset + 4 * value_x));
        } else {
            genThreeRegInstr("addi", "$t1", "$t0", int2string(offset));
            genThreeRegInstr("addu", "$t1", "$t1", base);
        }

    }
}

void genArrayOperateMips(ArrayOperateMidCode* arrayOperateMidCode) {
    if (arrayOperateMidCode->getMidCodeOperator() == MidCode::ARRAYLEFTSIDE) {
        if (arrayOperateMidCode->arrayDimension == ArrayOperateMidCode::DOUBLE) {
            genArrayEleOff("$t1", arrayOperateMidCode->arrayName, arrayOperateMidCode->x);
            genAssignRegFromExp("$t0",arrayOperateMidCode->exp);
            genSw("$t0", "$t1", 0);
        }
        else {
            genArrayEleOff("$t1", arrayOperateMidCode->arrayName, arrayOperateMidCode->x,arrayOperateMidCode->y);
            genAssignRegFromExp("$t0",arrayOperateMidCode->exp);
            genSw("$t0", "$t1", 0);
        }
    } else if (arrayOperateMidCode->getMidCodeOperator() == MidCode::ARRAYRIGHTSIDE) {
        if (arrayOperateMidCode->arrayDimension == ArrayOperateMidCode::DOUBLE) {
////            genAssignRegFromExp("$t0",arrayOperateMidCode->exp);
//            genAssignRegFromExp("$t1",arrayOperateMidCode->x);
//            genSll("$t1", "$t1", 2);
//            int offset = nowFuncSymbolTable->getOffset(arrayOperateMidCode->arrayName);
//            genThreeRegInstr("addi", "$t1", "$t1", int2string(offset));
//            genThreeRegInstr("addu", "$t1", "$t1", "$sp");
            genArrayEleOff("$t1", arrayOperateMidCode->arrayName, arrayOperateMidCode->x);
            genLw("$t0", "$t1", 0);
            genMoveVarToMem(arrayOperateMidCode->exp, "$t0");
        } else if (arrayOperateMidCode->arrayDimension == ArrayOperateMidCode::TRIBLE) {
            genArrayEleOff("$t1", arrayOperateMidCode->arrayName, arrayOperateMidCode->x,arrayOperateMidCode->y);
            genLw("$t0", "$t1", 0);
            genMoveVarToMem(arrayOperateMidCode->exp, "$t0");
        }
    } else {
        fprintf(stderr,"error at genArrayOperateMips\n");
    }
}

void genInitArray(InitArrayMidCode* initArrayMidCode)
{
    const std::string &arrayName = initArrayMidCode->arrayName;
    int offset = nowFuncSymbolTable->getOffset(arrayName);
    bool global = false;
    if (offset < 0 || nowFuncSymbolTable == globalSymbolTable) {
        offset = globalSymbolTable->getOffset(arrayName);
        global = true;
    }
    std::string base = (global) ? "$gp" : "$sp";
    std::vector<int> *values = initArrayMidCode->values;
    for (int i = 0, j = 0; i < values->size(); i++, j += 4) {
        if ((*values)[i] == 0) {
            genSw("$zero", base, offset + j);
        } else {
            genLi("$t0", (*values)[i]);
            genSw("$t0", base, offset + j);
        }
    }
    delete values;
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
                case MidCode::ARRAYOPERATE:
                {
                    genArrayOperateMips((ArrayOperateMidCode *) midCodeVec[j]);
                    break;
                }
                case MidCode::INITARRAYMIDCODE:
                {
                    genInitArray((InitArrayMidCode *) midCodeVec[j]);
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

