//
// Created by yxy on 2020-11-10.
//

#pragma once
#include <vector>
#include <string>
#include "SymbolTableItem.h"

class MidCode {
public:
    enum MidCodeOperator {
        PLUS,
        MINUS,
        MULTI,
        DIV,
        LSS,
        LEQ,
        GRE,
        GEQ,
        EQL,
        NEQ,
        ASSIGN,
        SCAN,
        WRITE,
        LABEL,
        FUNC,
        PARA,
        PUSH,
        CAll,
        RET,
        VAR,
        CONST,
        BNZ,
        BZ
    };
    enum MidCodeClass{
        CALMIDCODE,
        ASSIGNMIDCODE,
        WRITEMIDCODE,
        READMIDCODE,
        LABELMIDCODE,
        COMPAREMIDCODE
    };

    MidCode(MidCodeOperator midCodeOperator,MidCodeClass midCodeClass);

    MidCodeClass getMidCodeClass() {
        return midCodeClass;
    }

    MidCodeOperator getMidCodeOperator() {
        return midCodeOperator;
    }

    virtual void displayMidCode() = 0;
private:
    MidCodeOperator midCodeOperator;
    MidCodeClass midCodeClass;
};

class CalMidCode: public MidCode {
public:
    CalMidCode(MidCodeOperator midCodeOperator, const std::string& left, const std::string& right, const std::string& result);
    void displayMidCode() override;

    std::string left;
    std::string right;
    std::string result;
};

class AssignMidCode: public MidCode {
public:
    enum AssignType {
        IMMEDIATE,
        VAL
    };
    AssignMidCode(MidCodeOperator codeOperator,const std::string& result,const std::string left);
    AssignMidCode(MidCodeOperator codeOperator,const std::string& result,const int& left);
    void displayMidCode() override;
    AssignType getAssignType();

    AssignType assignType;
    std::string result;
    std::string leftVal;
    int leftImmediate;
};

class ReadMidCode: public MidCode {
public:
    enum ReadType{
        CHAR,
        INT
    };
    ReadMidCode(MidCodeOperator midCodeOperator,const std::string &result,const std::string &readType);
    void displayMidCode() override;
    ReadType getReadType();

    ReadType readType;
    std::string result;
};

class WriteMidCode: public MidCode {
public:
    enum WriteType{
        CHAR,
        INT
    };

    void displayMidCode() override;
    WriteType getWriteType();
    WriteMidCode(MidCodeOperator midCodeOperator ,const std::string& num,const std::string& writeType,const std::string& str);

    WriteType rightWriteType;
    std::string num;
    std::string str;
};

class LabelMidCode : public MidCode {
    std::string label;
public:
    LabelMidCode(MidCodeOperator midCodeOperator,const std::string& label);

    void displayMidCode() override;
};

class CompareMidCode : public MidCode {
public:
    bool result;

    std::string left;
    std::string right;
    CompareMidCode(MidCode::MidCodeOperator midCodeOperator,const std::string& left,const std::string& right);

    void displayMidCode() override;

};


void push2Vec(MidCode* midCode);

static MidCode* MidCodeFactory(MidCode::MidCodeOperator midCodeOperator,const std::string& result="",const std::string& left="",const std::string& right="")
{
    MidCode *newMidCodePtr;
    switch (midCodeOperator)
    {
        case MidCode::PLUS:
        case MidCode::MINUS:
        case MidCode::MULTI:
        case MidCode::DIV:
        {
            newMidCodePtr =  new CalMidCode(midCodeOperator, left, right, result);
            break;
        }
        case MidCode::ASSIGN: {
            newMidCodePtr = new AssignMidCode(midCodeOperator, result, left);
            break;
        }
        case MidCode::SCAN:
        {
            newMidCodePtr = new ReadMidCode(midCodeOperator, result, left);
            break;
        }
        case MidCode::WRITE:
        {
            newMidCodePtr = new WriteMidCode(midCodeOperator, result, left, right);
            break;
        }
        case MidCode::LABEL:
        {
            newMidCodePtr = new LabelMidCode(midCodeOperator, result);
        }
        case MidCode::EQL:
        case MidCode::NEQ:
        case MidCode::GRE:
        case MidCode::GEQ:
        case MidCode::LEQ:
        case MidCode::LSS:
        {

        }
        default:
            fprintf(stderr,"fuck,midCodeOperator Wrong");
    }
    push2Vec(newMidCodePtr);
    return newMidCodePtr;
}

static MidCode* MidCodeFactory(MidCode::MidCodeOperator midCodeOperator,const std::string& result,const int& left)
{
    MidCode* newMidCodePtr;
    switch (midCodeOperator) {
        case MidCode::ASSIGN:
        {
            newMidCodePtr =  new AssignMidCode(midCodeOperator,result,left);
            break;
        }
        default:
            fprintf(stderr,"fuck,midCodeOperator Wrong");
    }
    push2Vec(newMidCodePtr);
    return newMidCodePtr;
}


//extern std::vector<MidCode*> midCodeVec;
