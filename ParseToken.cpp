#include "ParseToken.h"
#include "ErrorOutputControl.h"
#include "ErrorInfo.h"
#include <cstdio>
#define LETTERS 0
#define INTCON 1
#define CHARCON 2
#define STRCON 3
#define OTHERS 4
int line = 1;
int row = 1;

FILE* readFrom;

char nowChar;
std::string TokenStr;
int type = 0;

bool isBlank(char c)
{
    if (c == '\n') {
        line++;
        row = 0;
    }
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r');
}

bool isLetter(char c) 
{
    return (c == '_' || isalpha(c));
}

void clearToken() 
{
    TokenStr = "";
    type = 0;
}

bool isChar(char c)
{
    return isLetter(c) || isdigit(c) || c == '+' || c == '-' || c == '*' || c == '/';
}

bool isStringChar(char c)
{
    return (c >= 35 && c <= 126) || (c == 32) || (c == 33);
}

void ParseToken::errorA()
{
    error(line, ErrorInfo::CHAR_WRONG);
}

void getNextChar()
{
    nowChar = fgetc(readFrom);
    row++;
}

bool singleChar(char c)
{
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')'
        || c == '[' || c == ']' || c == '{' || c == '}' || c == ',' || c == ':'
        || c == ';' || c == ',');
}

void ParseToken::Parse() 
{
    readFrom = fopen("testfile.txt", "r");
    getNextChar();
    while (!feof(readFrom))
    {
        clearToken();
        while (isBlank(nowChar))
        {
            getNextChar();
        }
        if (feof(readFrom)) break; // important to write 
        if (isLetter(nowChar))
        {
            while (isLetter(nowChar) || isdigit(nowChar))
            {
                TokenStr += nowChar;
                getNextChar();
            }
        }
        else if (isdigit(nowChar))
        {
            while (isdigit(nowChar))
            {
                TokenStr += nowChar;
                getNextChar();
            }
            type = INTCON;
        }
        else if (nowChar == '\'')
        {
            getNextChar();
            bool notR = true;
            if (isChar(nowChar))
            {
                TokenStr += nowChar;
            }
            else
            {
                if (nowChar == '\'') notR = false;
                errorA();
            }
            if (notR) {
                getNextChar();
            }
            if (nowChar != '\'') {
                printf("error");
            }
            getNextChar();
            type = CHARCON;
        }
        else if (nowChar == '\"') {
            getNextChar();
            if (nowChar == '\"')
            {
                errorA();
            }
            while (nowChar != '\"') {
                if (isStringChar(nowChar))
                {
                    TokenStr += nowChar;
                }
                else
                {
                    errorA();
                }
                getNextChar();
            }
            getNextChar();
            type = STRCON;
        }
        else
        {
            type = OTHERS;
            if (nowChar == '>' || nowChar == '=' || nowChar == '<' || nowChar == '!') 
            {
                TokenStr += nowChar;
                getNextChar();
                if (nowChar == '=') {
                    TokenStr += nowChar;
                    getNextChar();
                }
                else 
                {
                    if (TokenStr[0] == '!') 
                    {
                        printf("error!\n");
                    }
                }
            }
            else
            {
                if (singleChar(nowChar))
                {
                    TokenStr += nowChar;
                }
                else
                {
                    printf("%d here error!\n",nowChar);
                    //break;
                }
                getNextChar();
            }
        }
        Token newToken(TokenStr, type, line);
        this->TokenVec.push_back(newToken);
    }
}

ParseToken::ParseToken(std::vector<Token>& _TokenVec): TokenVec(_TokenVec){}
