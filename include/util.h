#ifndef UTIL_H
#define UTIL_H

#include <AST.h>
#include <fstream>
#include <memory>

/*********************判断是否为字母********************/
bool isLetter(char letter);

/*****************判断是否为数字************************/
bool isDigit(char digit);
bool isOct(char digit);
bool isHex(char digit);
bool isDelimiter(char ch);
bool isOperator(char ch);


void print_tree(const std::shared_ptr<AST> & root,std::ofstream & o,unsigned int layer);

#endif // UTIL_H