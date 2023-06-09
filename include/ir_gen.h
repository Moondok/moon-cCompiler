#ifndef __IR_GEN_H__
#define __IR_GEN_H__
#include <AST.h>
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <block.h>
#include <ir.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>

typedef std::tuple<bool, std::string , unsigned int ,unsigned int> return_msg;

class ir_gen
{
public:
std::vector<Block> block_stack;
std::fstream ofile;

IR ir;

int main_function_id=-1; // this variable restores the block id which is min function

ir_gen()
{
    ofile.open("block_table_cache",std::ios::out);
};

struct error_info
{
    std::string error_msg;
    unsigned int line;
    unsigned int col;
    error_info(){};
    error_info(std::string e,unsigned int l,unsigned int c):error_msg(e),line(l),col(c){};
} ;


void analyze_tree(const std::shared_ptr<AST> & root);

void analyze_selection_statement(const std::shared_ptr<AST> & root);

void analyze_jump_statement(const std::shared_ptr<AST> & root);

void analyze_expression_statement(const std::shared_ptr<AST> & root);

void analyze_statement(const std::shared_ptr<AST> & root);

void analyze_iteration_statement(const std::shared_ptr<AST> & root);

void analyze_compound_statement(const std::shared_ptr<AST> & root);

void analyze_function_definition(const std::shared_ptr<AST> &root);//// the return type is not consistent to the demo

void analyze_declaration(const std::shared_ptr<AST>& root);// the return type is not consistent to the demo

void analyze_init_declarator_list(std::shared_ptr<AST> root, std::string var_type) ;

void analyze_init_declarator(const std::shared_ptr<AST> &root,std::string var_type);

void analyze_parameter_list(const std::shared_ptr<AST>& root,std::string func_name,bool is_definite);

void analyze_parameter_declaration(const std::shared_ptr<AST>& root,const std::string &func_name, bool definite);

var_node analyze_assignment_expression(std::shared_ptr<AST> assign_exp);

var_node analyze_logical_or_expression(const std::shared_ptr<AST>& logical_or_exp);

var_node analyze_logical_and_expression(const std::shared_ptr<AST> &logical_and_exp);

var_node analyze_inclusive_or_expression(const std::shared_ptr<AST> &inclusive_or_exp);

var_node analyze_exclusive_or_expression(const std::shared_ptr<AST> &exclusive_or_exp);

var_node analyze_and_expression(const std::shared_ptr<AST> & and_exp);

var_node analyze_equality_expression(const std::shared_ptr<AST> &  equality_exp);

var_node analyze_relational_expression(const std::shared_ptr<AST> & relational_exp);

var_node analyze_shift_expression(const std::shared_ptr<AST> & shift_exp);

var_node analyze_additive_expression(const std::shared_ptr<AST> & additive_exp);

var_node analyze_multiplicative_expression(const std::shared_ptr<AST> & multiplicative_exp);

var_node analyze_unary_expression(const std::shared_ptr<AST> &  unary_exp);

var_node analyze_postfix_expression(const std::shared_ptr<AST> & postfix_exp);

var_node analyze_primary_expression(const std::shared_ptr<AST> & primary_exp);

var_node analyze_expression(const std::shared_ptr<AST> & exp);

void analyze_argument_expression_list(const std::shared_ptr<AST> & node, std::string func_name);

bool lookup_current_var(std::string var_name);

var_node loopup_node(std::string var_name);

var_node create_temp_var(std::string var_name,std::string var_type);

array_node loopup_array(std::string name);

int get_break_block_id();

std::string get_func_rtype();

void optimize();// this function is to optimize ir

bool get_result();

~ir_gen();
private:
    std::vector<error_info> error_infos;

    std::map<std::string, func_node> func_pool;


};



#endif