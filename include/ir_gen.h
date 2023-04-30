#ifndef __IR_GEN_H__
#define __IR_GEN_H__
#include <AST.h>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <block.h>
#include <ir.h>


typedef std::tuple<bool, std::string , unsigned int ,unsigned int> return_msg;

class ir_gen
{
public:
std::vector<Block> block_stack;

IR ir;

ir_gen(){};



return_msg analyze_tree(std::shared_ptr<AST> root);

void analyze_init_declarator(std::shared_ptr<AST> root, std::string var_type) ;

void analyze_parameter_declaration(std::shared_ptr<AST> root,const std::string &func_name, bool definite);

var_node analyze_assignment_expression(std::shared_ptr<AST> assign_exp);

bool lookup_var(std::string var_name);

private:
    std::string error_msg;

    std::map<std::string, func_node> func_pool;


};



#endif