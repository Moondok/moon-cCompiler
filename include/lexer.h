#ifndef __LEXER__H__
#define __LEXER__H__

#include <string>
#include <common.h>
#include <vector>
#include <lexer_dfa.h>
#include <regex>

class LexicalAnalyzer
{
private:


    Preprocessor preprocessor;
    AbstractDFA* DFAlist[11];
    static const int MAX_DIGIT_NUM=500;
    static const int MAX_IDENTIFIER_NUM=500;

    

public:
    LexicalAnalyzer();
    ~LexicalAnalyzer();
    std::tuple<bool,std::string,int ,int> analyze(std::string &in_str,std::string &out_str, int& pre_line, int& pre_col);

    
    token digit_table[MAX_DIGIT_NUM];
    token identifier_table[MAX_IDENTIFIER_NUM];
    unsigned int line=0; //行数
    unsigned int start=0,end=0;//出错起始位置
};




#endif // LEXICALANALYZER_H