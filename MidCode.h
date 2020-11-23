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
        FUNCOP,
        PARA,
        PUSH,
        CAll,
        RET,
        VAR,
        CONST,
        BNZ,
        ARRAYLEFTSIDE,
        ARRAYRIGHTSIDE,
        J,
        JAL
    };
    enum MidCodeClass{
        CALMIDCODE,
        ASSIGNMIDCODE,
        WRITEMIDCODE,
        READMIDCODE,
        LABELMIDCODE,
        COMPAREMIDCODE,
        JUMPMIDCODE,
        PUSHMIDCODE,
        CALLMIDCODE,
        HANDLEFUNCMIDCODE,
        RETMIDCODE,
        ARRAYOPERATE
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
public:
    LabelMidCode(MidCodeOperator midCodeOperator,const std::string& label);

    void displayMidCode() override;
    std::string label;
};

class CompareMidCode : public MidCode {
public:
    std::string result;

    std::string left;
    std::string right;
    CompareMidCode(MidCode::MidCodeOperator midCodeOperator,const std::string& result,const std::string& left,const std::string& right);

    void displayMidCode() override;

};

class JumpMidCode : public MidCode {
public:
    JumpMidCode(MidCodeOperator midCodeOperator, const std::string &label);
    std::string label;
    void displayMidCode() override;
};

class CallMidCode : public MidCode {
public:
    CallMidCode(MidCodeOperator midCodeOperator, const std::string &label);
    std::string label;
    void displayMidCode() override;
};

class PushMidCode : public MidCode {
public:
    PushMidCode(MidCodeOperator midCodeOperator,const std::string exp, int index);
    int index;
    std::string exp;
    void displayMidCode() override;
};

class HandleFuncMidCode : public MidCode {
public:
    enum Operate{
        ENTER,
        OUT
    };
    HandleFuncMidCode(MidCodeOperator midCodeOperator,const std::string& funcName,int op);
    Operate operate;
    std::string funcName;
    void displayMidCode() override;
};

class RetMidCode : public MidCode {
public:
    RetMidCode(MidCodeOperator midCodeOperator,const std::string& );

    std::string expName;
    void displayMidCode() override;
};

class ArrayOperateMidCode: public MidCode {
public:
    enum dimension {
       DOUBLE,
       TRIBLE
    };
    ArrayOperateMidCode(MidCodeOperator midCodeOperator,const std::string& arrayName,const std::string& x,const std::string& y,const std::string& exp);
    ArrayOperateMidCode(MidCodeOperator midCodeOperator,const std::string& arrayName,const std::string& x,const std::string& exp);
    void displayMidCode() override;

    std::string arrayName;
    std::string x;
    std::string y;
    std::string exp;
    dimension arrayDimension;
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
            break;
        }
        case MidCode::EQL:
        case MidCode::NEQ:
        case MidCode::GRE:
        case MidCode::GEQ:
        case MidCode::LEQ:
        case MidCode::LSS:
        {
            newMidCodePtr = new CompareMidCode(midCodeOperator, result, left, right);
            break;
        }
        case MidCode::J:
        case MidCode::JAL:
        {
            newMidCodePtr = new JumpMidCode(midCodeOperator, result);
            break;
        }
        case MidCode::CAll:
        {
            newMidCodePtr = new CallMidCode(midCodeOperator, result);
            break;
        }
        case MidCode::RET:
        {
            newMidCodePtr = new RetMidCode(midCodeOperator, result);
            break;
        }
        case MidCode::ARRAYLEFTSIDE:
        case MidCode::ARRAYRIGHTSIDE:
        {
            newMidCodePtr = new ArrayOperateMidCode(midCodeOperator,result,left,right);
            break;
        }
        default:
            fprintf(stderr,"fuck,midCodeOperator Wrong");
    }
    push2Vec(newMidCodePtr);
    return newMidCodePtr;
}

static MidCode *
MidCodeFactory(MidCode::MidCodeOperator midCodeOperator, const std::string &arrayName, const std::string &x,
               const std::string &y, const std::string &exp) {
    MidCode *newMidCodePtr = new ArrayOperateMidCode(midCodeOperator, arrayName, x, y, exp);
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
        case MidCode::PUSH:
        {
            newMidCodePtr = new PushMidCode(midCodeOperator, result, left);
            break;
        }
        case MidCode::FUNCOP:
        {
            newMidCodePtr = new HandleFuncMidCode(midCodeOperator, result, left);
            break;
        }

        default:
            fprintf(stderr,"fuck,midCodeOperator Wrong");
    }
    push2Vec(newMidCodePtr);
    return newMidCodePtr;
}


//extern std::vector<MidCode*> midCodeVec;
