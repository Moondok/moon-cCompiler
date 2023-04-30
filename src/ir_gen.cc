#include <ir_gen.h>

return_msg ir_gen::analyze_tree(std::shared_ptr<AST> root)
{
    if (root==nullptr)
        return return_msg(true,"none",-1,-1);

    else if (root->name=="declaration")
        return analyze_declaration(root);

    else if (root->name=="statement")
        return analyze_statement(root);
    
    else if(root->name=="function_definition")
        return analyze_function_definition(root);
    
    else
    {

    }
}

ir_gen:: analyze_declaration(std::shared_ptr<AST> root)
{
    std::string error_msg="";
    std::string var_type=root->left_child->name;

    if (root->left_child->right_child->name==";")
        return ; // don't know how to process     type_specifier ;

    if( var_type=="VOID")
        error_msg="can not assign value to \"void\" type";
    
    else
    {
        analyze_init_declarator_list(root->left_child->right_child,var_type);
    }
   
}

ir_gen::analyze_init_declarator_list(std::shared_ptr<AST> root, std::string type)
{
    if(root->left_child->name=="init_declarator_list")
        analyze_init_declarator_list(root->left,type);
    else if(node->left_child->name=="init_declarator")
        analyze_init_declarator(root->left,type);
    
    if(root->right_child->name==",")
        analyze_init_declarator(root->right_child->right_child,type);
}

void ir_gen::analyze_init_declarator(std::shared_ptr<AST> root, std::string var_type)
{
    std::shared_ptr<AST> declarator=root->left_child;

    if(declarator->right_child==nullptr) // no initialization
    {
        if(declarator->left_child->name=="IDENTIFIER")
        {
            std::string var_name=declarator->left_child->content;
            if(!lookup_var(var_name))
            {
                var_node new_var;
                new_var.name=var_name;
                new_var.type=var_type;
                new_var.id=this->ir.num_var++;
                this->block_stack.back().var_map.insert(std::make_pair(var_name,new_var));

            }
            else
                error_msg="multiple definitions for variable"+var_name+'\n';

        }

        else
        {
            //function definition
            if(declarator->left_child->right_child->name=="(")
            {
                std::string func_name=declarator->left_child->left_child->content;
                std::string func_rtype=var_type;
                if (this->block_stack.size()>1)
                    error_msg="function can not be defined in a block.\n";
                
                std::shared_ptr<AST> para_list=declarator->left_child->right_child->right_child;
                func_node new_func;
                new_func.is_defined=false; //cuz it is in declaration
                new_func.name=func_name;
                new_func.rtype=func_rtype;
                this->func_pool.insert(std::make_pair(func_name,new_func));

                analyze_parameter_list(declarator->left_child->right_child->right_child,func_name,false);

            }

            else if (declarator->left_child->right_child->name=="[")
            {
                std::string arr_name=declarator->left_child->left_child->content;
                std::string arr_type=var_type;
                std::shared_ptr<AST> assign_exp=declarator->left_child->right_child->right_child;
                var_node rnode=analyze_assignment_expression(assign_exp);
            }
        }
    }
}

void ir_gen::analyze_parameter_list(std::shared_ptr<AST> root,std::string func_name, bool definite)
{
    if(root->left_child->name=="parameter_list")
        analyze_parameter_list(root->left_child,func_name,definite);
    else
        analyze_parameter_declaration(root->left,func_name,definite);
    

    if(root->left_child->right_child->name==",")
    {
        analyze_parameter_declaration(root->left_child->right_child->right_child,func_name,definite);

    }
}

void ir_gen::analyze_parameter_declaration(std::shared_ptr<AST> root,const std::string &func_name, bool definite)
{
    if(root->left_child->left_child->name=="VOID")
        this->error_msg="can not define a variable with \'void\' type\n";

    std::shared_ptr<AST> declarator=root->left_child->right_child;
    std::string var_name=declarator->left_child->content;
    var_node new_var;
    new_var.name=var_name;
    new_var.type=root->left_child->left_child->content;

    if(definite)
    {
        new_var.id=this->ir.num_var++;
        block_stack.back().func.para_list.emplace_back(new_var);
    }

    func_pool[func_name].para_list.emplace_back(new_var);

    if(definite)
        ir.gen_parameter_ir(new_var);

}


bool ir_gen::lookup_var(std::string var_name)
{
    return this->block_stack.back().var_map.find(var_name)!=this->block_stack.back().var_map.end();
}