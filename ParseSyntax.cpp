#include "ParseSyntax.h"
#include "SymbolTableItem.h"
#include "ErrorOutputControl.h"
#include "Utils.h"
#include <vector>
#include <map>
#include <set>

#define Tokens (*TokenVecPointer)
const int INNER = 1;
const int OUTER = 0;
const int FUNC_VOID = 0;
const int FUNC_INT = 1;
const int FUNC_CHAR = 2;
const int EXP_UNKNOWN = 0;
const int EXP_INT = 1;
const int EXP_CHAR = 2;
const int EXP_ERROR = 3;
// 运用递归下降的方法分析语法
ParseSyntax::ParseSyntax(std::vector<Token>& _TokenVec):TokenVec(_TokenVec){}
int nothing;
int nowLoc = 0;
int isInner = 0;
bool isLeaf = true;
int inLoop = 0;
int FUNC_TYPE;
bool FUNC_HAS_RETURN;

extern std::vector<MidCode*> midCodeVec;

/**
 * for loop condition
 */
std::string conditionL;
std::string conditionR;
Token::TokenTypeIndex conditionIndex;
//std::vector<MidCode*> repeatMidCodes;
int repeatStart;
int repeatEnd;
/**
 * end for loop condition
 */
FunctionSymbolTable* curFuncTable;
FunctionSymbolTable* globalSymbolTable;
std::vector<MidCode*>* curMidCodeVec;
std::map<std::string,FunctionSymbolTable*> FunctionSymbolTableMap;
std::map<std::string,std::vector<MidCode*>*> FunctionMidCodeMap;
std::vector<std::vector<MidCode*>*> FunctionMidCodeVec;
std::vector<std::string> FunctionNames;

std::vector<Token>* TokenVecPointer;

SyntaxSymbolTable symbolTable;
SymbolTableItem *symbolTableItemPtr;

std::map<std::string,std::string> stringMap;
std::set<std::string> stringSet;

bool notFindSymbolTableItem() {
    return symbolTableItemPtr == nullptr;
}

inline void changeFuncArea(const std::string& funcName) {
    curFuncTable = new FunctionSymbolTable(funcName);
    FunctionSymbolTableMap[funcName] = curFuncTable;
    curMidCodeVec = new std::vector<MidCode*>;
//    MidCodeFactory(MidCode::LABEL,funcName);
    FunctionMidCodeMap[funcName] = curMidCodeVec;
    FunctionMidCodeVec.push_back(curMidCodeVec);
    FunctionNames.push_back(funcName);
}

void setInner(int Inner) {
    symbolTable.setInner(Inner);
    isInner = Inner;
}

std::string applyTmpId() {
    const std::string& tmpId = getNextTmpValId();
    curFuncTable->appendTempVar(tmpId);
    return tmpId;
}

std::string applyTmpId(int value) {
    const std::string& tmpId = getNextTmpValId();
    curFuncTable->appendConst(tmpId,value);
    return tmpId;
}

void addVarTimes(const std::string &name) {
    if (curFuncTable == globalSymbolTable) return;
    if (inLoop) {
        curFuncTable->addTimes(name, 10);
    } else {
        curFuncTable->addTimes(name);
    }
}

inline int getLastLine()
{
    return Tokens[nowLoc - 1].getLine();
}

inline bool defineCheckMulti(const std::string& name) {
    return (symbolTable.defineCheckMulti(isInner,name));
}

void clearInnerSymbolTable(const std::string& idName)
{
    symbolTable.clearInnerSymbolTable(idName);
}

bool indexMatchedIntOrChar(Token::TokenTypeIndex& index, int& intOrChar)
{
    if ((index == Token::INTTK && intOrChar == EXP_INT) || (index == Token::CHARTK && intOrChar == EXP_CHAR)) return true;
    return false;
}

std::string output;

// 断言类型，如果得到满意答案loc++
inline bool typeAssert(int loc, Token::TokenTypeIndex index)
{
	return (Tokens[loc].getIndex() == index);
}

inline bool typeAssert(Token::TokenTypeIndex index)
{
	return (Tokens[nowLoc].getIndex() == index);
}

bool typeEnsure(Token::TokenTypeIndex index)
{
	bool b = typeAssert(index);
	if (b)
	{
		output += Tokens[nowLoc].TokenPrintStr();
//		std::cout << Tokens[nowLoc].TokenPrintStr() << std::endl;
		nowLoc++;
	}
	return b;
}


// <加法运算符>::= +｜-
bool Handle_PLUS(bool show,Token::TokenTypeIndex& index)
{
	if (typeEnsure(Token::PLUS))
	{
	    index = Token::PLUS;
		return true;
	}
	if (typeEnsure(Token::MINU))
	{
	    index = Token::MINU;
		return true;
	}
	return false;
}
// <乘法运算符> ::= *｜/
bool Handle_MULT(bool show,Token::TokenTypeIndex& index)
{
	if (typeEnsure(Token::MULT))
	{
	    index = Token::MULT;
		return true;
	}
	if (typeEnsure(Token::DIV))
	{
	    index = Token::DIV;
		return true;
	}
	return false;
}
// <关系运算符> ::=  <｜<=｜>｜>=｜!=｜==
bool Handle_CMP(bool show)
{
	if (typeEnsure(Token::LSS))
	{
		return true;
	}
	if (typeEnsure(Token::LEQ))
	{
		return true;
	}
	if (typeEnsure(Token::GRE))
	{
		return true;
	}
	if (typeEnsure(Token::GEQ))
	{
		return true;
	}
	if (typeEnsure(Token::NEQ))
	{
		return true;
	}
	if (typeEnsure(Token::EQL))
	{
		return true;
	}
	return false;
}
// <字符串>  ::=  "｛十进制编码为32,33,35-126的ASCII字符｝"
bool Handle_STRCON(bool show,std::string & strName)
{
	if (typeEnsure(Token::STRCON))
	{
	    strName = getNextStringId();
	    stringMap[strName] = Tokens[nowLoc-1].getTokenStr();
		output += "<字符串>";
		output += '\n';
		return true;
	}
	return false;
}
// <无符号整数> ::= <非零数字>｛<数字>｝| 0
bool Handle_UNSIGNED_INTCON(bool show,int& value=nothing)
{
	if (typeEnsure(Token::INTCON))
	{
	    value = atoi(Tokens[nowLoc - 1].getTokenStr().c_str());
		output += "<无符号整数>";
		output += '\n';
		return true;
	}
	return false;
}
// <整数> ::= ［＋｜－］<无符号整数>
bool Handle_INTCON(bool show,int &value=nothing)
{
    bool minus = false;
    Token::TokenTypeIndex index = Token::NIL;
	Handle_PLUS(show,index);
	if (index == Token::MINU) {
	    minus = true;
	}
	if (Handle_UNSIGNED_INTCON(show,value))
	{
        if (minus) value = -value;
		output += "<整数>";
		output += '\n';
		return true;
	}
	return false;
}
//<标识符>   ::=  <字母>｛<字母>｜<数字>｝
bool Handle_IDENFR(bool show)
{
	return typeEnsure(Token::IDENFR);
}

bool Handle_CHARCON(bool show,int& value=nothing)
{
	if (typeEnsure(Token::CHARCON)) {
	    value = Tokens[nowLoc - 1].getTokenStr()[0];
        return true;
	}
    return false;
}

//<声明头部>  ::=  int<标识符>|char<标识符>
bool Handle_DECLARE_HEADER(bool show,int& funcType)
{
	if (typeEnsure(Token::INTTK) || typeEnsure(Token::CHARTK))
	{
	    Token::TokenTypeIndex index = Tokens[nowLoc - 1].getIndex();
	    if (index == Token::INTTK) {
	        funcType = FUNC_INT;
        } else if (index == Token::CHARTK) {
	        funcType = FUNC_CHAR;
	    }
 		if (typeEnsure(Token::IDENFR))
		{
			output += "<声明头部>";
			output += '\n';
			return true;
		}
	}
	return false;
}
bool Handle_CONST_INT_OR_CHAR(bool show,int& const_or_char,int &value=nothing)
{
	if (Handle_INTCON(show,value))
	{
	    const_or_char = EXP_INT;
		output += "<常量>";
		output += '\n';
		return true;
	}
	else if (Handle_CHARCON(show,value))
    {
        const_or_char = EXP_CHAR;
        output += "<常量>";
        output += '\n';
        return true;
    }
	return false;
}
//<类型标识符>     ::=  int | char
bool Handle_TYPE_IDENFR(bool show)
{
	if (typeEnsure(Token::INTTK) || typeEnsure(Token::CHARTK))
	{
		return true;
	}
	return false;
}

//＜变量定义＞ ::= ＜变量定义无初始化＞|＜变量定义及初始化＞
bool assert_VAR_DEFINE_WITH_INIT()
{
	bool flag = true;
	int loc = nowLoc;
	if (!(typeAssert(loc,Token::INTTK) || typeAssert(loc,Token::CHARTK))) return false;
	loc++;
	int line = Tokens[loc].getLine();
	if (!typeAssert(loc++, Token::IDENFR)) return false;
	while (!typeAssert(loc,Token::SEMICN)) {
	    if (line != Tokens[loc].getLine()) {
            break;
	    }
        if (typeAssert(loc,Token::ASSIGN)) return true;
        loc++;
	}
	return false;
}

bool assert_RETURN_FUNC_DEFINE()
{
	bool flag = true;
	int loc = nowLoc;
	if (!(typeAssert(loc, Token::INTTK) || typeAssert(loc, Token::CHARTK))) return false;
	loc++;
	if (!typeAssert(loc++, Token::IDENFR)) return false;
	if (typeAssert(loc, Token::LPARENT)) return true;
	return false;
}

bool Handle_VAR_DEFINE_NO_INIT(bool show)
{
	if (!Handle_TYPE_IDENFR(show))
	{
		return false;
	}
	Token::TokenTypeIndex index = Tokens[nowLoc - 1].getIndex();
    int vec1,vec2;
    while (Handle_IDENFR(show))
	{
	    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	    int& line = Tokens[nowLoc - 1].getLine();
	    if (defineCheckMulti(idName)) {
	        error(getLastLine(),ErrorInfo::NAME_REDEFINED);
	    }

        if (typeEnsure(Token::LBRACK))
		{
			if (!Handle_UNSIGNED_INTCON(show)) return false;
			if (!typeEnsure(Token::RBRACK)) {
			    error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
			}
			vec1 = atoi(Tokens[nowLoc - 2].getTokenStr().c_str());
			if (typeEnsure(Token::LBRACK))
			{
				if (!Handle_UNSIGNED_INTCON(show)) return false;
                if (!typeEnsure(Token::RBRACK)) {
                    error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
                }
                vec2 = atoi(Tokens[nowLoc - 2].getTokenStr().c_str());
                curFuncTable->appendLocalVar(idName,vec1,vec2);
                symbolTable.insertIntoSymbolTableVar(isInner,idName,index,2,vec1,vec2);
			}
			else
            {
                curFuncTable->appendLocalVar(idName,vec1);
                symbolTable.insertIntoSymbolTableVar(isInner,idName,index,1,vec1);
            }
		}
		else
        {
            curFuncTable->appendLocalVar(idName);
		    symbolTable.insertIntoSymbolTableVar(isInner,idName,index);
        }
		if (!typeEnsure(Token::COMMA)) break;
	}
	output += "<变量定义无初始化>";
	output += '\n';
	return true;
}

bool Handle_VAR_DEFINE_WITH_INIT(bool show)
{
	bool flag = false;
	if (!Handle_TYPE_IDENFR(show)||!Handle_IDENFR(show))
	{
		return false;
	}
    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	int line = Tokens[nowLoc - 1].getLine();
	Token::TokenTypeIndex index = Tokens[nowLoc - 2].getIndex();

	int vec1,vec2;
	int value;
	bool hadError = false;
	if (typeEnsure(Token::LBRACK))
	{
		if (!Handle_UNSIGNED_INTCON(show)) return false;
        vec1 = atoi(Tokens[nowLoc - 1].getTokenStr().c_str());
        if (!typeEnsure(Token::RBRACK)) {
            error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
        }
		if (typeEnsure(Token::LBRACK))
		{
			if (!Handle_UNSIGNED_INTCON(show)) return false;
            vec2 = atoi(Tokens[nowLoc - 1].getTokenStr().c_str());
            if (!typeEnsure(Token::RBRACK)) {
                error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
            }
            int vec1_in = 0;
            std::vector<int>* values = new std::vector<int>;
			if (typeEnsure(Token::ASSIGN))
			{
                curFuncTable->appendLocalVar(idName,vec1,vec2);
				typeEnsure(Token::LBRACE);
				while (typeEnsure(Token::LBRACE))
				{
//                    std::string x = applyTmpId(vec1_in);
                    vec1_in++;
                    int vec2_in = 0;
                    int int_or_char = EXP_UNKNOWN;
                    while (Handle_CONST_INT_OR_CHAR(show, int_or_char, value)) {
//                        std::string y = applyTmpId(vec2_in);
//                        std::string result = applyTmpId(value);
                        values->push_back(value);
                        vec2_in++;
//                        MidCodeFactory(MidCode::ARRAYLEFTSIDE, idName, x, y, result);
                        if (!indexMatchedIntOrChar(index, int_or_char)) {
                            error(getLastLine(), ErrorInfo::CONST_TYPE_WRONG);
                        }
                        if (!typeEnsure(Token::COMMA)) break;
                    }
					typeEnsure(Token::RBRACE);
                    if (vec2_in != vec2 && !hadError)
                    {
                        hadError = true;
                        error(line,ErrorInfo::ARRAY_INIT_WRONG_NUM);
                    }
					if (!typeEnsure(Token::COMMA)) break;
				}

                if (vec1_in != vec1 && !hadError)
                {
                    error(line,ErrorInfo::ARRAY_INIT_WRONG_NUM);
                }
				flag = typeEnsure(Token::RBRACE);
                if (defineCheckMulti(idName))
                {
                    error(line,ErrorInfo::NAME_REDEFINED);
                }
                MidCodeFactory(MidCode::INITARRAY, idName, values);
                symbolTable.insertIntoSymbolTableVar(isInner,idName,index,vec1,vec2);
            }
		}
		else if (typeEnsure(Token::ASSIGN))
		{
            curFuncTable->appendLocalVar(idName,vec1);
			typeEnsure(Token::LBRACE);
			int vec1_in = 0;
            int int_or_char = EXP_UNKNOWN;
            std::vector<int>* values = new std::vector<int>;
            while (Handle_CONST_INT_OR_CHAR(show,int_or_char,value))
			{
//                std::string x = applyTmpId(vec1_in);
//                std::string result = applyTmpId(value);
//                MidCodeFactory(MidCode::ARRAYLEFTSIDE, idName, x,result);
                values->push_back(value);
                if (!indexMatchedIntOrChar(index,int_or_char)) {
                    error(getLastLine(),ErrorInfo::CONST_TYPE_WRONG);
                }
			    vec1_in++;
				if (!typeEnsure(Token::COMMA)) break;
			}
            if (vec1_in != vec1)
            {
                error(line,ErrorInfo::ARRAY_INIT_WRONG_NUM);
            }
            if (defineCheckMulti(idName))
            {
                error(line,ErrorInfo::NAME_REDEFINED);
            }
            MidCodeFactory(MidCode::INITARRAY, idName, values);
            symbolTable.insertIntoSymbolTableVar(isInner,idName,index,vec1);
			flag = typeEnsure(Token::RBRACE);
		}
	}
	else if (typeEnsure(Token::ASSIGN))
	{
        if (defineCheckMulti(idName))
        {
            error(line,ErrorInfo::NAME_REDEFINED);
        }
        curFuncTable->appendLocalVar(idName);
        symbolTable.insertIntoSymbolTableVar(isInner,idName,index);
        int int_or_char = EXP_UNKNOWN;
		flag = Handle_CONST_INT_OR_CHAR(show,int_or_char,value);
        if (!indexMatchedIntOrChar(index,int_or_char)) {
            error(getLastLine(),ErrorInfo::CONST_TYPE_WRONG);
        }
        MidCodeFactory(MidCode::ASSIGN,idName,value);
	}
	if (flag)
	{
		output += "<变量定义及初始化>";
		output += '\n';
	}
	return flag;
}

bool Handle_VAR_DEFINE(bool show)
{
	if (assert_RETURN_FUNC_DEFINE())
	{
		return false;
	}
	if (assert_VAR_DEFINE_WITH_INIT())
	{
		if (Handle_VAR_DEFINE_WITH_INIT(show))
		{
			output += "<变量定义>";
			output += '\n';
			return true;
		}
	}
	else if (Handle_VAR_DEFINE_NO_INIT(show))
	{
		output += "<变量定义>";
		output += '\n';
		return true;
	}
	return false;
}


//<变量说明> ::= <变量定义>;{<变量定义>;}
bool Handle_VAR_EXPLAIN(bool show)
{
	bool flag = false;
	while (Handle_VAR_DEFINE(show))
	{
	    int line = Tokens[nowLoc - 1].getLine();
        if (!typeEnsure(Token::SEMICN))
        {
            error(line,ErrorInfo::SEMICN_SHOULD_OCCUR);
        }
		flag = true;
	}
	if (flag)
	{
		output += "<变量说明>";
		output += '\n';
	}
	return flag;
}
//<参数表>   ::=  <类型标识符><标识符>{,<类型标识符><标识符>}| <空>
bool Handle_PARA_LIST(bool show,std::vector<SymbolTableItem::ItemReturnType>& paras)
{
	while (Handle_TYPE_IDENFR(show) && Handle_IDENFR(show))
	{
        const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
        int line = Tokens[nowLoc - 1].getLine();
        curFuncTable->appendPara(idName);
        Token::TokenTypeIndex index = Tokens[nowLoc - 2].getIndex();
        SymbolTableItem::ItemReturnType type = (index == Token::INTTK) ? SymbolTableItem::INT : SymbolTableItem::CHAR;
        if (defineCheckMulti(idName))
        {
            symbolTable.insertIntoSymbolTableVar(isInner,idName,index);
            error(line,ErrorInfo::NAME_REDEFINED);
        }
        else
        {
            symbolTable.insertIntoSymbolTableVar(isInner,idName,index);
        }
        paras.push_back(type);
		typeEnsure(Token::COMMA);
	}
	output += "<参数表>";
	output += '\n';
	return true;
}
//<项>    ::= <因子>{<乘法运算符><因子>}
bool Handle_TERM(bool show,int& isChar,std::string &varName)
{
    int charOrNot = EXP_UNKNOWN;
    bool b = false;
    bool e = false;
    std::string varNameTop;
    std::string varName2;
    int valueTop = 0;
    int value2 = 0;
    Token::TokenTypeIndex index = Token::NIL;
	if (!Handle_FACTOR(show, charOrNot,varNameTop)) return false;
    if (charOrNot == EXP_ERROR) e = true;
	while (Handle_MULT(show,index) && Handle_FACTOR(show, charOrNot,varName2)) {
	    std::string nextTmp;
	    valueTop = value2 = -1;
        bool b1 = curFuncTable->isConstValue(varNameTop, valueTop);
        bool b2 = curFuncTable->isConstValue(varName2, value2);
        if (b1 && b2) {
            if (index == Token::MULT) nextTmp = applyTmpId(valueTop * value2);
            if (index == Token::DIV) nextTmp = applyTmpId(valueTop / value2);
        }
	    else if ((valueTop == 0 || value2 == 0) && index == Token::MULT || (valueTop == 0 && index == Token::DIV)) {
	        nextTmp = applyTmpId(0);
	    }
	    else if ((valueTop == 1 || value2 == 1) && index == Token::MULT) {
	        nextTmp = (valueTop == 1) ? varName2 : varNameTop;
	    }
	    else if (value2 == 1 && index == Token::DIV) {
	        nextTmp = varNameTop;
	    }
	    else {
	        nextTmp = applyTmpId();
            if (index == Token::MULT) MidCodeFactory(MidCode::MULTI,nextTmp,varNameTop,varName2);
            if (index == Token::DIV) MidCodeFactory(MidCode::DIV,nextTmp,varNameTop,varName2);
	    }
        varNameTop = nextTmp;
	    if (charOrNot == EXP_ERROR) e = true;
	    b = true;
	}
    isChar = e ? EXP_ERROR : (b) ? EXP_INT : charOrNot;
	varName = varNameTop;
	output += "<项>";
	output += '\n';
	return true;
}
//<表达式>::=［＋｜－］<项>{<加法运算符><项>}//[+|-]只作用于第一个<项>
bool Handle_EXPRESSION(bool show,int& expType,std::string& VarName)
{
    int isChar = EXP_UNKNOWN;
    bool b = false;
    bool e = false;
    bool zeroFront = false;
    std::string varNameTop;
    std::string varName2;
    int valueTop = 0;
    int value2 = 0;
    Token::TokenTypeIndex index;
	if (Handle_PLUS(show,index)) {
	    isChar = EXP_INT;
	    if (index == Token::MINU) {
            std::string tmpVal = applyTmpId(0);
            valueTop = 0;
            varNameTop = tmpVal;
            zeroFront = true;
	    }
        b = true;
	}
	if (zeroFront) {
	    std::string nextTmp;
        if (!Handle_TERM(show,isChar,varName2)) return false;
        if (curFuncTable->isConstValue(varName2,value2)) {
            nextTmp = applyTmpId(-value2);
        } else {
            nextTmp = applyTmpId();
            MidCodeFactory(MidCode::MINUS, nextTmp, varNameTop, varName2);
        }
//        MidCodeFactory(MidCode::MINUS,nextTmp,varNameTop,varName2);
        varNameTop = nextTmp;
	} else {
        if (!Handle_TERM(show,isChar,varNameTop)) return false;
	}
	if (isChar == EXP_ERROR) {
	    e = true;
	}
	while (Handle_PLUS(show,index) && Handle_TERM(show,isChar,varName2)) {
        std::string nextTmp;
        valueTop = value2 = -1;
        bool b1 = curFuncTable->isConstValue(varNameTop, valueTop);
        if (b1 && curFuncTable->isConstValue(varName2, value2)) {
            if (index == Token::PLUS) nextTmp = applyTmpId(valueTop + value2);
            if (index == Token::MINU) nextTmp = applyTmpId(valueTop - value2);
        }
        else if ((valueTop == 0 || value2 == 0) && index == Token::PLUS) {
            nextTmp = (valueTop == 0) ? varName2 : varNameTop;
        }
        else if ((value2 == 0) && index == Token::MINU) {
            nextTmp = varNameTop;
        }
        else {
            nextTmp = applyTmpId();
            if (index == Token::PLUS) MidCodeFactory(MidCode::PLUS,nextTmp,varNameTop,varName2);
            if (index == Token::MINU) MidCodeFactory(MidCode::MINUS,nextTmp,varNameTop,varName2);
        }
        varNameTop = nextTmp;
//	    std::string nextTmp = getNextTmpValId();
//	    if (index == Token::PLUS) MidCodeFactory(MidCode::PLUS,nextTmp,varNameTop,varName2);
//	    if (index == Token::MINU) MidCodeFactory(MidCode::MINUS,nextTmp,varNameTop,varName2);
//	    varNameTop = nextTmp;
        if (isChar == EXP_ERROR) {
            e = true;
        }
	    b = true;
	}
	VarName = varNameTop;
	expType = e ? EXP_ERROR : (b) ? EXP_INT: isChar;
	output += "<表达式>";
	output += '\n';
	return true;
}

//'('<表达式>')'
bool Handle_LPARENT_EXP_RPARENT(bool show,int& exp_type,std::string& varName)
{
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_EXPRESSION(show,exp_type,varName)) {
        exp_type = EXP_ERROR;
	}
	if (!typeEnsure(Token::RPARENT)) {
	    error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
	}
    return true;
}

//＜因子＞    :: = ＜标识符＞｜＜标识符＞'['＜表达式＞']' | ＜标识符＞'['＜表达式＞']''['＜表达式＞']' | '('＜表达式＞')'｜＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞
bool Handle_FACTOR(bool show,int& ischar,std::string& varName)
{
    bool flag = false;
    int value;
    if (typeAssert(nowLoc,Token::IDENFR))
    {
        flag = true;
        if (typeAssert(nowLoc+1, Token::LBRACK))
        {
            Handle_IDENFR(show);
            const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
            const std::string &arrayTmp = applyTmpId();
            symbolTableItemPtr = symbolTable.findSymbolTableItem(idName);
            if (notFindSymbolTableItem()) {
                error(getLastLine(), ErrorInfo::NAME_UNDEFINED);
                ischar = EXP_ERROR;
            } else {
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::INT) ischar = EXP_INT;
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::CHAR) ischar = EXP_CHAR;
            }
            typeEnsure(Token::LBRACK);
            int exp_type = EXP_UNKNOWN;
            std::string expName_x;
            std::string expName_y;
            Handle_EXPRESSION(show,exp_type,expName_x);
            if (exp_type != EXP_INT) {error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);}
            if (!typeEnsure(Token::RBRACK)) {
                error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
            }
            if (typeAssert(nowLoc, Token::LBRACK))
            {
                typeEnsure(Token::LBRACK);
                Handle_EXPRESSION(show,exp_type,expName_y);
                if (exp_type != EXP_INT) {error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);}
                if (!typeEnsure(Token::RBRACK)) {
                    error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
                }
                MidCodeFactory(MidCode::ARRAYRIGHTSIDE, idName, expName_x, expName_y, arrayTmp);
            }
            else { // 1wei shuzu
                MidCodeFactory(MidCode::ARRAYRIGHTSIDE, idName, expName_x,arrayTmp);
            }
            varName = arrayTmp;
        }
        else if (typeAssert(nowLoc+1, Token::LPARENT))
        {
            varName = applyTmpId();
            curFuncTable->setIsLeaf(false);
            Handle_RETURN_FUNC_CALL(show,ischar);
            MidCodeFactory(MidCode::ASSIGN, varName, "$v0");
        }
        else
        {
            Handle_IDENFR(show);
            const std::string idName = Tokens[nowLoc - 1].getTokenStr();
            symbolTableItemPtr = symbolTable.findSymbolTableItem(idName);
            if (notFindSymbolTableItem()) {
                ischar = EXP_ERROR;
                error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
            }
            else
            {
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::INT) ischar = EXP_INT;
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::CHAR) ischar = EXP_CHAR;
                varName = idName;
                addVarTimes(varName);
            }
        }
    }
    else if (Handle_LPARENT_EXP_RPARENT(show,ischar,varName))
    {
        ischar = EXP_INT;
        flag = true;
    }
    else if (Handle_INTCON(show,value))
    {
        flag = true;
        ischar = EXP_INT;
        std::string tmpVal = applyTmpId(value);
        varName = tmpVal;
//        MidCodeFactory(MidCode::ASSIGN,tmpVal,value);
    }
    else if (Handle_CHARCON(show,value))
    {
        flag = true;
        ischar = EXP_CHAR;
        std::string tmpVal = applyTmpId(value);
        varName = tmpVal;
//        MidCodeFactory(MidCode::ASSIGN,tmpVal,value);
    }
    if (flag)
    {
        output += "<因子>";
        output += '\n';
    }
    return flag;

}

//<值参数表>  ::= <表达式>{,<表达式>}｜<空>
bool Handle_VAL_PARA_LIST(bool show, std::vector<SymbolTableItem::ItemReturnType> &paras,
                          std::vector<std::string> &paraNames, bool &should_cmp_paras) {

    int exp_type;
    std::string varName;
    int index = 0;
    if (Handle_EXPRESSION(show, exp_type, varName)) {
        if (exp_type == EXP_INT) paras.push_back(SymbolTableItem::INT);
        else if (exp_type == EXP_CHAR) paras.push_back(SymbolTableItem::CHAR);
        else if (exp_type == EXP_ERROR) should_cmp_paras = false;
//        MidCodeFactory(MidCode::PUSH, varName, index++);
        paraNames.push_back(varName);
    }
    while (typeEnsure(Token::COMMA) && Handle_EXPRESSION(show, exp_type, varName)) {
        if (exp_type == EXP_INT) paras.push_back(SymbolTableItem::INT);
        else if (exp_type == EXP_CHAR) paras.push_back(SymbolTableItem::CHAR);
        else if (exp_type == EXP_ERROR) should_cmp_paras = false;
//        MidCodeFactory(MidCode::PUSH, varName, index++);
        paraNames.push_back(varName);
    }
    output += "<值参数表>";
    output += '\n';
    return true;
}
//<有返回值函数调用语句>::= <标识符>'('<值参数表>')'
bool Handle_RETURN_FUNC_CALL(bool show,int& ischar/*,const std::string& retExpName*/)
{
    bool should_cmp_paras = true;
	if (!Handle_IDENFR(show)) return false;
	const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
//	findSymbolTableItem(idName);
    SymbolTableItem * localPtr = symbolTable.findSymbolTableItem(idName);
	if (localPtr->getReturnType() == SymbolTableItem::INT) ischar = EXP_INT;
    if (localPtr->getReturnType() == SymbolTableItem::CHAR) ischar = EXP_CHAR;
	if (!typeEnsure(Token::LPARENT)) return false;
    std::vector<SymbolTableItem::SymbolTableItem::ItemReturnType> paras;
    std::vector<std::string> paraNames;
	if (!Handle_VAL_PARA_LIST(show,paras,paraNames,should_cmp_paras)) return false;
    if (should_cmp_paras) {
        if (!localPtr->compareParaListNum(paras)) {
            error(getLastLine(),ErrorInfo::PARA_NUM_UNMATCHED);
        } else {
            if (!localPtr->compareParaList(paras)) {
                error(getLastLine(),ErrorInfo::PARA_TYPE_UNMATCHED);
            }
        }
    }
    MidCodeFactory(MidCode::CAll, idName);
    for (int i = 0; i < paraNames.size(); i++) {
        MidCodeFactory(MidCode::PUSH, paraNames[i],i);
    }
    MidCodeFactory(MidCode::JAL, idName);
	if (typeEnsure(Token::RPARENT))
	{
		output += "<有返回值函数调用语句>";
		output += '\n';
		return true;
	}
	else {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
	}
	return true;
}
////<无返回值函数调用语句>:: = <标识符>'('<值参数表>')'
bool Handle_VOID_FUNC_CALL(bool show)
{
    bool should_cmp_paras = true;
	if (!Handle_IDENFR(show)) return false;
	const std::string & idName = Tokens[nowLoc - 1].getTokenStr();
    SymbolTableItem * localPtr = symbolTable.findSymbolTableItem(idName);
    std::vector<SymbolTableItem::SymbolTableItem::ItemReturnType> paras;
    std::vector<std::string> paraNames;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_VAL_PARA_LIST(show,paras,paraNames,should_cmp_paras)) return false;
    if (should_cmp_paras) {
        if (!localPtr->compareParaListNum(paras)) {
            error(getLastLine(),ErrorInfo::PARA_NUM_UNMATCHED);
        } else {
            if (!localPtr->compareParaList(paras)) {
                error(getLastLine(),ErrorInfo::PARA_TYPE_UNMATCHED);
            }
        }
    }
    MidCodeFactory(MidCode::CAll, idName);
    for (int i = 0; i < paraNames.size(); i++) {
        MidCodeFactory(MidCode::PUSH, paraNames[i],i);
    }
    MidCodeFactory(MidCode::JAL, idName);
	if (typeEnsure(Token::RPARENT))
	{
		output += "<无返回值函数调用语句>";
		output += '\n';
		return true;
	}
    else {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    return true;
}

// <常量说明>::=  const<常量定义>;{ const<常量定义>;}
bool Handle_CONST_EXPLAIN(bool show)
{
	if (!typeEnsure(Token::CONSTTK)) return false;
	if (!Handle_CONST_DEFINE(show)) return false;
	if (!typeEnsure(Token::SEMICN)) {
	    error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
	}
	while (typeEnsure(Token::CONSTTK))
	{
		Handle_CONST_DEFINE(show);
		if (!typeEnsure(Token::SEMICN))
        {
            error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
        }
	}
	output += "<常量说明>";
	output += '\n';
	return true;
}
//<常量定义>::=int<标识符>＝<整数>{,<标识符>＝<整数>}| char<标识符>＝<字符>{ ,<标识符>＝<字符>}
bool Handle_CONST_DEFINE(bool show)
{
	bool flag = false;
	if (Handle_TYPE_IDENFR(show))
	{
        Token::TokenTypeIndex index = Tokens[nowLoc - 1].getIndex();
        if (!Handle_IDENFR(show)) return false;
		const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
        if (defineCheckMulti(idName))
        {
            error(getLastLine(),ErrorInfo::NAME_REDEFINED);
        }
        else
        {
            symbolTable.insertIntoSymbolTableConst(isInner,idName,index);
        }

		if (!typeEnsure(Token::ASSIGN)) return false;
		int value;
		if (!Handle_INTCON(show,value)&&!Handle_CHARCON(show,value)) return false;
        curFuncTable->appendConst(idName,value);
//		MidCodeFactory(MidCode::ASSIGN,idName,value);
		while (typeEnsure(Token::COMMA))
		{
			if (!Handle_IDENFR(show)) return false;
            const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
            if (defineCheckMulti(idName))
            {
                error(getLastLine(),ErrorInfo::NAME_REDEFINED);
            }
            else
            {
                symbolTable.insertIntoSymbolTableConst(isInner,idName,index);
            }
            if (!typeEnsure(Token::ASSIGN)) return false;
			if (!Handle_INTCON(show,value) && !Handle_CHARCON(show,value)) return false;
            curFuncTable->appendConst(idName,value);
//            MidCodeFactory(MidCode::ASSIGN,idName,value);
		}
	}
	output += "<常量定义>";
	output += '\n';
	return true;
}
// //<复合语句>  ::=  ［<常量说明>］［<变量说明>］<语句列>
bool Handle_COMPLEX_STATE(bool show)
{
	Handle_CONST_EXPLAIN(show);
	Handle_VAR_EXPLAIN(show);
	if (Handle_STATE_LIST(show))
	{
		output += "<复合语句>";
		output += '\n';
		return true;
	}
	return false;
}
//<有返回值函数定义> ::=  <声明头部>'('<参数表>')' '{'<复合语句>'}'
bool Handle_RETURN_FUNC_DEFINE(bool show)
{
    FUNC_HAS_RETURN = false;
    if (!Handle_DECLARE_HEADER(show,FUNC_TYPE)) return false;
    setInner(INNER);
    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
    int& line = Tokens[nowLoc - 1].getLine();
    Token::TokenTypeIndex index = Tokens[nowLoc - 2].getIndex();
    changeFuncArea(idName);
    MidCodeFactory(MidCode::FUNCOP, idName, 1);
    std::vector<SymbolTableItem::SymbolTableItem::ItemReturnType> paras;
    if (!typeEnsure(Token::LPARENT)) return false;
    if (!Handle_PARA_LIST(show,paras)) return false;
    if (symbolTable.findSymbolTableItem(OUTER,idName) != nullptr) {
        error(line,ErrorInfo::NAME_REDEFINED);
    }
    else {
        symbolTable.insertIntoSymbolTableFunc(OUTER,idName,index,paras);
    }
    if (!typeEnsure(Token::RPARENT)) {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    if (!typeEnsure(Token::LBRACE)) return false;
    if (!Handle_COMPLEX_STATE(show)) return false;
    if (!typeEnsure(Token::RBRACE)) return false;
    if (!FUNC_HAS_RETURN) {
        error(getLastLine(),ErrorInfo::NON_VOID_FUNC_WRONG_RETURN);
    }
    output += "<有返回值函数定义>";
    output += '\n';
    setInner(OUTER);
//    MidCodeFactory(MidCode::FUNCOP, idName, 0);
    clearInnerSymbolTable(idName);
    return true;
}

//<无返回值函数定义> :: = void<标识符>'('<参数表>')''{'<复合语句>'}'
bool Handle_VOID_FUNC_DEFINE(bool show)
{
    if (!typeAssert(nowLoc, Token::VOIDTK)) return false;
    if (!typeAssert(nowLoc+1, Token::IDENFR)) return false;
    if (!typeEnsure(Token::VOIDTK)) return false;
    if (!typeEnsure(Token::IDENFR)) return false;
    setInner(INNER);
    FUNC_TYPE = FUNC_VOID;
    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
    int line = Tokens[nowLoc - 1].getLine();
    changeFuncArea(idName);
    MidCodeFactory(MidCode::FUNCOP, idName, 1);
    if (!typeEnsure(Token::LPARENT)) return false;
    std::vector<SymbolTableItem::ItemReturnType> paras;
    if (!Handle_PARA_LIST(show,paras)) return false;
    if (symbolTable.findSymbolTableItem(OUTER,idName) != nullptr)
    {
        error(line,ErrorInfo::NAME_REDEFINED);
    }
    else
    {
        symbolTable.insertIntoSymbolTableFunc(OUTER,idName,Token::VOIDTK,paras);
    }
    if (!typeEnsure(Token::RPARENT)) {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    if (!typeEnsure(Token::LBRACE)) return false;
    if (!Handle_COMPLEX_STATE(show)) return false;
    if (!typeEnsure(Token::RBRACE)) return false;
    output += "<无返回值函数定义>";
    output += '\n';
    setInner(OUTER);
    MidCodeFactory(MidCode::FUNCOP, idName, 0);
    clearInnerSymbolTable(idName);
    return true;
}
//<返回语句>  ::=  return['('<表达式>')']   
bool Handle_RETURN_STATE(bool show)
{
    std::string expName;
	if (!typeEnsure(Token::RETURNTK)) return false;
	FUNC_HAS_RETURN = true;
	int exp_type = EXP_UNKNOWN;
	Handle_LPARENT_EXP_RPARENT(show,exp_type,expName);
    MidCodeFactory(MidCode::RET, expName);
    if (exp_type != FUNC_TYPE)
    {
        if (FUNC_TYPE == FUNC_VOID)
        {
            error(getLastLine(),ErrorInfo::VOID_FUNC_WRONG_RETURN);
        }
        else
        {
            error(getLastLine(),ErrorInfo::NON_VOID_FUNC_WRONG_RETURN);
        }
    }
	output += "<返回语句>";
	output += '\n';
	return true;
}

//<写语句>   ::= printf '(' <字符串>,<表达式>')'| printf '('<字符串>')'| printf '('<表达式>')'
bool Handle_PRINTF_STATE(bool show)
{
    int type = 0;
    std::string typeString;
    std::string expName;
    std::string strName;
    if (!typeEnsure(Token::PRINTFTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (Handle_STRCON(show,strName))
	{
	    const std::string& stringTmp = Tokens[nowLoc - 1].getTokenStr();
		if (typeEnsure(Token::COMMA))
		{
			if (!Handle_EXPRESSION(show,type,expName)) return false;
			typeString = (type == EXP_INT) ? "INT" : "CHAR";
            MidCodeFactory(MidCode::WRITE,expName,typeString,strName);
		}
		else
        {
            stringSet.insert(strName);
            MidCodeFactory(MidCode::WRITE,"","",strName);
        }
	}
	else
	{
		if (!Handle_EXPRESSION(show,type,expName)) return false;
        typeString = (type == EXP_INT) ? "INT" : "CHAR";
        MidCodeFactory(MidCode::WRITE,expName,typeString);
	}
	
	if (typeEnsure(Token::RPARENT))
	{
		output += "<写语句>";
		output += '\n';
		return true;
	}
    else {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    return true;
}
//<读语句>   ::=  scanf '('<标识符>')'
bool Handle_SCANF_STATE(bool show)
{
	if (!typeEnsure(Token::SCANFTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_IDENFR(show)) return false;
    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
    symbolTableItemPtr = symbolTable.findSymbolTableItem(idName);
    if (notFindSymbolTableItem())
    {
        error(getLastLine(), ErrorInfo::NAME_UNDEFINED);
    }
    else
    {
        if (symbolTableItemPtr->getClassType() == SymbolTableItem::CONST) {
            error(getLastLine(), ErrorInfo::CONST_VALUE_CHANGED);
        }
    }
    auto index = symbolTableItemPtr->getReturnType();
    std::string typeString = (index == SymbolTableItem::INT) ? "INT" :"CHAR";
    if (typeEnsure(Token::RPARENT))
	{
        MidCodeFactory(MidCode::SCAN,idName,typeString);
		output += "<读语句>";
		output += '\n';
		return true;
	}
    else {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    return true;
}
//<语句列>  ::= ｛<语句>｝
bool Handle_STATE_LIST(bool show)
{
	//if (!Handle_STATEMENT(show)) return false;
	while (Handle_STATEMENT(show));
	output += "<语句列>";
	output += '\n';
	return true;
}
/*<条件>   ::=  <表达式><关系运算符><表达式>*/
bool Handle_CONDITION(bool show,const std::string& label,int workMode,int &result=nothing)
{
    int typeL = EXP_UNKNOWN;
    int typeR = EXP_UNKNOWN;
    std::string expL;
    std::string expR;
    repeatStart = curMidCodeVec->size();
	if (Handle_EXPRESSION(show,typeL,expL))
	{
        bool not2gen = false;
		Handle_CMP(show);
        Token::TokenTypeIndex index = Tokens[nowLoc - 1].getIndex();
		Handle_EXPRESSION(show,typeR,expR);
		if (typeL != EXP_INT || typeR != EXP_INT)
        {
		    error(getLastLine(),ErrorInfo::TYPE_JUDGE_WRONG);
        }
		conditionL = expL;
		conditionR = expR;
        conditionIndex = index;
        int value1,value2;
        if (curFuncTable->isConstValue(expL, value1) && curFuncTable->isConstValue(expR, value2)) {
            not2gen = true;
        }
        repeatEnd = curMidCodeVec->size() - 1;
		if (workMode == 0) // nomal {
        {
            MidCodeFactory(index2MidCode(index), label, expL, expR);
        }
		else { // for if statement
            MidCodeFactory(reverseIndex2MidCode(index), label, expL, expR);
		}
		output += "<条件>";
		output += '\n';
		return true;
	}
	return false;
}

//<步长>::= <无符号整数> 
bool Handle_STEP(bool show,int& value)
{
	if (!Handle_UNSIGNED_INTCON(show,value)) return false;
	output += "<步长>";
	output += '\n';
	return true;
}
//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’     /*测试程序需出现情况语句*/
bool Handle_CONDITION_STATE(bool show)
{
    bool default_missed = false;
    std::string expName;
    int exp_type = EXP_UNKNOWN;
	if (!typeEnsure(Token::SWITCHTK)) return false;
	if (!Handle_LPARENT_EXP_RPARENT(show,exp_type,expName)) return false;
    setExp2Judge(expName);
	if (!typeEnsure(Token::LBRACE)) return false;
	if (!Handle_CONDITON_TABLE(show,exp_type)) return false;
	if (!Handle_DEFAULT(show)) { default_missed = true; }
	if (typeEnsure(Token::RBRACE))
	{
	    if (default_missed) {
            error(getLastLine(),ErrorInfo::DEFALUT_SHOULD_OCCUR);
	    }

		output += "<情况语句>";
		output += '\n';
		return true;
	}
	return false;
}
//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}  
bool Handle_CONDITON_TABLE(bool show,const int& exp_type)
{
	if (!Handle_CONDITION_CHILD_STATE(show,exp_type)) return false;
	while (Handle_CONDITION_CHILD_STATE(show,exp_type));
	output += "<情况表>";
	output += '\n';
	return true;
}

//＜情况子语句＞  ::=  case＜常量＞：＜语句＞
bool Handle_CONDITION_CHILD_STATE(bool show, const int &exp_type) {
    int const_or_char = EXP_UNKNOWN;
    const std::string& caseNo = getNextCaseId();
    if (!typeEnsure(Token::CASETK)) return false;
    int value = 0;
    if (!Handle_CONST_INT_OR_CHAR(show,const_or_char,value)) return false;
    const std::string& v = applyTmpId(value);
    MidCodeFactory(MidCode::NEQ, getNextCaseId(),getExp2Judge(), v);
//    int&& line = getLastLine();
    if (const_or_char != exp_type)
    {
        error(getLastLine(),ErrorInfo::CONST_TYPE_WRONG);
    }
    if (!typeEnsure(Token::COLON)) return false;
    if (Handle_STATEMENT(show)) {
        MidCodeFactory(MidCode::J, getNextSwitchEnd());
        MidCodeFactory(MidCode::LABEL, getNextCaseId(true));
        output += "<情况子语句>";
        output += '\n';
        return true;
    }
    return false;
}
//＜缺省＞   ::=  default :＜语句＞       
bool Handle_DEFAULT(bool show)
{
	if (!typeEnsure(Token::DEFAULTTK)) return false;
	if (!typeEnsure(Token::COLON)) return false;
	if (Handle_STATEMENT(show))
	{
	    MidCodeFactory(MidCode::LABEL,getNextSwitchEnd());
	    addSwitchId();
		output += "<缺省>";
		output += '\n';
		return true;
	}
	return false;
}
bool Handle_ELSE_STATE(bool show)
{
	if (!typeEnsure(Token::ELSETK)) return false;
	return Handle_STATEMENT(show);
}
//<条件语句> ::= if '('<条件>')'<语句>［else<语句>］
bool Handle_IF_STATE(bool show)
{
	if (!typeEnsure(Token::IFTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
    std::string &&elseBegin = getNextElseId();
    std::string &&elseEnd = getNextElseEndId();
    int result = 0;
	if (!Handle_CONDITION(show,elseBegin,1,result)) return false;
    if (!typeEnsure(Token::RPARENT)) {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
	if (!Handle_STATEMENT(show)) return false;
    if (typeAssert(nowLoc,Token::ELSETK)) {
        MidCodeFactory(MidCode::J, elseEnd);
    }
    MidCodeFactory(MidCode::LABEL, elseBegin);
    if (typeAssert(nowLoc,Token::ELSETK)) {
        Handle_ELSE_STATE(show);
        MidCodeFactory(MidCode::LABEL, elseEnd);
    }
	output += "<条件语句>";
	output += '\n';
	return true;

}
//＜赋值语句＞   :: = ＜标识符＞＝＜表达式＞ | ＜标识符＞'['＜表达式＞']' = ＜表达式＞ | ＜标识符＞'['＜表达式＞']''['＜表达式＞']' = ＜表达式＞
bool Handle_ASSIGN_STATE(bool show)
{
	bool flag = false;
	int type;
	std::string expName;
	if (Handle_IDENFR(show))
	{
	    int& line = Tokens[nowLoc - 1].getLine();
	    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	    symbolTableItemPtr = symbolTable.findSymbolTableItem(idName);
	    if (notFindSymbolTableItem())
        {
	        error(line,ErrorInfo::NAME_UNDEFINED);
        }
	    // 常数赋值
	    if (symbolTableItemPtr->getClassType() == SymbolTableItem::CONST)
        {
	        error(line,ErrorInfo::CONST_VALUE_CHANGED);
        }

		if (typeEnsure(Token::ASSIGN))
		{
			flag = Handle_EXPRESSION(show,type,expName);
			int value = 0;
			if (curFuncTable->isConstValue(expName,value)) {
                MidCodeFactory(MidCode::ASSIGN,idName,value);
			}
			else {
                MidCodeFactory(MidCode::ASSIGN,idName,expName);
			}
            addVarTimes(idName);
		}
		else if (typeEnsure(Token::LBRACK))
		{
		    std::string exp_x;
			if (!Handle_EXPRESSION(show,type,exp_x)) return false;
			if (type != EXP_INT) {
			    error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);
			}
            if (!typeEnsure(Token::RBRACK)) {
                error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
            }
			if (typeEnsure(Token::ASSIGN))
			{
				flag = Handle_EXPRESSION(show,type,expName);
                MidCodeFactory(MidCode::ARRAYLEFTSIDE, idName, exp_x, expName);
			}
			else if (typeEnsure(Token::LBRACK))
			{
			    std::string exp_y;
				if (!Handle_EXPRESSION(show,type,exp_y)) return false;
                if (type != EXP_INT) {
                    error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);
                }
                if (!typeEnsure(Token::RBRACK)) {
                    error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
                }
				if (typeEnsure(Token::ASSIGN))
				{
					flag = Handle_EXPRESSION(show,type,expName);
                    MidCodeFactory(MidCode::ARRAYLEFTSIDE, idName,exp_x, exp_y, expName);
				}
			}
			else return false;
		}
		else return false;
	}
	if (flag)
	{
		output += "<赋值语句>";
		output += '\n';
	}
	return flag;
}
//for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
bool Handle_FOR_STATE_PARENT(bool show) 
{
    int type;
    std::string expName;
    int value;
    const std::string& loopstart = getNextForId();
    const std::string& loopend = getNextForEnd();
    const std::string &elseBegin = getNextElseId();
    const std::string &elseEnd = getNextElseEndId();

	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_IDENFR(show)) return false;
	const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
    symbolTableItemPtr = symbolTable.findSymbolTableItem(idName);
    if (notFindSymbolTableItem()){
	    error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
	}
	if (!typeEnsure(Token::ASSIGN)) return false;
	if (!Handle_EXPRESSION(show,type,expName)) return false;
	if (curFuncTable->isConstValue(expName,value)) {
        MidCodeFactory(MidCode::ASSIGN,idName,value);
    } else {
	    MidCodeFactory(MidCode::ASSIGN,idName,expName);
	}
    addVarTimes(idName);
//    MidCodeFactory(MidCode::LABEL, elseBegin);
	if (!typeEnsure(Token::SEMICN)) {
	    error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
	}
	if (!Handle_CONDITION(show,elseBegin,1)) return false;
    Token::TokenTypeIndex cIndex = conditionIndex;
    const std::string L = conditionL;
    const std::string R = conditionR;
    int s = repeatStart;
    int e = repeatEnd;
    if (!typeEnsure(Token::SEMICN)) {
        error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
    }
    MidCodeFactory(MidCode::LABEL, loopstart);
	if (!Handle_IDENFR(show)) return false;

	const std::string& l = Tokens[nowLoc - 1].getTokenStr();
	if (!typeEnsure(Token::ASSIGN)) return false;
	if (!Handle_IDENFR(show)) return false;
    const std::string& r = Tokens[nowLoc - 1].getTokenStr();
	Token::TokenTypeIndex index;
	if (!Handle_PLUS(show,index)) return false;
	if (!Handle_STEP(show,value)) return false;
	const std::string& constTmp = applyTmpId(value);
	if (typeEnsure(Token::RPARENT))
	{
        if (!Handle_STATEMENT(show)) return false;
        if (index == Token::PLUS) {
            MidCodeFactory(MidCode::PLUS, l, r, constTmp);
        }
        else if (index == Token::MINU) {
            MidCodeFactory(MidCode::MINUS, l, r, constTmp);
        }
        for (int i = s; i <= e; i++) {
            push2Vec((*curMidCodeVec)[i]);
        }
        MidCodeFactory(index2MidCode(cIndex), loopstart, L, R);
//        MidCodeFactory(MidCode::J, loopstart);
//        MidCodeFactory(MidCode::LABEL, loopend);
    }
    else {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    MidCodeFactory(MidCode::LABEL, elseBegin);
    return true;
}

bool Handle_LOOP_STATE(bool show)
{
	bool flag = false;
	if (typeEnsure(Token::WHILETK))
	{
        inLoop++;
        const std::string& loopstart = getNextWhileId();
        const std::string& loopend = getNextWhileEnd();
        const std::string &elseBegin = getNextElseId();
        const std::string &elseEnd = getNextElseEndId();

//	    MidCodeFactory(MidCode::LABEL,loopstart);
		if (!typeEnsure(Token::LPARENT)) return false;
		if (!Handle_CONDITION(show,elseBegin,1)) return false;
        Token::TokenTypeIndex cIndex = conditionIndex;
        std::string L = conditionL;
        std::string R = conditionR;
        int s = repeatStart;
        int e = repeatEnd;
        if (!typeEnsure(Token::RPARENT)) {
            error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
        }
        MidCodeFactory(MidCode::LABEL,loopstart);
        if (Handle_STATEMENT(show)) flag = true;
        for (int i = s; i <= e; i++) {
            push2Vec((*curMidCodeVec)[i]);
        }
        MidCodeFactory(index2MidCode(cIndex), loopstart, L, R);
        MidCodeFactory(MidCode::LABEL,elseBegin);
        inLoop--;

    }
	else if (typeEnsure(Token::FORTK))
	{
        inLoop++;
		if (!Handle_FOR_STATE_PARENT(show)) return false;
        inLoop--;
        flag = true;
//		if (Handle_STATEMENT(show)) flag = true;
	}
	if (flag)
	{
		output += "<循环语句>";
		output += '\n';
	}
	return flag;
}
/*<语句>   ::= <条件语句>｜<循环语句>| '{'<语句列>'}'| <有返回值函数调用语句>;
			|<无返回值函数调用语句>;｜<赋值语句>;｜<读语句>;｜<写语句>;｜<空>;|<返回语句>;*/
bool Handle_STATEMENT(bool show)
{
	// todo 有无返回值
	int type;
	bool flag = false;
	if (Handle_IF_STATE(show)) flag = true;
	else if (Handle_LOOP_STATE(show)) flag = true;
	else if (Handle_CONDITION_STATE(show)) flag = true;
	else if (typeEnsure(Token::LBRACE))
	{
		if (!Handle_STATE_LIST(show)) return false;
		if (typeEnsure(Token::RBRACE))
		{
			flag = true;
		}
	}
	else if (Handle_SCANF_STATE(show)||Handle_PRINTF_STATE(show)||Handle_RETURN_STATE(show))
	{
		if (typeEnsure(Token::SEMICN)) flag = true;
		else
        {
		    error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
            flag = true;
        }
	}
	else if (typeEnsure(Token::SEMICN)) flag = true;
	else if (typeAssert(Token::IDENFR))
	{
		flag = true;
		const std::string& idName = Tokens[nowLoc].getTokenStr();
        symbolTableItemPtr = symbolTable.findSymbolTableItem(idName);
        if (notFindSymbolTableItem()) {
		    typeEnsure(Token::IDENFR);
            error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
		    while (!typeAssert(nowLoc,Token::SEMICN)) nowLoc++;
		}
		else {
            if (symbolTableItemPtr->getClassType() == SymbolTableItem::FUNC)
            {
                curFuncTable->setIsLeaf(false);
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::VOID)
                {
                    Handle_VOID_FUNC_CALL(show);
                }
                else {
                    Handle_RETURN_FUNC_CALL(show, type);
                }
            } else if (symbolTableItemPtr->getClassType() == SymbolTableItem::CONST ||
                       symbolTableItemPtr->getClassType() == SymbolTableItem::VAR) {
                Handle_ASSIGN_STATE(show);
            }
		}
		if (typeEnsure(Token::SEMICN)) flag = true;
		else {
            error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
            flag = true;
		}
	}
	else if (typeEnsure(Token::LBRACE))
	{
		Handle_STATE_LIST(show);
		flag = typeEnsure(Token::RBRACE);
	}
	if (flag)
	{
		output += "<语句>";
		output += '\n';
	}
	return flag;
}
//<程序>   ::= ［<常量说明>］［<变量说明>］{<有返回值函数定义>|<无返回值函数定义>}<主函数>
bool Handle_PROGRAM(bool show)
{
    isInner = OUTER;
    changeFuncArea("#global");
    MidCodeFactory(MidCode::FUNCOP, "#global", 1);
    globalSymbolTable = curFuncTable;
	Handle_CONST_EXPLAIN(show);
	Handle_VAR_EXPLAIN(show);
    MidCodeFactory(MidCode::CAll, "main");
	MidCodeFactory(MidCode::JAL,"main"); // jump to main
    MidCodeFactory(MidCode::J, "PROGRAM_END");
	while (Handle_RETURN_FUNC_DEFINE(show) || Handle_VOID_FUNC_DEFINE(show));
	if (!Handle_MAIN(show)) return false;
	output += "<程序>";
	output += '\n';
	return true;
}
/*处理主函数main*/
bool Handle_MAIN(bool show)
{
    setInner(INNER);
    FUNC_TYPE = FUNC_VOID;
    changeFuncArea("main");
    MidCodeFactory(MidCode::FUNCOP, "main", 1);
//    FunctionSymbolTableMap["main"] = curFuncTable;
    if (!typeEnsure(Token::VOIDTK)) return false;
	if (!typeEnsure(Token::MAINTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
    if (!typeEnsure(Token::RPARENT)) {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    if (!typeEnsure(Token::LBRACE)) return false;
	if (!Handle_COMPLEX_STATE(show)) return false;
	if (typeEnsure(Token::RBRACE))
	{
	    isInner = OUTER;
		output += "<主函数>";
		output += '\n';
		return true;
	}
    setInner(OUTER);
	return false;
}

void ParseSyntax::parse()
{
	TokenVecPointer = &this->TokenVec;
	Handle_PROGRAM(false);
    MidCodeFactory(MidCode::LABEL, "PROGRAM_END");
//    FILE* writeTo;
//    writeTo = fopen("output.txt", "w");
//    for (int i = 0; i < output.size(); i++)
//    {
//        fprintf(writeTo, "%c", output[i]);
//    }
}
