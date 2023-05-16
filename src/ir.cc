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

std::string IR::gen_argument_ir(var_node node)
{
    // here pointer are not considered for now
    if(node.id<0)// temp
        return "arg "+node.name;
    else // user defined variables
        return "arg var"+std::to_string(node.id);
}

std::string IR::get_node_name(var_node node) 
{
    if(node.id<0)// temp variable
        return node.name;
    else // a user defined variable
        return ("var"+std::to_string(node.id));
}

std::string IR::gen_array_name(array_node node)
{
    return "array"+std::to_string(node.id);
}

std::string IR::gen_binary_operation_ir(std::string result,var_node a,var_node b,std::string op)
{
    std::string re=result+" := ";
    if(a.id==-1)
        re+=a.name;
    else
        re+="var"+std::to_string(a.id);
    re=re+" "+op+" ";
    if(b.id==-1)
        re+=b.name;
    else
        re+="var"+std::to_string(b.id);

    return re;
}

std::string IR::gen_label_name()
{
    return "label"+std::to_string(num_label++);
}

std::string IR::gen_return_ir(var_node node)
{
    std::string re="RESULT ";
    if(node.id==-1)
        re+=node.name;
    else 
        re+="var"+std::to_string(node.id);
}

void IR::output_ir()
{
    std::ofstream f("ir.mcc");
    if(!f.is_open())
        std::cerr<<"can not open file for ir.\n";
    else
        for(auto & ir:ir_list)
            f<<ir<<'\n';
    f.close();
}
