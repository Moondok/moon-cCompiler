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
    unsigned int num_var=0;
    std::string gen_parameter_ir(var_node node);
    void add_ir(std::string);


};


#endif