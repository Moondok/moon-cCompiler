#ifndef COMMON_H
#define COMMON_H

#include<iostream>
#include<map>
#include<string>
#include<algorithm>
#include<string.h>
#include <vector>
#include<fstream>
#include<sstream>
#include <set>
#include <regex>

using uint=unsigned int;

const int CANNOT_ENTER=-2;
const int NOT=-1;
const int SUCCESS=1;
//const int PreProcess_NOT=-50;
const int NOT_BOUND=1000;//原来不在表中

//const int RIGHT_STATUS=10000;

const int RESERVED_WORD_DFA=0;
const int IDENTIFIER_DFA=1;
const int OPERATOR_DFA=2;
const int DELIMITER_DFA=3;
const int DIGIT_DFA=4;
const int CONST_CHAR_DFA = 5;
const int LITERAL_STRING_DFA = 6;

const int RESERVED_WORD_NUM=35;
const int OPERATOR_NUM=39;
const int DELIMITER_NUM=7;

struct token
{
    unsigned int row;//起始行
    unsigned int column;//起始列
    std::string token_name;
    std::string type; //token_type是一个枚举类
};


#endif // COMMON_H