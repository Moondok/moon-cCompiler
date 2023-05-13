#include <ir.h>
void IR::add_ir(std::string new_code)
{
    this->ir_list.emplace_back(new_code);
}

std::string IR::gen_parameter_ir(var_node node)
{
    std::string re="param var"+std::to_string(node.id);
    return re;
}
