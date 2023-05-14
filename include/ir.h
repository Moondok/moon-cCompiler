#ifndef __IR_H__
#define __IR_H__

#include <vector>
#include <string>
#include <block.h>

class IR
{
private:
    std::vector<std::string> ir_list;


public:
    unsigned int num_var=0;  //user defined variable
    unsigned int num_temp=0; //temp variable
    std::string gen_parameter_ir(var_node node);
    std::string gen_argument_ir(var_node node);
    std::string get_node_name(var_node node); // get the correct ir representation for a variable;
    std::string get_array_name(array_node node);// same for the above
    void add_ir(std::string);


};


#endif