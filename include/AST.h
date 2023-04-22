#ifndef __AST__H__
#define __AST__H__
#include <memory>
#include <string>


class AST
{
public:
    std::string content;
    std::string name;
    unsigned int line;
    unsigned int col;
    std::shared_ptr<AST> left_child;
    std::shared_ptr<AST> right_child;

AST()
{
    content="";
    name="";
    line=col=-1;
    left_child=right_child=nullptr;
};

AST(std::string c,std::string n="",unsigned int l=-1,unsigned int co=-1,std::shared_ptr<AST> l_child=nullptr,std::shared_ptr<AST> r_child=nullptr):
content(c),name(n),line(l),col(co),left_child(l_child),right_child(r_child)
{


}

};




#endif