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

 void print_tree(const std::shared_ptr<AST>& root,std::ofstream & o,unsigned int  layer)
 {
    if(layer==0)
        o<<'{'<<'\n';
    
    for (int i=0;i<layer;i++)
        o<<" ";
    if(root->content.size())
        o<<"\""<<root->name<<" ("<<root->content<<") "<<"\""<<":\n";
    else
        o<<"\""<<root->name<<"\""<<":\n";


    for (int i=0;i<layer+1;i++)
        o<<" ";
    o<<"{\n";

    if(root->left_child!=nullptr)
    {
        print_tree(root->left_child,o,layer+1);

        std::shared_ptr<AST> start=root->left_child->right_child;
        if(start!=nullptr)
        {
            for (int i=0;i<layer+1;i++)
                o<<" ";
            o<<",\n";
        }
        while(start!=nullptr)
        {
            print_tree(start,o,layer+1);
            start=start->right_child;
            if(start!=nullptr)
            {
                for (int i=0;i<layer+1;i++)
                    o<<" ";
                o<<",\n";
            }
        }
        
    }
    o<<"\n";
    for (int i=0;i<layer+1;i++)
        o<<" ";
    o<<"}\n";

    if(layer==0)
        o<<'}'<<'\n';
    
 }


 
 

