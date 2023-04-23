#include <lexer_dfa.h>

AbstractDFA::AbstractDFA()
{

}

extern std::string Delimiter[DELIMITER_NUM];
int DelimitersAnalyzer::searchDelimiter(std::string ch)//判断是否在界符表里
{
    for (int i = 0; i < DELIMITER_NUM; i++) {
        if (ch.find(Delimiter[i]) != -1)
            return i;
    }
    return NOT;
}

std::tuple<bool, std::string, std::string> DelimitersAnalyzer::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    uint pos = start;
    if (searchDelimiter(&str[pos]) == NOT) 
        return std::make_tuple(false,"None","LEXICAL ERROR::delimiter required\n");
    else 
    {
        start = end = pos;
        col++;
        return std::make_tuple(true,str.substr(start,end-start+1), str.substr(start, end - start + 1));
    }
}



extern std::set<std::string> Digit;
DigitAnalyzer::DigitAnalyzer()
{
    state = this->STATE_START;
}
double a = 1.f;
std::tuple<bool, std::string, std::string> DigitAnalyzer::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    unsigned int pos = start;
    this->state = STATE_START;
    std::string error_message = "LEXICAL ERROR::Unrecognizable numeric constants\n ";
    std::string type="NON_SENSE";// here we distinguish double or int
    if (str.at(pos) == '.')// { D }*"."{D} + {E} ? {FS} ?
    {
        type="CONSTANT_DOUBLE";
        while (++pos<str.size())
        {
            if (isDigit(str.at(pos)))
                this->state = STATE_TERMINATE;
            else
                break; 
        }
        if (this->state == STATE_TERMINATE)// { D }+
        {
            //{E}?
            if (pos<str.size()&&(str.at(pos) == 'e' || str.at(pos) == 'E'))
            {
                this->state = STATE_ERROR;
                //
                if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                    ;
                else
                    pos--;
                while (++pos < str.size() )
                {
                    if(isDigit(str.at(pos)))
                        this->state = STATE_TERMINATE;
                    else
                        break;
                }
                //{FS}?
                if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                    pos++;
            }

            else if (pos < str.size() && (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L'))
                pos++;
             //search some incompatibla character existed or no, such as [a-z][0-9] and[.]
            if (pos < str.size())
            {
                if (isDigit(str.at(pos)) || isLetter(str.at(pos)) || str.at(pos) == '.')
                {
                    this->state = STATE_ERROR;
                    error_message = "LEXICAL ERROR::Text beyond expectations after decimals\n";
                }

            }
        }
    }


    else if(str.at(pos)!='0')//[1-9]{D}*{IS}?  {D}+{E}{FS}?  {D}+"."{D}*{E}?{FS}?  IS:((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))
    {
        type="CONSTANT_INT";
        this->state = STATE_TERMINATE;
        while (++pos < str.size() && isDigit(str.at(pos)))
            ;
        if (pos < str.size())
        {
            if (str.at(pos) == 'u' || str.at(pos) == 'U')//[1 - 9]{ D }* {IS} ?
            {
                type="CONSTANT_INT";
                pos++;
                if (pos < str.size()&& (str.at(pos) == 'l' || str.at(pos) == 'L'))
                {
                    pos++;
                    if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                        pos++;
                }
            }

            else if (str.at(pos) == 'l' || str.at(pos) == 'L') //[1-9]{D}*{IS}?
            {
                type="CONSTANT_INT";
                pos++;
                if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                    pos++;
                if (pos < str.size() && (str.at(pos) == 'u' || str.at(pos) == 'U'))
                    pos++;
            }

            else if (str.at(pos) == 'E' || str.at(pos) == 'e')// { D } + {E} {FS} ?
            {
                type="CONSTANT_DOUBLE";
                this->state = STATE_ERROR;
                //
                if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                    ;
                else
                    pos--;
                while (++pos < str.size())
                {
                    if (isDigit(str.at(pos)))
                        this->state = STATE_TERMINATE;
                    else
                        break;
                }
                //{FS}?
                if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                    pos++;
            }

            else if (str.at(pos) == '.')// {D}+"."{D}*{E}?{FS}?
            {
                type="CONSTANT_DOUBLE";
                this->state = STATE_TERMINATE;
                while (++pos < str.size() && isDigit(str.at(pos)))
                    ;

                if (pos < str.size() && (str.at(pos) == 'e' || str.at(pos) == 'E'))// { E } ?
                {
                    this->state = STATE_ERROR;
                    //
                    if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                        ;
                    else
                        pos--;
                    while (++pos < str.size())
                    {
                        if (isDigit(str.at(pos)))
                            this->state = STATE_TERMINATE;
                        else
                            break;
                    }
                    //{FS}?
                    if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                        pos++;
                }
                //{FS}?
                else if (pos < str.size() && (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L'))
                    pos++;
            }

        }

        //search some incompatibla character existed or no, such as [a-z][0-9] and[.]
        if (pos < str.size())
        {
            if (isDigit(str.at(pos)) || isLetter(str.at(pos)) || str.at(pos) == '.')
            {
                this->state = STATE_ERROR;
                error_message = "LEXICAL ERROR::Text beyond expectations after decimals\n";
            }

        }

    }

    
    else if (str.at(pos) == '0')
    {
        pos++;
        if (pos < str.size() && (str.at(pos) == 'x' || str.at(pos) == 'X')) //0[xX]{H}+{IS}?     0[xX]{H}+{P}{FS}?     0[xX] {H}*"."{H} + {P} {FS} ?     0[xX] {H} + "."{H}* {P} {FS} ? 
        {
            state = STATE_ERROR;
            if (++pos < str.size() && str.at(pos) == '.') // 0[xX] {H}*"."{H} + {P} {FS} ?
            {
                type="CONSTANT_DOUBLE";
                bool tag = false;
                while(++pos < str.size() && isHex(str.at(pos)))//{H}+
                    tag=true;
                if (tag && pos < str.size() && (str.at(pos) == 'p' || str.at(pos) == 'P'))
                {
                    this->state = STATE_ERROR;
                    //
                    if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                        ;
                    else
                        pos--;
                    while (++pos < str.size())
                    {
                        if (isDigit(str.at(pos)))
                            this->state = STATE_TERMINATE;
                        else
                            break;
                    }
                    //{FS}?
                    if (this->state == STATE_TERMINATE)
                        if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                            pos++;
                }

            }

            else if (pos < str.size() && isHex(str.at(pos))) //0[xX]{H}+{IS}?    0[xX]{H}+{P}{FS}?    0[xX] {H} + "."{H}* {P} {FS} ? 
            {
                type="CONSTANT_INT";
                this->state = STATE_TERMINATE;
                while (++pos < str.size() && isHex(str.at(pos)))//{H}+
                    ;


                if (str.at(pos) == '.')// 0[xX] {H} + "."{H}* {P} {FS} ? 
                {
                    type="CONSTANT_DOUBLE";
                    this->state = STATE_ERROR;
                    while (++pos < str.size() && isHex(str.at(pos)))
                        ;
                    if (pos < str.size() && (str.at(pos) == 'p' || str.at(pos) == 'P'))
                    {
                        this->state = STATE_ERROR;
                        //
                        if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                            ;
                        else
                            pos--;
                        while (++pos < str.size())
                        {
                            if (isDigit(str.at(pos)))
                                this->state = STATE_TERMINATE;
                            else
                                break;
                        }
                        //{FS}?
                        if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                            pos++;
                    }

                }
                else if (pos<str.size()&&(str.at(pos) == 'p' || str.at(pos) == 'P') )//  0[xX]{H}+{P}{FS}?
                {
                    type="CONSTANT_DOUBLE";
                    this->state = STATE_ERROR;
                    //
                    if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                        ;
                    else
                        pos--;
                    while (++pos < str.size())
                    {
                        if (isDigit(str.at(pos)))
                            this->state = STATE_TERMINATE;
                        else
                            break;
                    }
                    //{FS}?
                    if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                        pos++;
                }

                //0[xX]{H}+{IS}?
                else if (pos < str.size() && (str.at(pos) == 'u' || str.at(pos) == 'U'))
                {
                    type="CONSTANT_INT";
                    pos++;
                    if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                    {
                        pos++;
                        if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                            pos++;
                    }
                }
                else if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                {
                    type="CONSTANT_INT";
                    pos++;
                    if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                        pos++;
                    if (pos < str.size() && (str.at(pos) == 'u' || str.at(pos) == 'U'))
                        pos++;
                }

            }

            double x = 02234528.l;
        }

        else  //0[0-7]*{IS}?  {D}+{E}{FS}?   {D}+"."{D}*{E}?{FS}?
        {
            pos--;
            state = STATE_TERMINATE;
            while (++pos < str.size() && isOct(str.at(pos)))
                ;

            

            if (pos < str.size() && (isDigit(str.at(pos))||str.at(pos)=='e'|| str.at(pos) == 'E'|| str.at(pos) == '.'))  //   {D}+{E}{FS}?   {D}+"."{D}*{E}?{FS}?
            {
                type="CONSTANT_DOUBLE";
                state = STATE_ERROR;
                error_message = "LEXICAL ERROR::Unrecognized octal constants\n";
                if(isDigit(str.at(pos)))
                    while (++pos < str.size() && isDigit(str.at(pos)))
                        ;
                if (pos < str.size() && (str.at(pos) == 'e' || str.at(pos) == 'E'))//{D}+{E}{FS}?
                {
                    error_message= "LEXICAL ERROR::Unrecognizable numeric constants\n ";
                    if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                        ;
                    else
                        pos--;
                    while (++pos < str.size())
                    {
                        if (isDigit(str.at(pos)))
                            this->state = STATE_TERMINATE;
                        else
                            break;
                    }
                    //{FS}?
                    if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                        pos++;

                }
                

                else if (pos < str.size() && str.at(pos) == '.')//{D}+"."{D}*{E}?{FS}?
                {
                    type="CONSTANT_DOUBLE";
                    state = STATE_TERMINATE;
                    
                    while (++pos < str.size() && isDigit(str.at(pos)))
                        ;

                    if (pos < str.size() && (str.at(pos) == 'e' || str.at(pos) == 'E'))
                    {
                        state = STATE_TERMINATE;
                        error_message = "LEXICAL ERROR::Unrecognizable numeric constants\n ";
                        if (++pos < str.size() && (str.at(pos) == '+' || str.at(pos) == '-'))
                            ;
                        else
                            pos--;
                        while (++pos < str.size())
                        {
                            if (isDigit(str.at(pos)))
                                this->state = STATE_TERMINATE;
                            else
                                break;
                        }
                        //{FS}?
                        if (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L')
                            pos++;
                    }
                    //{FS}?
                    else if (pos < str.size() && (str.at(pos) == 'f' || str.at(pos) == 'F' || str.at(pos) == 'l' || str.at(pos) == 'L'))
                        pos++;
                    

                }

            }
            else if (pos < str.size()) //0[0-7]*{IS}?
            {
                type="CONSTANT_INT";
                if (str.at(pos) == 'u' || str.at(pos) == 'U') //{IS}?
                {
                    pos++;
                    if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                    {
                        pos++;
                        if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                            pos++;
                    }
                }

                else if (str.at(pos) == 'l' || str.at(pos) == 'L') //{IS}?
                {
                    pos++;
                    if (pos < str.size() && (str.at(pos) == 'l' || str.at(pos) == 'L'))
                        pos++;
                    if (pos < str.size() && (str.at(pos) == 'u' || str.at(pos) == 'U'))
                        pos++;
                }
            }
            
        }
        if (pos < str.size()&&this->state==STATE_TERMINATE)
        {
            if (isDigit(str.at(pos)) || isLetter(str.at(pos)) || str.at(pos) == '.')
            {
                this->state = STATE_ERROR;
                error_message = "LEXICAL ERROR::Text beyond expectations after decimals\n";
            }

        }
    }

    //after traversing 9 automatas above
    if (this->state == STATE_TERMINATE)
    {
        end = pos - 1;
        col = col + end - start + 1;
        return std::make_tuple(true, str.substr(start, end - start + 1), type);
    }
    else
        return std::make_tuple(false, "None", error_message);

}
/*
{
    if (state != STATE_START)
        return std::make_tuple(false," "," ");
    uint i = start;
    for (;; i++) {
        if (i > str.size()) {
            return std::make_tuple(false, " ", " ");
        }
        if (state == STATE_START) {
            if (str[i] <= '9' && str[i] >= '0') {
            }
            else if (str[i] == '.') {
                state = STATE_1;
            }
            else if (str[i] == 'e') {
                state = STATE_3;
            }
            else {
                state = STATE_TERMINATE;
                break;
            }
        }
        else if (state == STATE_1) {
            if (str[i] <= '9' && str[i] >= '0') {
                state = STATE_2;
            }
            else if (str[i] == 'e') {
                state = STATE_3;
            }
            else if (str[i] == '.') {
                state = STATE_ERROR;
                break;
            }
            else {
                state = STATE_TERMINATE;
                break;
            }
        }
        else if (state == STATE_2) {
            if (str[i] <= '9' && str[i] >= '0') {
            }
            else if (str[i] == 'e') {
                state = STATE_3;
            }
            else if (str[i] == '.') {
                state = STATE_ERROR;
                break;
            }
            else {
                state = STATE_TERMINATE;
                break;
            }
        }
        else if (state == STATE_3) {
            if (str[i] <= '9' && str[i] >= '0') {
                state = STATE_4;
            }
            else if (str[i] == '.' || str[i] == 'e') {
                state = STATE_ERROR;
                break;
            }
            else {
                state = STATE_TERMINATE;
                break;
            }
        }
        else {//STATE_4
            if (str[i] <= '9' && str[i] >= '0') {
            }
            else if (str[i] == '.' || str[i] == 'e') {
                state = STATE_ERROR;
                break;
            }
            else {
                state = STATE_TERMINATE;
                break;
            }
        }
    }
    if (state == STATE_TERMINATE) {//其他接受状态后会一直接受符号直到遇到不符符号直到TERMINATE
        end = i - 1;//回退
        state = STATE_START;
        col = col + (i - start);
        //更新Digit
        string temp = str.substr(start, end - start + 1);
        Digit.insert(temp);
        return std::make_tuple(true, temp, "CONSTANT");
    }
    else {
        state = STATE_START;
        return std::make_tuple(false, " ", " ");;
    }
}
*/
void DigitAnalyzer::clear()
{
    state = STATE_START;
}



extern std::string Operator[OPERATOR_NUM];
extern std::vector<string> Operator_token;
OperatorAnalyzer::OperatorAnalyzer()
{
    for (unsigned int i = 0; i < OPERATOR_NUM; i++)
        Operator_Map[Operator[i]] = i;
}

std::tuple<bool, std::string, std::string> OperatorAnalyzer::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    uint pos = start;
    uint cur_col = col;

    std::string token_content, token_type;
    char ch = str[pos];//进来一定是Operator
    
    {
        if (ch == '.')
        {
            end = pos;
            if (pos + 2 < str.size() && str[pos + 1] == '.' && str[pos + 2] == '.')
            {
                end = pos + 2;
                token_content = "...";
            }
            else
                token_content = "."; 
        }
        else if (ch == ',' || ch == '?' || ch == ':' || ch == '~')
        {                               //for '.' , we will judge at another method
            end = pos;  
            token_content = token_type = ch;
        }//4
        
        else if (ch == '+') 
        {
            char ch1 = str[pos + 1];
            if (ch1 == '+') 
            {
                end = pos + 1;
                token_content = "++";
                token_type = "INC_OP";
            }
            else if (ch1 == '=') 
            {
                end = pos + 1;
                token_content = "+=";
                token_type = "ADD_ASSIGN";
            }
            else 
            {
                end = pos;
                token_content = token_type= "+";
            }
        }  //perfix= '+'  ++,+=,+, 3

        else if (ch == '-') 
        {
            char ch1 = str[pos + 1];
            end = pos + 1;
            if (ch1 == '-') 
            {
                token_content = "--";
                token_type = "DEC_OP";
            }
            else if (ch1 == '=') 
            {
                token_content = "-=";
                token_type = "SUB_ASSIGN";
                
            }
            else if (ch1 == '>') 
            {
                token_content = "->";
                token_type = "PTR_OP";
            }
            else 
            {
                end = pos;
                token_content = token_type = "-";
            }
        } //perfix=   --,-=,-,-> 4


        else if (ch == '*' || ch == '/' || ch == '%' || ch == '=' || ch == '!' || ch == '^') 
        {
            token_content = token_type=ch;
            char ch1 = str[pos + 1];
            end = pos + 1;
            if (ch1 == '=') 
            {
                token_content += ch1;
                token_type = Operator_token.at(Operator_Map.find(token_content)->second);
            }
            else 
                end = pos;
        } // * ,/, \% ,=,!,^,  12


        else if (ch == '>'||ch=='<') 
        {
            token_content = token_type = ch;
            char ch1 = str[pos + 1];
            end = pos + 1;
            if (ch1 == ch) 
            {
                token_content+=ch1;
                
                char ch2 = str[pos + 2];
                if (ch2 == '=') 
                {
                    token_content += ch2;
                    end = pos + 2;
                }
                token_type = Operator_token.at(Operator_Map.find(token_content)->second);
            }
            else if (ch1 == '=')
            {
                token_content += ch1;
                token_type = Operator_token.at(Operator_Map.find(token_content)->second);
            }
                
            else
                end = pos;
        }    // >,>= >>,>>= ,< <=,<< ,<<= 8
        

        else if (ch == '&'||ch=='|') 
        {
            token_content = token_type = ch;
            char ch1 = str[pos + 1];
            end = pos + 1;
            if (ch1 == ch||ch1=='=')
                token_content += ch1;
            else 
                end = pos;
                
        } // &&,&=,&,| ,|=,|| 6

       
        else 
            return std::make_tuple(false,"none","LEXICAL:ERROR::operator required\n");
           
        
    }
    //成功
    col = cur_col + (end - pos) + 1;
    token_type= Operator_token.at(Operator_Map.find(token_content)->second);
    return std::make_tuple(true, token_content, token_type);
}



extern std::set<std::string> Identifier;
void IdentifierAnalyzer::InsertToken(std::string ch)//插入标识符表里
{
    //动态加入标识符
    Identifier.insert(ch);
}

std::tuple<bool, std::string, std::string> IdentifierAnalyzer::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    uint pos = start;
    uint cur_col = col;
    std::string token_content;

    bool tag = true;
    if (isLetter(str[pos]))
    {
        while (true)
        {
            if (isLetter(str[pos]) || (isDigit(str[pos])))
            {//由于传入前已经特判，所以一定不是数字开头
                token_content += str[pos];
                pos++;
                cur_col++;
                
            }
            else
                break;
        }
    }
    else
        tag = false;

    end = pos - 1;
    col = cur_col;

    if (!tag)
        return std::make_tuple(false, "None", "LEXICAL ERROR::Identifier required\n");
    else
    {
        InsertToken(token_content);
        
        return std::make_tuple(true, token_content, "IDENTIFIER");
    }
}



extern std::string ReserveWord[RESERVED_WORD_NUM];
std::tuple<bool, std::string, std::string> ReservedWordAnalyzer::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    uint pos = start;
    uint cur_col = col;
    std::string token_content;

    bool tag = true;
    if (isLetter(str[pos]))
    {
        while (true)
            if (isLetter(str[pos]) || isDigit(str[pos]))
            {
                token_content += str[pos];
                pos++;
                cur_col++;
            }
            else
                break;
        auto iter = ReservedWord_Map.find(token_content);
        if (iter != ReservedWord_Map.end())
        {
            end = pos - 1;//to relocate the reserve word
            col = cur_col;
            tag = true;
        }
        else
            tag = false;
    }
    else
        tag = false;



    if(!tag)
        return std::make_tuple(false, "None", "LEXICAL ERROR::Reserved Word required\n");
    else
    {
        std::string token_type=token_content;
        std::transform(token_type.begin(), token_type.end(), token_type.begin(), ::toupper);
        return std::make_tuple(true, token_content,token_type);
    }

}

ReservedWordAnalyzer::ReservedWordAnalyzer()
{
    //建立保留字表中值与索引的mapping关系
    for (unsigned int i = 0; i < RESERVED_WORD_NUM; i++)
        ReservedWord_Map[ReserveWord[i]] = i;
}



int Preprocessor::preprocess(std::string& in_str, std::string& out_str, int& line, int& col)
{
    //预处理
    out_str = "";
    unsigned int index = 0;
    int cur_col = col;
    int cur_line = line;
    while (index < in_str.size()) {
        char ch = in_str[index];
        //去注释
        if (ch == '/') {
            char ch1 = in_str[++index];
            if (index >= in_str.size()) {
                line = cur_line;
                col = cur_col;
                return NOT;
            }
            if (ch1 == '/') {// "//"
                while ((ch1 = in_str[++index]) != '\n') {
                    cur_col++;
                }
                out_str += '\n';
                cur_line++;
                cur_col = 0;
            }
            else if (ch1 != '*') {
                out_str += '/';
                out_str += ch1;
                cur_col++;
            }
            else {// "/*    */"
                char ch2 = in_str[++index];
                if (index >= in_str.size()) {
                    line = cur_line;
                    col = cur_col;
                    return NOT;
                }
                while (1) {
                    while (ch2 != '*') {
                        if (ch2 == '\n') {
                            cur_line++;
                            cur_col = 0;
                            out_str += '\n';
                        }
                        ch2 = in_str[++index];
                        cur_col++;
                        if (index >= in_str.size()) {
                            line = cur_line;
                            col = cur_col;
                            return NOT;
                        }
                    }
                    ch2 = in_str[++index];
                    cur_col++;
                    if (index >= in_str.size()) {
                        line = cur_line;
                        col = cur_col;
                        return NOT;
                    }
                    if (ch2 == '/')
                        break;
                    else if (ch2 == '\n') {
                        cur_line++;
                        cur_col = 0;
                        out_str += '\n';
                    }
                }
            }
        }

        //去Tab->空格
        else if (ch == '\t') {
            out_str += ' ';
            cur_col++;
        }
        else if (ch == '\n') {
            out_str += '\n';
            cur_line++;
            cur_col = 0;
        }
        //正常
        else {
            out_str += ch;
            cur_col++;
        }
        index++;
    }
    line = cur_line;
    col = cur_col;
    return 1;//正常返回
}

std::tuple<bool, std::string, std::string> Preprocessor::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    return std::make_tuple(true, "", "");
}


std::tuple<bool, std::string, std::string> const_char_DFA:: isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{

    std::string tmp = str.substr(start);
    bool re = std::regex_search(tmp, tmp_match_result, const_char_dfa);
    if (!re)
        return std::make_tuple(false,"none", "LEXICAL ERROR::the const character can't be identidied\n");
    else
    {
        end =start+ tmp_match_result[0].str().size()-1;
        col += tmp_match_result[0].str().size();
        return std::make_tuple(true, tmp_match_result[0].str(), "CONSTANT");
    }

}



std::tuple<bool, std::string, std::string> literal_string_DFA::isAccepted(const std::string& str, unsigned int& start, unsigned int& end, unsigned int& line, unsigned int& col)
{
    std::string tmp = str.substr(start);
    bool re = std::regex_search(tmp, tmp_match_result, literal_string_dfa);
    if (!re)
        return std::make_tuple(false, "none", "LEXICAL ERROR::the literal string is not closed\n");
    else
    {
        end = start + tmp_match_result[0].str().size() - 1;
        col += tmp_match_result[0].str().size();
        return std::make_tuple(true, tmp_match_result[0].str(), "STRING_LITERAL");
    }
}