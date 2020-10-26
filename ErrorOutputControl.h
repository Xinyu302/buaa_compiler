//
// Created by yxy on 2020-10-20.
//

#pragma once
#include <cstdio>

extern FILE * outputError;

inline void error(int line,ErrorInfo::Info info)
{
    fprintf(outputError,"%d %c\n",line,info + 'a');
}

inline void fuckHere()
{
    std::cout << "fuck" << std::endl;
}