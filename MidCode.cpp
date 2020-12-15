//
// Created by yxy on 2020-11-10.
//

#include <fstream>
#include <iostream>
#include "MidCode.h"
std::ofstream midout("midcode.txt");
extern FunctionSymbolTable* curFuncTable;
extern std::vector<MidCode*>* curMidCodeVec;
MidCode::MidCode(MidCode::MidCodeOperator midCodeOperator,MidCodeClass midCodeClass) {
    this->midCodeOperator = midCodeOperator;
    this->midCodeClass = midCodeClass;
}

AssignMidCode::AssignMidCode(MidCode::MidCodeOperator codeOperator, const std::string &result, const std::string left)
        : MidCode(codeOperator, ASSIGNMIDCODE) {
    this->result = result;
    this->leftVal = left;
    this->assignType = VAL;
}

AssignMidCode::AssignMidCode(MidCode::MidCodeOperator codeOperator, const std::string &result, const int &left)
        : MidCode(codeOperator, ASSIGNMIDCODE) {
    this->result = result;
    this->leftImmediate = left;
    this->assignType = IMMEDIATE;
}

void AssignMidCode::displayMidCode() {
    midout <<  this->result << "=";
    if (this->assignType == IMMEDIATE)
    {
        midout << this->leftImmediate << std::endl;
    }
    else if (this->assignType == VAL)
    {
        midout << this->leftVal << std::endl;
    }
}

AssignMidCode::AssignType AssignMidCode::getAssignType() {
    return this->assignType;
}


ReadMidCode::ReadMidCode(MidCodeOperator midCodeOperator, const std::string &result, const std::string &readType)
        : MidCode(midCodeOperator, READMIDCODE) {
    this->result = result;
    this->readType = (readType == "INT") ? INT : CHAR;
}

void ReadMidCode::displayMidCode() {
    midout << "read" << "\t" << result << std::endl;;
}

ReadMidCode::ReadType ReadMidCode::getReadType() {
    return this->readType;
}

CalMidCode::CalMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &left, const std::string &right,
                       const std::string &result) : MidCode(midCodeOperator, CALMIDCODE) {
    this->left = left;
    this->result = result;
    this->right = right;
}

void CalMidCode::displayMidCode() {
    char c;
    switch (this->getMidCodeOperator()) {
        case MidCode::PLUS: {
            c = '+';
            break;
        }
        case MidCode::MINUS: {
            c = '-';
            break;
        }
        case MidCode::MULTI: {
            c = '*';
            break;
        }
        case MidCode::DIV: {
            c = '/';
            break;
        }
    }
    midout << result << "=\t" << left << "\t" << c << "\t" << right << std::endl;;
}


WriteMidCode::WriteMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &num,
                           const std::string &writeType, const std::string &str): MidCode(midCodeOperator, WRITEMIDCODE) {
    this->num = num;
    this->str = str;
    this->rightWriteType = (writeType == "INT") ? INT : CHAR;
}

void WriteMidCode::displayMidCode() {
    midout << "write" << "\t" << str << "\t" << num << std::endl;
}

WriteMidCode::WriteType WriteMidCode::getWriteType() {
    return this->rightWriteType;
}

void push2Vec(MidCode* midCode) {
    curMidCodeVec->push_back(midCode);
}


LabelMidCode::LabelMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &label) : MidCode(midCodeOperator, LABELMIDCODE) {
    this->label = label;
}

void LabelMidCode::displayMidCode() {
    midout << this->label << ":" << std::endl;
}

void CompareMidCode::displayMidCode() {
    static std::map<MidCode::MidCodeOperator, std::string> opMap = {{MidCode::EQL, "beq"},
                                                                    {MidCode::NEQ, "bne"},
                                                                    {MidCode::GRE, "bgt"},
                                                                    {MidCode::GEQ, "bge"},
                                                                    {MidCode::LSS, "blt"},
                                                                    {MidCode::LEQ, "ble"}};
    midout << opMap[this->getMidCodeOperator()] << "\t" << this->left << "\t" << this->right << "\t" << this->result << std::endl;
}

CompareMidCode::CompareMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &result,
                               const std::string &left, const std::string &right):MidCode(midCodeOperator,COMPAREMIDCODE) {
    this->result = result;
    this->left = left;
    this->right = right;
}

JumpMidCode::JumpMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &label) : MidCode(midCodeOperator,JUMPMIDCODE) {
    this->label = label;
}

void JumpMidCode::displayMidCode() {
    switch (this->getMidCodeOperator()) {
        case J:
        {
            midout << "J\t";
            break;
        }
        case JAL:
        {
            midout << "JAL\t";
            break;
        }
        default:
            return;
    }
    midout << label << std::endl;
}

PushMidCode::PushMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string exp, int index):MidCode(midCodeOperator,PUSHMIDCODE) {
    this->exp = exp;
    this->index = index;
}

void PushMidCode::displayMidCode() {
    midout << "push" << "\tno." << index << "para\t" << exp << std::endl;
}

CallMidCode::CallMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &label):MidCode(midCodeOperator,CALLMIDCODE) {
    this->label = label;
}

void CallMidCode::displayMidCode() {
    midout << "call" << "\t" << label << std::endl;
}

HandleFuncMidCode::HandleFuncMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string& funcName, int op) : MidCode(midCodeOperator,HANDLEFUNCMIDCODE) {
    this->operate = (op == 1) ? ENTER : OUT;
    this->funcName = funcName;
}

void HandleFuncMidCode::displayMidCode() {
    std::string op = (operate == ENTER) ? "enter" : "out";
    midout << op << "\t" << funcName << std::endl;
}

RetMidCode::RetMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &exp): MidCode(midCodeOperator,RETMIDCODE) {
    this->expName = exp;
}

void RetMidCode::displayMidCode() {
    midout << "return" << "\t" << expName << std::endl;
}

ArrayOperateMidCode::ArrayOperateMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &arrayName,
                                         const std::string &x, const std::string &y, const std::string &exp) : MidCode(midCodeOperator,ARRAYOPERATE){
    this->arrayName = arrayName;
    this->x = x;
    this->y = y;
    this->exp = exp;
    this->arrayDimension = TRIBLE;
}

ArrayOperateMidCode::ArrayOperateMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &arrayName,
                                         const std::string &x, const std::string &exp) : MidCode(midCodeOperator,ARRAYOPERATE){
    this->arrayName = arrayName;
    this->x = x;
    this->exp = exp;
    this->arrayDimension = DOUBLE;
}

void ArrayOperateMidCode::displayMidCode() {
    if (arrayDimension == DOUBLE) {
        if (this->getMidCodeOperator() == ARRAYLEFTSIDE) {
            midout << arrayName << "[" << x << "]\t=\t" << exp << std::endl;
        } else {
            midout << exp << "\t=\t" << arrayName << "[" << x << "]" << std::endl;
        }
    } else {
        if (this->getMidCodeOperator() == ARRAYLEFTSIDE) {
            midout << arrayName << "[" << x << "][" << y << "]\t=\t" << exp << std::endl;
        } else {
            midout << exp << "\t=\t" << arrayName << "[" << x << "][" << y << "]\t" << std::endl;
        }
    }
}

InitArrayMidCode::InitArrayMidCode(MidCode::MidCodeOperator midCodeOperator, const std::string &arrayName,
                                   std::vector<int> *values) : MidCode(midCodeOperator,INITARRAYMIDCODE){
    this->values = values;
    this->arrayName = arrayName;
}

void InitArrayMidCode::displayMidCode() {

}
