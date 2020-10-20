//
// Created by yxy on 2020-10-20.
//
#pragma once
#include <string>
class ErrorInfo {
public:
    enum Info{
        CHAR_WRONG,
        NAME_REDEFINED,
        NAME_UNDEFINED,
        PARA_NUM_UNMATCHED,
        PARA_TYPE_UNMATCHED,
        TYPE_JUDGE_WRONG,
        VOID_FUNC_WRONG_RETURN,
        NON_VOID_FUNC_WRONG_RETURN,
        ARRAY_INDEX_NOT_NUM,
        CONST_VALUE_CHANGED,
        SEMICN_SHOULD_OCCUR,
        RPARENT_SHOULD_OCCUR,
        RBRACK_SHOULD_OCCUR,
        ARRAY_INIT_WRONG_NUM,
        CONST_TYPE_WRONG,
        DEFALUT_SHOULD_OCCUR
    };
    ErrorInfo(int line,Info info);
    void printInfoStr();
private:
    int line;
    Info info;
};



