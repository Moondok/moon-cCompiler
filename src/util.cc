#include <util.h>
#include <lexer.h>

extern std::string Delimiter[DELIMITER_NUM];
extern std::string Operator[OPERATOR_NUM];


bool isLetter(char letter)
 {//注意C语言允许下划线也为标识符的一部分可以放在首部或其他地方
     return((letter >= 'a'&&letter <= 'z') || (letter >= 'A'&&letter <= 'Z')|| letter=='_')?true:false;
 }
 /*********************判断是否为字母********************/


 /*****************判断是否为数字************************/
 bool isDigit(char digit)
 {
     return (digit >= '0'&&digit <= '9')?true:false;
 }

 bool isOct(char digit)
 {
     return (digit >= '0' && digit <= '7');
 }

 bool isHex(char digit)
 {
     return (digit >= '0' && digit <= '9') || (digit >= 'a' && digit <= 'f') || (digit >= 'A' && digit <= 'F');
 }
/*****************判断是否为数字************************/


 bool isDelimiter(char ch)
 {
    for(int i=0;i<DELIMITER_NUM;i++){
        if(Delimiter[i][0]==ch){
            return true;
        }
    }
    return false;
 }

 bool isOperator(char ch)
 {
     for(int i=0;i<OPERATOR_NUM;i++){
        if(Operator[i][0]==ch){
            return true;
        }
    }
    return false;
 }


 
 

