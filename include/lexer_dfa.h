#ifndef _LEXER_DFA_H_
#define _LEXER_DFA_H_
#include <common.h>
#include <string>
#include <util.h>
#include <tuple>
#include <algorithm>

using namespace std;

class AbstractDFA
{
public:
    explicit AbstractDFA();
    // virtual int isAccepted(ScanBuffer & ourScanBuffer)=0;
    virtual std::tuple<bool, std::string,std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) = 0;
};


class DelimitersAnalyzer : public AbstractDFA
{
private:

public:
    int searchDelimiter(std::string ch);
    // virtual int isAccepted (ScanBuffer & ourScanBuffer);
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;
};

class DigitAnalyzer : public AbstractDFA
{
private:
    //const int STARTSTATE=0;
    //const int EORPOINTSTATE=1;
    //const int ERRORSTATE=2;
    //const int RECSTATE=3;
    //const int NUMSTATE=4;
    const int STATE_START = 0;
    //const int STATE_1 = 1;
    //const int STATE_2 = 2;
    //const int STATE_3 = 3;
    //const int STATE_4 = 4;
    const int STATE_ERROR = 5;
    const int STATE_TERMINATE = 6;
    int state;
public:
    DigitAnalyzer();
    void clear();//»Øµ½³õÊ¼×´Ì¬
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;
};

class IdentifierAnalyzer : public AbstractDFA
{
private:
    uint cur_map_index = 0;//Token_MapÓÐ¼¸¸ö

public:
    void InsertToken(std::string ch);
    // virtual  int isAccepted (ScanBuffer & ourScanBuffer);
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;
};

class OperatorAnalyzer : public AbstractDFA
{
private:
    std::map<std::string, int> Operator_Map;
public:
    OperatorAnalyzer();
    //int Search_Operator(std::string ch);
    // virtual  int isAccepted (ScanBuffer & ourScanBuffer);
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;
};

class Preprocessor : public AbstractDFA
{
public:
    int preprocess(std::string& in_str, std::string& out_str, int& line, int& col);
    // virtual int isAccepted(ScanBuffer & ourScanBuffer);
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;

};

class ReservedWordAnalyzer : public AbstractDFA
{
private:
    std::map<std::string, int> ReservedWord_Map;

public:
    ReservedWordAnalyzer();
    // virtual int isAccepted (ScanBuffer & ourScanBuffer);//override virtual function
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;//override virtual function
};

class const_char_DFA : public AbstractDFA
{
private:
    std::regex const_char_dfa = std::regex("^'(\\\\.|[^\\\\'\n])+'");
    std::smatch tmp_match_result;
public:
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;
};

class literal_string_DFA :public AbstractDFA
{
private:
    std::regex literal_string_dfa = std::regex("^\"(\\\\.|[^\\\"\n])*\"");
    std::smatch tmp_match_result;
public:
    virtual std::tuple<bool, std::string, std::string> isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col) override;
};



#endif