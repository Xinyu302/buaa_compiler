#include "ParseSyntax.h"
#include <iostream>
#include <vector>
#include <set>
#define Tokens (*TokenVecPointer)
// 运用递归下降的方法分析语法
ParseSyntax::ParseSyntax(std::vector<Token>& TokenVec):TokenVec(TokenVec){}
int nowLoc = 0;
int end;
std::set<std::string> voidFunc, nonVoidFunc;
std::vector<Token>* TokenVecPointer;
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
	bool b;
	if (b = typeAssert(index))
	{
		output += Tokens[nowLoc].TokenPrintStr();
		//std::cout << Tokens[nowLoc].TokenPrintStr();
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
bool Handle_DECLARE_HEADER(bool show)
{
	if (typeEnsure(Token::INTTK) || typeEnsure(Token::CHARTK))
	{
		if (typeEnsure(Token::IDENFR))
		{
			output += "<声明头部>";
			output += '\n';
			return true;
		}
		
	}
	return false;
}
bool Handle_CONST_INT_OR_CHAR(bool show)
{
	if (Handle_INTCON(show) || Handle_CHARCON(show))
	{
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

bool Handle_LBRACK_UINT_RBRACK(bool show)
{
	if (typeEnsure(Token::LBRACK))
	{
		if (Handle_UNSIGNED_INTCON(show))
		{
			return typeEnsure(Token::RBRACK);
		}
		return false;
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
	if (!typeAssert(loc++, Token::IDENFR)) return false;
	if (typeAssert(loc, Token::ASSIGN)) return true;
	if (!typeAssert(loc++, Token::LBRACK)) return false;
	if (!typeAssert(loc++, Token::INTCON)) return false;
	if (!typeAssert(loc++, Token::RBRACK)) return false;
	if (typeAssert(loc, Token::ASSIGN)) return true;
	if (!typeAssert(loc++, Token::LBRACK)) return false;
	if (!typeAssert(loc++, Token::INTCON)) return false;
	if (!typeAssert(loc++, Token::RBRACK)) return false;
	if (typeAssert(loc, Token::ASSIGN)) return true;
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
	while (Handle_IDENFR(show))
	{
		if (typeEnsure(Token::LBRACK))
		{
			if (!Handle_UNSIGNED_INTCON(show) || !typeEnsure(Token::RBRACK)) return false;
			if (typeEnsure(Token::LBRACK))
			{
				if (!Handle_UNSIGNED_INTCON(show) || !typeEnsure(Token::RBRACK)) return false;
			}
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
	if (typeEnsure(Token::LBRACK))
	{
		if (!Handle_UNSIGNED_INTCON(show) || !typeEnsure(Token::RBRACK)) return false;
		if (typeEnsure(Token::LBRACK))
		{
			if (!Handle_UNSIGNED_INTCON(show) || !typeEnsure(Token::RBRACK)) return false;
			if (typeEnsure(Token::ASSIGN))
			{
				typeEnsure(Token::LBRACE);
				while (typeEnsure(Token::LBRACE))
				{
					while (Handle_CONST_INT_OR_CHAR(show))
					{
						if (!typeEnsure(Token::COMMA)) break;
					}
					typeEnsure(Token::RBRACE);
					if (!typeEnsure(Token::COMMA)) break;
				}
				flag = typeEnsure(Token::RBRACE);
			}
		}
		else if (typeEnsure(Token::ASSIGN))
		{
			typeEnsure(Token::LBRACE);
			while (Handle_CONST_INT_OR_CHAR(show))
			{
				if (!typeEnsure(Token::COMMA)) break;
			}
			flag = typeEnsure(Token::RBRACE);
		}
	}
	else if (typeEnsure(Token::ASSIGN))
	{
		flag = Handle_CONST_INT_OR_CHAR(show);
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
	if (!Handle_TYPE_IDENFR(show) && !Handle_IDENFR(show))
	{
		return false;
	}
	Handle_LBRACK_UINT_RBRACK(show);
	while (true)
	{
		if (!typeEnsure(Token::COMMA)) break;
		if (!Handle_IDENFR(show)) break;
		Handle_LBRACK_UINT_RBRACK(show);
	}
	return true;

}


//<变量说明> ::= <变量定义>;{<变量定义>;}
bool Handle_VAR_EXPLAIN(bool show)
{
	bool flag = false;
	while (Handle_VAR_DEFINE(show) && typeEnsure(Token::SEMICN))
	{
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
bool Handle_PARA_LIST(bool show)
{
	while (Handle_TYPE_IDENFR(show) && Handle_IDENFR(show))
	{
		typeEnsure(Token::COMMA);
	}
	output += "<参数表>";
	output += '\n';
	return true;
}
//<项>    ::= <因子>{<乘法运算符><因子>}
bool Handle_TERM(bool show)
{
	if (!Handle_FACTOR(show)) return false;
	while (Handle_MULT(show) && Handle_FACTOR(show));
	output += "<项>";
	output += '\n';
	return true;
}
//<表达式>::=［＋｜－］<项>{<加法运算符><项>}//[+|-]只作用于第一个<项>
bool Handle_EXPRESSION(bool show)
{
	Handle_PLUS(show);
	if (!Handle_TERM(show)) return false;
	while (Handle_PLUS(show) && Handle_TERM(show));
	output += "<表达式>";
	output += '\n';
	return true;
}

bool Handle_LBRACK_EXP_RBRACK(bool show,bool twice)
{
	if (!typeEnsure(Token::LBRACK)) return false;
	if (!Handle_EXPRESSION(show)) return false;
	if (!typeEnsure(Token::RBRACK)) return false;
	if (!typeEnsure(Token::LBRACK)) return false;
	if (!Handle_EXPRESSION(show)) return false;
	if (!typeEnsure(Token::RBRACK)) return false;
	return true;
}
//'['<表达式>']'
bool Handle_LBRACK_EXP_RBRACK(bool show)
{
	if (!typeEnsure(Token::LBRACK)) return false;
	if (!Handle_EXPRESSION(show)) return false;
	return typeEnsure(Token::RBRACK);
}
//'('<表达式>')'
bool Handle_LPARENT_EXP_RPARENT(bool show)
{
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_EXPRESSION(show)) return false;
	return typeEnsure(Token::RPARENT);
}
//<字符>::=  '<加法运算符>'｜'<乘法运算符>'｜'<字母>'｜'<数字>'
bool Handle_CHAR(bool show)
{
	return typeEnsure(Token::CHARCON);
}
//<因子>   ::= <标识符>｜<标识符>'['<表达式>']'|'('<表达式>')'｜<整数>|<字符>｜<有返回值函数调用语句> 
bool Handle_FACTOR(bool show)
{
	bool flag = false;
	if (typeAssert(nowLoc,Token::IDENFR))
	{	
		flag = true;
		if (typeAssert(nowLoc+1, Token::LBRACK))
		{
			Handle_IDENFR(show);
			if (typeAssert(nowLoc + 3, Token::LBRACK))
			Handle_LBRACK_EXP_RBRACK(show,true);
			else 
			{
				Handle_LBRACK_EXP_RBRACK(show);
			}
		} 
		else if (typeAssert(nowLoc+1, Token::LPARENT))
		{
			Handle_RETURN_FUNC_CALL(show);
		}
		else
		{
			Handle_IDENFR(show);
		}
	}
	else if (Handle_LPARENT_EXP_RPARENT(show))
	{
		flag = true;
	}
	else if (Handle_INTCON(show))
	{
		flag = true;
	}
	else if (Handle_CHARCON(show))
	{
		flag = true;
	}
	if (flag)
	{
		output += "<因子>";
		output += '\n';
	}
	return flag;

}
//<值参数表>  ::= <表达式>{,<表达式>}｜<空>
bool Handle_VAL_PARA_LIST(bool show)
{
	Handle_EXPRESSION(show);
	while (typeEnsure(Token::COMMA) && Handle_EXPRESSION(show));
	output += "<值参数表>";
	output += '\n';
	return true;
}
//<有返回值函数调用语句>::= <标识符>'('<值参数表>')'
bool Handle_RETURN_FUNC_CALL(bool show)
{
	if (!Handle_IDENFR(show)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_VAL_PARA_LIST(show)) return false;
	if (typeEnsure(Token::RPARENT))
	{
		output += "<有返回值函数调用语句>";
		output += '\n';
		return true;
	}
	return false;
}
////<无返回值函数调用语句>:: = <标识符>'('<值参数表>')'
bool Handle_VOID_FUNC_CALL(bool show)
{
	if (!Handle_IDENFR(show)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_VAL_PARA_LIST(show)) return false;
	if (typeEnsure(Token::RPARENT))
	{
		output += "<无返回值函数调用语句>";
		output += '\n';
		return true;
	}
	return false;
}

// <常量说明>::=  const<常量定义>;{ const<常量定义>;}
bool Handle_CONST_EXPLAIN(bool show)
{
	if (!typeEnsure(Token::CONSTTK)) return false;
	if (!Handle_CONST_DEFINE(show)) return false;
	if (!typeEnsure(Token::SEMICN)) return false;
	while (typeEnsure(Token::CONSTTK))
	{
		Handle_CONST_DEFINE(show);
		typeEnsure(Token::SEMICN);
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
		if (!Handle_IDENFR(show)) return false;
		if (!typeEnsure(Token::ASSIGN)) return false;
		if (!Handle_INTCON(show)&&!Handle_CHARCON(show)) return false;
		while (typeEnsure(Token::COMMA))
		{
			if (!Handle_IDENFR(show)) return false;
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
	if (!Handle_DECLARE_HEADER(show)) return false;
	const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_PARA_LIST(show)) return false;
	if (!typeEnsure(Token::RPARENT)) return false;
	if (!typeEnsure(Token::LBRACE)) return false;
	if (!Handle_COMPLEX_STATE(show)) return false;
	if (!typeEnsure(Token::RBRACE)) return false;
	nonVoidFunc.insert(idName);
	output += "<有返回值函数定义>";
	output += '\n';
	return true;
}

//<无返回值函数定义> :: = void<标识符>'('<参数表>')''{'<复合语句>'}'
bool Handle_VOID_FUNC_DEFINE(bool show)
{
	if (!typeAssert(nowLoc, Token::VOIDTK)) return false;
	if (!typeAssert(nowLoc+1, Token::IDENFR)) return false;
	if (!typeEnsure(Token::VOIDTK)) return false;
	if (!typeEnsure(Token::IDENFR)) return false;
	const std::string& idName = Tokens[nowLoc - 1].getTokenStr();
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_PARA_LIST(show)) return false;
	if (!typeEnsure(Token::RPARENT)) return false;
	if (!typeEnsure(Token::LBRACE)) return false;
	if (!Handle_COMPLEX_STATE(show)) return false;
	if (!typeEnsure(Token::RBRACE)) return false;
	voidFunc.insert(idName);
	output += "<无返回值函数定义>";
	output += '\n';
	return true;
}
//<返回语句>  ::=  return['('<表达式>')']   
bool Handle_RETURN_STATE(bool show)
{
	if (!typeEnsure(Token::RETURNTK)) return false;
	Handle_LPARENT_EXP_RPARENT(show);
	output += "<返回语句>";
	output += '\n';
	return true;
}

//<写语句>   ::= printf '(' <字符串>,<表达式>')'| printf '('<字符串>')'| printf '('<表达式>')'
bool Handle_PRINTF_STATE(bool show)
{
	if (!typeEnsure(Token::PRINTFTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (Handle_STRCON(show))
	{
		if (typeEnsure(Token::COMMA))
		{
			if (!Handle_EXPRESSION(show)) return false;
		}
	}
	else
	{
		if (!Handle_EXPRESSION(show)) return false;
	}
	
	if (typeEnsure(Token::RPARENT))
	{
		output += "<写语句>";
		output += '\n';
		return true;
	}
	return false;
}
//<读语句>   ::=  scanf '('<标识符>{,<标识符>}')'
bool Handle_SCANF_STATE(bool show)
{
	if (!typeEnsure(Token::SCANFTK)) return false;
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_IDENFR(show)) return false;
	while (typeEnsure(Token::COMMA))
	{
		if (!Handle_IDENFR(show)) return false;
	}
	if (typeEnsure(Token::RPARENT))
	{
		output += "<读语句>";
		output += '\n';
		return true;
	}
	return false;
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
/*<条件>   ::=  <表达式><关系运算符><表达式>//整型表达式之间才能进行关系运算

	   ｜<表达式>   //表达式为整型，其值为0条件为假，值不为0时条件为真*/
bool Handle_CONDITION(bool show)
{
	if (Handle_EXPRESSION(show))
	{
		Handle_CMP(show);
		Handle_EXPRESSION(show);
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
	if (!typeEnsure(Token::SWITCHTK)) return false;
	if (!Handle_LPARENT_EXP_RPARENT(show)) return false;
	if (!typeEnsure(Token::LBRACE)) return false;
	if (!Handle_CONDITON_TABLE(show)) return false;
	if (!Handle_DEFAULT(show)) return false;
	if (typeEnsure(Token::RBRACE))
	{
		output += "<情况语句>";
		output += '\n';
		return true;
	}
	return false;
}
//＜情况表＞   ::=  ＜情况子语句＞{＜情况子语句＞}  
bool Handle_CONDITON_TABLE(bool show)
{
	if (!Handle_CONDITION_CHILD_STATE(show)) return false;
	while (Handle_CONDITION_CHILD_STATE(show));
	output += "<情况表>";
	output += '\n';
	return true;
}
//＜情况子语句＞  ::=  case＜常量＞：＜语句＞   
bool Handle_CONDITION_CHILD_STATE(bool show)
{
	if (!typeEnsure(Token::CASETK)) return false;
	if (!Handle_CONST_INT_OR_CHAR(show)) return false;
	if (!typeEnsure(Token::COLON)) return false;
	if (Handle_STATEMENT(show))
	{
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
	//std::cout << "here" << std::endl;
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
	if (!typeEnsure(Token::RPARENT)) return false;
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
	if (Handle_IDENFR(show))
	{
		if (typeEnsure(Token::ASSIGN))
		{
			flag = Handle_EXPRESSION(show);
		}
		else if (typeEnsure(Token::LBRACK))
		{
			if (!Handle_EXPRESSION(show)) return false;
			if (!typeEnsure(Token::RBRACK)) return false;
			if (typeEnsure(Token::ASSIGN))
			{
				flag = Handle_EXPRESSION(show);
			}
			else if (typeEnsure(Token::LBRACK))
			{
				if (!Handle_EXPRESSION(show)) return false;
				if (!typeEnsure(Token::RBRACK)) return false;
				if (typeEnsure(Token::ASSIGN))
				{
					flag = Handle_EXPRESSION(show);
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
	if (!typeEnsure(Token::LPARENT)) return false;
	if (!Handle_IDENFR(show)) return false;
	if (!typeEnsure(Token::ASSIGN)) return false;
	if (!Handle_EXPRESSION(show)) return false;
	if (!typeEnsure(Token::SEMICN)) return false;
	if (!Handle_CONDITION(show)) return false;
	if (!typeEnsure(Token::SEMICN)) return false;
	if (!Handle_IDENFR(show)) return false;
	if (!typeEnsure(Token::ASSIGN)) return false;
	if (!Handle_IDENFR(show)) return false;
	if (!Handle_PLUS(show)) return false;
	if (!Handle_STEP(show)) return false;
	
	if (typeEnsure(Token::RPARENT))
	{
		return true;
	}
	return false;
}

bool Handle_LOOP_STATE(bool show)
{
	bool flag = false;
	if (typeEnsure(Token::WHILETK))
	{
		if (!typeEnsure(Token::LPARENT)) return false;
		if (!Handle_CONDITION(show)) return false;
		if (!typeEnsure(Token::RPARENT)) return false;
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
	}
	else if (typeEnsure(Token::SEMICN)) flag = true;
	else if (typeAssert(Token::IDENFR))
	{
		flag = true;
		const std::string& idName = Tokens[nowLoc].getTokenStr();
		if (nonVoidFunc.count(idName) != 0)
		{
			flag = Handle_RETURN_FUNC_CALL(show);
		}
		else if (voidFunc.count(idName) != 0)
		{
			flag = Handle_VOID_FUNC_CALL(show);
		}
		else flag = Handle_ASSIGN_STATE(show);
		if (typeEnsure(Token::SEMICN)) flag = true;
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
	Handle_CONST_EXPLAIN(show);
	Handle_VAR_EXPLAIN(show);
	while (Handle_RETURN_FUNC_DEFINE(show) || Handle_VOID_FUNC_DEFINE(show));
	if (!HandleMAIN(show)) return false;
	output += "<程序>";
	output += '\n';
	return true;
}
/*处理主函数main*/
bool HandleMAIN(bool show)
{
	if (!typeEnsure(Token::VOIDTK)) return false;
	if (!typeEnsure(Token::MAINTK)) return false;
	if (!typeEnsure(Token::LPARENT) || !typeEnsure(Token::RPARENT) || !typeEnsure(Token::LBRACE)) return false;
	if (!Handle_COMPLEX_STATE(show)) return false;
	if (typeEnsure(Token::RBRACE))
	{
		output += "<主函数>";
		output += '\n';
		return true;
	}
	return false;
}

void ParseSyntax::parse()
{
	end = this->TokenVec.size();
	TokenVecPointer = &this->TokenVec;
	//printf("%d %d\n", this->TokenVec.size(), TokenVec.size());
	Handle_PROGRAM(false);
	FILE* writeTo;
	writeTo = fopen("output.txt", "w");
	for (int i = 0; i < output.size(); i++)
	{
		fprintf(writeTo, "%c", output[i]);
	}
}

std::vector<Token>& ParseSyntax::getTokens()
{
	return this->TokenVec;
}
