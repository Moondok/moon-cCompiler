#include <string>
#include <lexer.h>

std::string Delimiter[DELIMITER_NUM]=
{
    "(",")",
    "{", "}",
    "[","]",
    ";"
};

string Operator[OPERATOR_NUM]=
{
    "...",">>=","<<=","+=","-=",
    "*=","/=","%=","&=","^=",
    "|=",">>","<<","++","--",
    "->","&&","||","<=",">=",
    "==","!=",",",":","=",
    ".","&","!","~","-",
    "+","*","/","%","<",
    ">","^","|","?",
};
std::vector<string> Operator_token = { 
    "ELLIPSIS", "RIGHT_ASSIGN","LEFT_ASSIGN","ADD_ASSIGN","SUB_ASSIGN",
    "MUL_ASSIGN","DIV_ASSIGN","MOD_ASSIGN","AND_ASSIGN","XOR_ASSIGN",
    "OR_ASSIGN","RIGHT_OP","LEFT_OP","INC_OP","DEC_OP",
    "PTR_OP","AND_OP","OR_OP","LE_OP","GE_OP",
    "EQ_OP","NE_OP",",",":","=",
    ".","&","!","~","-",
    "+","*","/","%","<",
    ">","^","|","?",
};

string ReserveWord[RESERVED_WORD_NUM]=
{
    "auto","break","case","char","const",
    "continue","default","do","double","else",
    "enum","extern","float","for","goto",
    "if","int","long","register","return",
    "short","signed","sizeof","static","struct",
    "switch","typedef","union","unsigned","void",
    "volatile","while","true","false","bool"
};

std::set<std::string> Identifier;//标识符
std::set<std::string> Digit;

// we need to cope with LITERAL_STRING , CONSTANT CHAR "\r", and some strange number like .3e3 
std::tuple<bool, std::string,int ,int > LexicalAnalyzer::analyze(std::string & in_str,std::string & out_str,int &pre_line,int &pre_col)
{
    //preprocess
    if(preprocessor.preprocess(in_str,out_str,pre_line,pre_col)!=1)
        return std::make_tuple(false,  "The comment is not closed.",pre_line,pre_col);

    
    //调各dfa识别出token
    uint start = 0, end = -1;
    uint line = 1, col = 0;
    stringstream out_stream;
    uint last_line = line, last_col = col;

    bool tag = true;
    std::tuple<bool, std::string, int, int> return_value;  //for report error, the first value refers to whether the lexical analysis valid
    std::tuple<bool, std::string, std::string> new_token;


    while(true)
    {
        //the start point is equals to "end+1"
        start=end+1;

        if(start>=out_str.size())
            break;
        
        //去掉所有当前空格和换行
        {
            uint pos=start;
            uint cur_col=col;
            uint cur_line=line;
            while(out_str[pos]==' '||out_str[pos]=='\n'||out_str[pos]=='\r')
            {
                if(out_str[pos]==' '||out_str[pos]=='\r')
                {
                    pos++;
                    cur_col++;
                }
                else
                {//'\n'
                    pos++;
                    cur_line++;
                    cur_col=0;
                }
                if(pos>=out_str.size())
                    break;
                
            }
            start=pos;
            end=pos;
            line=cur_line;
            col=cur_col;
        }
        if(start>=out_str.size())
            break;
        
       // int a = 03k;
        //we can identify the digit_constant , the limiter or the operator only with the first character given
        int enter_dfa=-1;
        char ch = out_str[start];
        if (ch == '\'')        // const char 
            enter_dfa = CONST_CHAR_DFA;
        else if (ch == '\"')   //literal string
            enter_dfa = LITERAL_STRING_DFA;
        else if (ch == '.')    //'.'  and  ellipse  and '.3'
        {
            if (start + 1 < out_str.size() && isDigit(out_str[start + 1]))
                enter_dfa = DIGIT_DFA;
            else
                enter_dfa = OPERATOR_DFA;
        }
        else if(isDelimiter(ch))
            enter_dfa=DELIMITER_DFA;
        else if(isDigit(ch))
            enter_dfa=DIGIT_DFA;
        else if(isOperator(ch))
            enter_dfa=OPERATOR_DFA;
        

        //调用各dfa
        if (enter_dfa == -1)
            for (int i = 0; i < 7; i++)
            {
                if (i == OPERATOR_DFA || i == DELIMITER_DFA || i == DIGIT_DFA || i == CONST_CHAR_DFA || i == LITERAL_STRING_DFA)
                    continue;
                new_token = DFAlist[i]->isAccepted(out_str, start, end, line, col);
                if (!std::get<0>(new_token))
                    continue;
                else
                {
                    enter_dfa = i;
                    break;
                }
            }
        else
        {
            new_token = DFAlist[enter_dfa]->isAccepted(out_str, start, end, line, col);
            if (!std::get<0>(new_token))
            {
                tag = false;
                return_value = std::make_tuple(false, std::get<2>(new_token), line, col + 1);
                break;

            }
        }
 
         // if the program can reach here , one token should be generated
        out_stream << line << " " << col << " " << std::get<2>(new_token) << " " << std::get<1>(new_token) << "\n";
        last_line = line;
        last_col = col;
        string out_path = "./token_result.tmp";
        ofstream outfile(out_path);
        if (!outfile.is_open())
        {
            cout << "can't open " << out_path << "\n";
            return std::make_tuple(false, "INNER ERROR::Can't create intermediate file\n", 0, 0);
        }
        outfile << out_stream.str();
        outfile << last_line << ' ' << 1 << ' ' << "#" << ' ' << "#" << "\n";
        outfile.close();

        
    }



    
    if (tag) {
        string out_path = "./token_result.tmp";
        ofstream outfile(out_path);
        if (!outfile.is_open()) 
        {
            cout << "can't open " << out_path << "\n";
            return std::make_tuple(false, "INNER ERROR::Can't create intermediate file\n", 0, 0);
        }
        outfile << out_stream.str();
        outfile << last_line+1 << ' ' << 1 << ' ' << "#" << ' ' << "#" << "\n";
        outfile.close();
        return std::make_tuple(true, "LEXICAL ANALYSIS SUCCESS.\n", 0, 0);
    }
    else
    {
        return return_value;
        //string value = out_str.substr(start, end - start + 1);
       
       // outfile << last_line << ' ' << last_col+1 << ' ' << type << ' ' << value << "\n";
        
    }

    
}


LexicalAnalyzer::LexicalAnalyzer()
{
   DFAlist[RESERVED_WORD_DFA]=new ReservedWordAnalyzer;
   DFAlist[IDENTIFIER_DFA]=new IdentifierAnalyzer;
   DFAlist[OPERATOR_DFA]=new OperatorAnalyzer;
   DFAlist[DELIMITER_DFA]=new DelimitersAnalyzer;
   DFAlist[DIGIT_DFA]=new DigitAnalyzer;
   DFAlist[CONST_CHAR_DFA] = new const_char_DFA;
   DFAlist[LITERAL_STRING_DFA] = new literal_string_DFA;


}

LexicalAnalyzer::~LexicalAnalyzer()
{
    for(unsigned int i=0;i<7;i++)
    {
        if(DFAlist[i]!=nullptr)
        {
            delete DFAlist[i];
            DFAlist[i]=nullptr;
        }
    }
}