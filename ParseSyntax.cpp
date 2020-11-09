#include "ParseSyntax.h"
#include "SymbolTableItem.h"
#include "ErrorOutputControl.h"
#include <iostream>
#include <vector>

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
int nowLoc = 0;
int isInner = 0;
int FUNC_TYPE;
bool FUNC_HAS_RETURN;

std::vector<Token>* TokenVecPointer;
std::vector<SymbolTableItem> OuterSymbolTable;
std::vector<SymbolTableItem> InnerSymbolTable;
SymbolTableItem *symbolTableItemPtr;

int getLastLine()
{
    return Tokens[nowLoc - 1].getLine();
}

void InsertPtrIntoTable(int inner, const SymbolTableItem *itemPtr) {
    if (inner == INNER)
    {
        InnerSymbolTable.push_back(*itemPtr);
    }
    else
    {
        OuterSymbolTable.push_back(*itemPtr);
    }
}

bool insertIntoSymbolTableVar(int inner, const std::string& name, Token::TokenTypeIndex index, int dimension = 0, int x = 0, int y = 0)
{
    SymbolTableItem* itemPtr;
    if (index == Token::INTTK)
    {
        itemPtr = new SymbolTableItem(name,SymbolTableItem::VAR,SymbolTableItem::INT,dimension,x,y);
    }
    else if (index == Token::CHARTK)
    {
        itemPtr = new SymbolTableItem(name,SymbolTableItem::VAR,SymbolTableItem::CHAR,dimension,x,y);
    }
    InsertPtrIntoTable(inner, itemPtr);
    delete itemPtr;
    return true;
}

bool insertIntoSymbolTableConst(int inner, const std::string& name, Token::TokenTypeIndex index)
{
    SymbolTableItem* itemPtr;
    if (index == Token::INTTK)
    {
        itemPtr = new SymbolTableItem(name,SymbolTableItem::CONST,SymbolTableItem::INT);
    }
    else if (index == Token::CHARTK)
    {
        itemPtr = new SymbolTableItem(name, SymbolTableItem::CONST, SymbolTableItem::CHAR);
    }
    InsertPtrIntoTable(inner, itemPtr);
    delete itemPtr;
    return true;
}

bool insertIntoSymbolTableFunc(int inner, const std::string& name, Token::TokenTypeIndex index,const std::vector<SymbolTableItem::ItemReturnType>& s)
{
    SymbolTableItem* itemPtr;
    if (index == Token::INTTK)
    {
        itemPtr = new SymbolTableItem(name,SymbolTableItem::FUNC,SymbolTableItem::INT,s);
    }
    else if (index == Token::CHARTK)
    {
        itemPtr = new SymbolTableItem(name, SymbolTableItem::FUNC, SymbolTableItem::CHAR,s);
    }
    else if (index == Token::VOIDTK)
    {
        itemPtr = new SymbolTableItem(name, SymbolTableItem::FUNC, SymbolTableItem::VOID,s);
    }
    InsertPtrIntoTable(inner, itemPtr);
    delete itemPtr;
    return true;
}

bool findSymbolTableItem(int inner, const std::string& name)
{
    if (inner == INNER)
    {
        for (int i = InnerSymbolTable.size() - 1; i >= 0; i--) {
            if (InnerSymbolTable[i].compareName(name) == true)
            {
                symbolTableItemPtr = &(InnerSymbolTable[i]);
                return true;
            }
        }
        return false;
    }
    else
    {
        for (int i = OuterSymbolTable.size() - 1; i >= 0; i--) {
            if (OuterSymbolTable[i].compareName(name) == true)
            {
                symbolTableItemPtr = &(OuterSymbolTable[i]);
                return true;
            }
        }
    }
    return false;
}

bool defineCheckMulti(const std::string& name) {
    return findSymbolTableItem(isInner,name);
}

bool findSymbolTableItem(const std::string& name)
{
    if (isInner)
    {
        if (findSymbolTableItem(INNER,name) || findSymbolTableItem(OUTER,name)) return true;
        return false;
    }
    return findSymbolTableItem(OUTER,name);
}

void clearInnerSymbolTable()
{
    InnerSymbolTable.clear();
}

bool indexMatchedIntOrChar(Token::TokenTypeIndex& index, int& intOrChar)
{
    if (index == Token::INTTK && intOrChar == EXP_INT || index == Token::CHARTK && intOrChar == EXP_CHAR) return true;
    return false;
}

std::string output;

void ParseSyntax::printAllTokens()
{
	for (int i = 0; i < TokenVec.size(); i++) {
		std::cout << TokenVec[i].TokenPrintStr();
	}
}
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
		nowLoc++;
	}
	return b;
}


// <加法运算符>::= +｜-
bool Handle_PLUS(bool show = 0)
{
	if (typeEnsure(Token::PLUS))
	{
		return true;
	}
	if (typeEnsure(Token::MINU))
	{
		return true;
	}
	return false;
}
// <乘法运算符> ::= *｜/
bool Handle_MULT(bool show)
{
	if (typeEnsure(Token::MULT))
	{
		return true;
	}
	if (typeEnsure(Token::DIV))
	{
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
bool Handle_STRCON(bool show)
{
	if (typeEnsure(Token::STRCON))
	{
		output += "<字符串>";
		output += '\n';
		return true;
	}
	return false;
}
// <无符号整数> ::= <非零数字>｛<数字>｝| 0
bool Handle_UNSIGNED_INTCON(bool show)
{
	if (typeEnsure(Token::INTCON))
	{
		output += "<无符号整数>";
		output += '\n';
		return true;
	}
	return false;
}
// <整数> ::= ［＋｜－］<无符号整数>
bool Handle_INTCON(bool show)
{
	Handle_PLUS();
	if (Handle_UNSIGNED_INTCON(show))
	{
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

bool Handle_CHARCON(bool show)
{
	return typeEnsure(Token::CHARCON);
}

bool Handle_CONSTTK(bool show)
{
	return typeEnsure(Token::CONSTTK);
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
bool Handle_CONST_INT_OR_CHAR(bool show,int& const_or_char)
{
	if (Handle_INTCON(show))
	{
	    const_or_char = EXP_INT;
		output += "<常量>";
		output += '\n';
		return true;
	}
	else if (Handle_CHARCON(show))
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
	int&& line = getLastLine();
	if (!typeAssert(loc++, Token::IDENFR)) return false;
	while (!typeAssert(loc,Token::SEMICN)) {
	    if (line != getLastLine()) {
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
                insertIntoSymbolTableVar(isInner,idName,index,2,vec1,vec2);
			}
			else
            {
                insertIntoSymbolTableVar(isInner,idName,index,1,vec1);
            }
		}
		else
        {
		    /*
		    if (findSymbolTableItem(isInner,idName))
            {
		        error(line,ErrorInfo::NAME_REDEFINED);
            }
            */
		    insertIntoSymbolTableVar(isInner,idName,index);
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
    const std::string idName = Tokens[nowLoc - 1].getTokenStr();
	int& line = Tokens[nowLoc - 1].getLine();
	Token::TokenTypeIndex index = Tokens[nowLoc - 2].getIndex();
	int vec1,vec2;
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

			if (typeEnsure(Token::ASSIGN))
			{
				typeEnsure(Token::LBRACE);
				while (typeEnsure(Token::LBRACE))
				{
                    vec1_in++;
                    int vec2_in = 0;
                    int int_or_char = EXP_UNKNOWN;
					while (Handle_CONST_INT_OR_CHAR(show, int_or_char))
					{
					    vec2_in++;
					    if (!indexMatchedIntOrChar(index,int_or_char)) {
					        error(getLastLine(),ErrorInfo::CONST_TYPE_WRONG);
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
                insertIntoSymbolTableVar(isInner,idName,index,vec1,vec2);
            }
		}
		else if (typeEnsure(Token::ASSIGN))
		{
			typeEnsure(Token::LBRACE);
			int vec1_in = 0;
            int int_or_char = EXP_UNKNOWN;
			while (Handle_CONST_INT_OR_CHAR(show,int_or_char))
			{
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
            insertIntoSymbolTableVar(isInner,idName,index,vec1);
			flag = typeEnsure(Token::RBRACE);
		}
	}
	else if (typeEnsure(Token::ASSIGN))
	{
        if (defineCheckMulti(idName))
        {
            error(line,ErrorInfo::NAME_REDEFINED);
        }
        insertIntoSymbolTableVar(isInner,idName,index);
        int int_or_char = EXP_UNKNOWN;
		flag = Handle_CONST_INT_OR_CHAR(show,int_or_char);
        if (!indexMatchedIntOrChar(index,int_or_char)) {
            error(getLastLine(),ErrorInfo::CONST_TYPE_WRONG);
        }
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
	    int& line = Tokens[nowLoc - 1].getLine();
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
        int& line = Tokens[nowLoc - 1].getLine();
        Token::TokenTypeIndex index = Tokens[nowLoc - 2].getIndex();
        if (findSymbolTableItem(isInner,idName))
        {
            insertIntoSymbolTableVar(isInner,idName,index);
            error(line,ErrorInfo::NAME_REDEFINED);
        }
        else
        {
            insertIntoSymbolTableVar(isInner,idName,index);
        }
//        insertIntoSymbolTableVar(isInner, idName, index);
        paras.push_back(InnerSymbolTable.back().getReturnType());
		typeEnsure(Token::COMMA);
	}
	output += "<参数表>";
	output += '\n';
	return true;
}
//<项>    ::= <因子>{<乘法运算符><因子>}
bool Handle_TERM(bool show,int& isChar)
{
    int charOrNot = EXP_UNKNOWN;
    bool b = false;
    bool e = false;
	if (!Handle_FACTOR(show, charOrNot)) return false;
    if (charOrNot == EXP_ERROR) e = true;
	while (Handle_MULT(show) && Handle_FACTOR(show, charOrNot)) {
	    if (charOrNot == EXP_ERROR) e = true;
	    b = true;
	}
    isChar = e ? EXP_ERROR : (b) ? EXP_INT : charOrNot;
	output += "<项>";
	output += '\n';
	return true;
}
//<表达式>::=［＋｜－］<项>{<加法运算符><项>}//[+|-]只作用于第一个<项>
bool Handle_EXPRESSION(bool show,int& expType)
{
    int isChar = EXP_UNKNOWN;
    bool b = false;
    bool e = false;
	if (Handle_PLUS(show)) {
	    isChar = EXP_INT;
        b = true;
	}
	if (!Handle_TERM(show,isChar)) return false;
	if (isChar == EXP_ERROR) {
	    e = true;
	}
	while (Handle_PLUS(show) && Handle_TERM(show,isChar)) {
        if (isChar == EXP_ERROR) {
            e = true;
        }
	    b = true;
	}
	expType = e ? EXP_ERROR : (b) ? EXP_INT: isChar;
	output += "<表达式>";
	output += '\n';
	return true;
}
//'('<表达式>')'
bool Handle_LPARENT_EXP_RPARENT(bool show,int& exp_type)
{
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_EXPRESSION(show,exp_type)) {
        exp_type = EXP_ERROR;
	}
	if (!typeEnsure(Token::RPARENT)) {
	    error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
	}
    return true;
}
//<字符>::=  '<加法运算符>'｜'<乘法运算符>'｜'<字母>'｜'<数字>'
bool Handle_CHAR(bool show)
{
	return typeEnsure(Token::CHARCON);
}

//＜因子＞    :: = ＜标识符＞｜＜标识符＞'['＜表达式＞']' | ＜标识符＞'['＜表达式＞']''['＜表达式＞']' | '('＜表达式＞')'｜＜整数＞ | ＜字符＞｜＜有返回值函数调用语句＞
bool Handle_FACTOR(bool show,int& ischar)
{
    bool flag = false;
    if (typeAssert(nowLoc,Token::IDENFR))
    {
        flag = true;
        if (typeAssert(nowLoc+1, Token::LBRACK))
        {
            Handle_IDENFR(show);
            const std::string idName = Tokens[nowLoc - 1].getTokenStr();
            if (!findSymbolTableItem(idName)) {
                error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
                ischar = EXP_ERROR;
            }
            else {
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::INT) ischar = EXP_INT;
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::CHAR) ischar = EXP_CHAR;
            }
            typeEnsure(Token::LBRACK);
            int exp_type = EXP_UNKNOWN;
            Handle_EXPRESSION(show,exp_type);
            if (exp_type != EXP_INT) {error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);}
            if (!typeEnsure(Token::RBRACK)) {
                error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
            }
            if (typeAssert(nowLoc, Token::LBRACK))
            {
                typeEnsure(Token::LBRACK);
                Handle_EXPRESSION(show,exp_type);
                if (exp_type != EXP_INT) {error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);}
                if (!typeEnsure(Token::RBRACK)) {
                    error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
                }
            }
        }
        else if (typeAssert(nowLoc+1, Token::LPARENT))
        {

            Handle_RETURN_FUNC_CALL(show,ischar);
        }
        else
        {
            Handle_IDENFR(show);
            const std::string idName = Tokens[nowLoc - 1].getTokenStr();
            if (!findSymbolTableItem(idName)) {
                ischar = EXP_ERROR;
                error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
            }
            else
            {
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::INT) ischar = EXP_INT;
                if (symbolTableItemPtr->getReturnType() == SymbolTableItem::CHAR) ischar = EXP_CHAR;
            }
        }
    }
    else if (Handle_LPARENT_EXP_RPARENT(show,ischar))
    {
        ischar = EXP_INT;
        flag = true;
    }
    else if (Handle_INTCON(show))
    {
        flag = true;
        ischar = EXP_INT;
    }
    else if (Handle_CHARCON(show))
    {
        flag = true;
        ischar = EXP_CHAR;
    }
    if (flag)
    {
        output += "<因子>";
        output += '\n';
    }
    return flag;

}
//<值参数表>  ::= <表达式>{,<表达式>}｜<空>
bool Handle_VAL_PARA_LIST(bool show,std::vector<SymbolTableItem::ItemReturnType>& paras,bool& should_cmp_paras)
{
    int exp_type;
	if (Handle_EXPRESSION(show,exp_type))
    {
	    if (exp_type == EXP_INT) paras.push_back(SymbolTableItem::INT);
	    else if (exp_type == EXP_CHAR) paras.push_back(SymbolTableItem::CHAR);
	    else if (exp_type == EXP_ERROR) should_cmp_paras = false;
    }
	while (typeEnsure(Token::COMMA) && Handle_EXPRESSION(show,exp_type))
    {
        if (exp_type == EXP_INT) paras.push_back(SymbolTableItem::INT);
        else if (exp_type == EXP_CHAR) paras.push_back(SymbolTableItem::CHAR);
        else if (exp_type == EXP_ERROR) should_cmp_paras = false;
    }
	output += "<值参数表>";
	output += '\n';
	return true;
}
//<有返回值函数调用语句>::= <标识符>'('<值参数表>')'
bool Handle_RETURN_FUNC_CALL(bool show,int& ischar)
{
    bool should_cmp_paras = true;
	if (!Handle_IDENFR(show)) return false;
	const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	findSymbolTableItem(idName);
    SymbolTableItem * localPtr = symbolTableItemPtr;
	if (localPtr->getReturnType() == SymbolTableItem::INT) ischar = EXP_INT;
    if (localPtr->getReturnType() == SymbolTableItem::CHAR) ischar = EXP_CHAR;
	if (!typeEnsure(Token::LPARENT)) return false;
    std::vector<SymbolTableItem::SymbolTableItem::ItemReturnType> paras;
	if (!Handle_VAL_PARA_LIST(show,paras,should_cmp_paras)) return false;
    if (should_cmp_paras) {
        if (!localPtr->compareParaListNum(paras)) {
            error(getLastLine(),ErrorInfo::PARA_NUM_UNMATCHED);
        } else {
            if (!localPtr->compareParaList(paras)) {
                error(getLastLine(),ErrorInfo::PARA_TYPE_UNMATCHED);
            }
        }
    }
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
    findSymbolTableItem(idName);
    SymbolTableItem * localPtr = symbolTableItemPtr;
    std::vector<SymbolTableItem::SymbolTableItem::ItemReturnType> paras;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_VAL_PARA_LIST(show,paras,should_cmp_paras)) return false;
    if (should_cmp_paras) {
        if (!localPtr->compareParaListNum(paras)) {
            error(getLastLine(),ErrorInfo::PARA_NUM_UNMATCHED);
        } else {
            if (!localPtr->compareParaList(paras)) {
                error(getLastLine(),ErrorInfo::PARA_TYPE_UNMATCHED);
            }
        }
    }
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
            insertIntoSymbolTableConst(isInner,idName,index);
        }
		if (!typeEnsure(Token::ASSIGN)) return false;
		if (!Handle_INTCON(show)&&!Handle_CHARCON(show)) return false;
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
                insertIntoSymbolTableConst(isInner,idName,index);
            }
            if (!typeEnsure(Token::ASSIGN)) return false;
			if (!Handle_INTCON(show) && !Handle_CHARCON(show)) return false;
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
    isInner = INNER;
    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
    int& line = Tokens[nowLoc - 1].getLine();
    Token::TokenTypeIndex index = Tokens[nowLoc - 2].getIndex();
    std::vector<SymbolTableItem::SymbolTableItem::ItemReturnType> paras;
    if (!typeEnsure(Token::LPARENT)) return false;
    if (!Handle_PARA_LIST(show,paras)) return false;
    if (findSymbolTableItem(OUTER,idName)) {
        error(line,ErrorInfo::NAME_REDEFINED);
    }
    else {
        insertIntoSymbolTableFunc(OUTER,idName,index,paras);
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
    isInner = OUTER;
    clearInnerSymbolTable();
    return true;
}

//<无返回值函数定义> :: = void<标识符>'('<参数表>')''{'<复合语句>'}'
bool Handle_VOID_FUNC_DEFINE(bool show)
{
    if (!typeAssert(nowLoc, Token::VOIDTK)) return false;
    if (!typeAssert(nowLoc+1, Token::IDENFR)) return false;
    if (!typeEnsure(Token::VOIDTK)) return false;
    if (!typeEnsure(Token::IDENFR)) return false;
    FUNC_TYPE = FUNC_VOID;
    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
    int line = Tokens[nowLoc - 1].getLine();
    isInner = true;
    if (!typeEnsure(Token::LPARENT)) return false;
    std::vector<SymbolTableItem::ItemReturnType> paras;
    if (!Handle_PARA_LIST(show,paras)) return false;
    if (findSymbolTableItem(OUTER,idName))
    {
        error(line,ErrorInfo::NAME_REDEFINED);
    }
    else
    {
        insertIntoSymbolTableFunc(OUTER,idName,Token::VOIDTK,paras);
    }
    if (!typeEnsure(Token::RPARENT)) {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    if (!typeEnsure(Token::LBRACE)) return false;
    if (!Handle_COMPLEX_STATE(show)) return false;
    if (!typeEnsure(Token::RBRACE)) return false;
    output += "<无返回值函数定义>";
    output += '\n';
    isInner = false;
    clearInnerSymbolTable();
    return true;
}
//<返回语句>  ::=  return['('<表达式>')']   
bool Handle_RETURN_STATE(bool show)
{
	if (!typeEnsure(Token::RETURNTK)) return false;
	FUNC_HAS_RETURN = true;
	int exp_type = EXP_UNKNOWN;
	Handle_LPARENT_EXP_RPARENT(show,exp_type);
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
    if (!typeEnsure(Token::PRINTFTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (Handle_STRCON(show))
	{
		if (typeEnsure(Token::COMMA))
		{
			if (!Handle_EXPRESSION(show,type)) return false;
		}
	}
	else
	{
		if (!Handle_EXPRESSION(show,type)) return false;
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
    const std::string idName = Tokens[nowLoc - 1].getTokenStr();
    if (!findSymbolTableItem(idName))
    {
        error(getLastLine(), ErrorInfo::NAME_UNDEFINED);
    }
    else
    {
        if (symbolTableItemPtr->getClassType() == SymbolTableItem::CONST) {
            error(getLastLine(), ErrorInfo::CONST_VALUE_CHANGED);
        }
    }
	if (typeEnsure(Token::RPARENT))
	{
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
bool Handle_CONDITION(bool show)
{
    int typeL = EXP_UNKNOWN;
    int typeR = EXP_UNKNOWN;
	if (Handle_EXPRESSION(show,typeL))
	{
		Handle_CMP(show);
		Handle_EXPRESSION(show,typeR);
		if (typeL != EXP_INT || typeR != EXP_INT)
        {
		    error(getLastLine(),ErrorInfo::TYPE_JUDGE_WRONG);
        }
		output += "<条件>";
		output += '\n';
		return true;
	}
	return false;
}

//<步长>::= <无符号整数> 
bool Handle_STEP(bool show)
{
	if (!Handle_UNSIGNED_INTCON(show)) return false;
	output += "<步长>";
	output += '\n';
	return true;
}
//＜情况语句＞  ::=  switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’     /*测试程序需出现情况语句*/
bool Handle_CONDITION_STATE(bool show)
{
    bool default_missed = false;
    int exp_type = EXP_UNKNOWN;
	if (!typeEnsure(Token::SWITCHTK)) return false;
	if (!Handle_LPARENT_EXP_RPARENT(show,exp_type)) return false;
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
    if (!typeEnsure(Token::CASETK)) return false;
    if (!Handle_CONST_INT_OR_CHAR(show,const_or_char)) return false;
//    int&& line = getLastLine();
    if (const_or_char != exp_type)
    {
        error(getLastLine(),ErrorInfo::CONST_TYPE_WRONG);
    }
    if (!typeEnsure(Token::COLON)) return false;
    if (Handle_STATEMENT(show)) {
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
	if (!Handle_CONDITION(show)) return false;
    if (!typeEnsure(Token::RPARENT)) {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
	if (!Handle_STATEMENT(show)) return false;
	Handle_ELSE_STATE(show);
	output += "<条件语句>";
	output += '\n';
	return true;

}
//＜赋值语句＞   :: = ＜标识符＞＝＜表达式＞ | ＜标识符＞'['＜表达式＞']' = ＜表达式＞ | ＜标识符＞'['＜表达式＞']''['＜表达式＞']' = ＜表达式＞
bool Handle_ASSIGN_STATE(bool show)
{
	bool flag = false;
	int type;
	if (Handle_IDENFR(show))
	{
	    int& line = Tokens[nowLoc - 1].getLine();
	    const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	    if (!findSymbolTableItem(idName))
        {
	        error(line,ErrorInfo::NAME_UNDEFINED);
        }
	    if (symbolTableItemPtr->getClassType() == SymbolTableItem::CONST)
        {
	        error(line,ErrorInfo::CONST_VALUE_CHANGED);
        }
		if (typeEnsure(Token::ASSIGN))
		{
			flag = Handle_EXPRESSION(show,type);
		}
		else if (typeEnsure(Token::LBRACK))
		{
			if (!Handle_EXPRESSION(show,type)) return false;
			if (type != EXP_INT) {
			    error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);
			}
            if (!typeEnsure(Token::RBRACK)) {
                error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
            }
			if (typeEnsure(Token::ASSIGN))
			{
				flag = Handle_EXPRESSION(show,type);
			}
			else if (typeEnsure(Token::LBRACK))
			{
				if (!Handle_EXPRESSION(show,type)) return false;
                if (type != EXP_INT) {
                    error(getLastLine(),ErrorInfo::ARRAY_INDEX_NOT_NUM);
                }
                if (!typeEnsure(Token::RBRACK)) {
                    error(getLastLine(),ErrorInfo::RBRACK_SHOULD_OCCUR);
                }
				if (typeEnsure(Token::ASSIGN))
				{
					flag = Handle_EXPRESSION(show,type);
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
//＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞     
bool Handle_FOR_STATE_PARENT(bool show) 
{
    int type;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_IDENFR(show)) return false;
	const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	if (!findSymbolTableItem(idName)) {
	    error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
	}
	if (!typeEnsure(Token::ASSIGN)) return false;
	if (!Handle_EXPRESSION(show,type)) return false;
	if (!typeEnsure(Token::SEMICN)) {
	    error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
	}
	if (!Handle_CONDITION(show)) return false;
    if (!typeEnsure(Token::SEMICN)) {
        error(getLastLine(),ErrorInfo::SEMICN_SHOULD_OCCUR);
    }
	if (!Handle_IDENFR(show)) return false;
	if (!typeEnsure(Token::ASSIGN)) return false;
	if (!Handle_IDENFR(show)) return false;
	if (!Handle_PLUS(show)) return false;
	if (!Handle_STEP(show)) return false;
	
	if (typeEnsure(Token::RPARENT))
	{
		return true;
	}
    else {
        error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
    }
    return true;
}

bool Handle_LOOP_STATE(bool show)
{
	bool flag = false;
	if (typeEnsure(Token::WHILETK))
	{
		if (!typeEnsure(Token::LPARENT)) return false;
		if (!Handle_CONDITION(show)) return false;
        if (!typeEnsure(Token::RPARENT)) {
            error(getLastLine(),ErrorInfo::RPARENT_SHOULD_OCCUR);
        }
		if (Handle_STATEMENT(show)) flag = true;
	}
	else if (typeEnsure(Token::FORTK))
	{
		if (!Handle_FOR_STATE_PARENT(show)) return false;
		if (Handle_STATEMENT(show)) flag = true;
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
		if (!findSymbolTableItem(idName)) {
		    typeEnsure(Token::IDENFR);
            error(getLastLine(),ErrorInfo::NAME_UNDEFINED);
		    while (!typeAssert(nowLoc,Token::SEMICN)) nowLoc++;
		}
		else {
            if (symbolTableItemPtr->getClassType() == SymbolTableItem::FUNC)
            {
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
	Handle_CONST_EXPLAIN(show);
	Handle_VAR_EXPLAIN(show);
	while (Handle_RETURN_FUNC_DEFINE(show) || Handle_VOID_FUNC_DEFINE(show));
	if (!Handle_MAIN(show)) return false;
	output += "<程序>";
	output += '\n';
	return true;
}
/*处理主函数main*/
bool Handle_MAIN(bool show)
{
    isInner = INNER;
    FUNC_TYPE = FUNC_VOID;
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
	return false;
}

void ParseSyntax::parse()
{
	TokenVecPointer = &this->TokenVec;
//	printf("%d %d\n", this->TokenVec.size(), TokenVec.size());
	Handle_PROGRAM(false);
    FILE* writeTo;
    writeTo = fopen("output.txt", "w");
    for (int i = 0; i < output.size(); i++)
    {
        fprintf(writeTo, "%c", output[i]);
    }
}
