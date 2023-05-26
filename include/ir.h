#ifndef __IR_H__
#define __IR_H__

#include <vector>
#include <string>
#include <block.h>
#include <fstream>
#include <iostream>

class IR
{
private:
    


public:
    std::vector<std::string> ir_list;
    unsigned int num_var=0;  //user defined variable
    unsigned int num_temp=0; //temp variable
    unsigned int num_arr=0;
    unsigned int num_label=0;
    int num_block=0;

    std::string gen_binary_operation_ir(std::string result,var_node a,var_node b,std::string op);
    std::string gen_parameter_ir(var_node node);
    std::string gen_argument_ir(var_node node);
    std::string get_node_name(var_node node); // get the correct ir representation for a variable;
    std::string gen_array_name(array_node node);// same for the above
    std::string gen_label_name();
    std::string gen_return_ir(var_node node);
    void output_ir();
    void add_ir(std::string);


};


#endif