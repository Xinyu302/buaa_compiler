//
// Created by yxy on 2020-10-20.
//

#include "ErrorInfo.h"
#include "OutputControl.h"
#include <cstdio>
ErrorInfo::ErrorInfo(int _line, Info _info): line(_line),info(_info){}

void ErrorInfo::printInfoStr()
{
    char c = info + 'a';
    fprintf(output,"%d %c\n",line,c);
}