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
    {
        ir.add_ir(ir.gen_parameter_ir(new_var));
    }

}

var_node ir_gen::analyze_assignment_expression(std::shared_ptr<AST> assign_exp)
{
    if(assign_exp->left_child->name=="logical_or_expression")
    {
        std::shared_ptr<AST> logical_or_exp=assign_exp->left_child;
        return this->analyze_logical_or_expression(logical_or_exp);
    }
}

var_node analyze_logical_or_expression(const std::shared_ptr<AST>& logical_or_exp)
{
    if(logical_or_exp->left_child->name=="logical_and_expression")
    {
        std::shared_ptr<AST> logical_and_exp=logical_or_exp->left_child;
        return analyze_logical_and_expression(logical_and_exp);
    }
}

var_node analyze_logical_and_expression(const std::shared_ptr<AST> &logical_and_exp)
{
    if(logical_and_exp->left_child->name=="inclusive_or_expression")
    {
        std::shared_ptr<AST> inclusive_or_exp=logical_and_exp->left_child;
        return analyze_inclusive_or_expression(inclusive_or_exp);
    }
}

var_node analyze_inclusive_or_expression(const std::shared_ptr<AST> &inclusive_or_exp)
{
    if(inclusive_or_exp->left_child->name=="exclusive_or_expression")
    {
        std::shared_ptr<AST> exclusive_or_exp=inclusive_or_exp->left_child;
        return this->analyze_exclusive_or_expression(exclusive_or_exp);
    }
}

var_node analyze_exclusive_or_expression(const std::shared_ptr<AST> &exclusive_or_exp)
{
    if(exclusive_or_exp->left_child->name=="and_expression")
    {
        std::shared_ptr<AST> and_exp=exclusive_or_exp->left_child;
        return analyze_and_expression(and_exp);
    }
}

var_node analyze_and_expression(const std::shared_ptr<AST> & and_exp)
{
    if(and_exp->left_child->name=="equality_expression")
    {
        std::shared_ptr<AST> equality_exp=and_exp->left_child;
        return analyze_equality_expression(equality_exp);
    }
}

var_node ir_gen::analyze_equality_expression(const std::shared_ptr<AST> &  equality_exp)
{
    if(equality_exp->left_child->name=="relational_expression")
    {
        std::shared_ptr<AST> relational_exp=equality_exp->left_child;
        return analyze_relational_expression(relational_exp);
    }
}

var_node ir_gen::analyze_relational_expression(const std::shared_ptr<AST> & relational_exp)
{
    if(relational_exp->left_child->name=="shift_expression")
    {
        std::shared_ptr<AST> shift_exp=relational_exp->left_child;
        return analyze_shift_expression(shift_exp);
    }
}

var_node ir_gen::analyze_shift_expression(const std::shared_ptr<AST> & shift_exp)
{
    if(shift_exp->left_child->name=="additive_expression")
    {
        std::shared_ptr<AST>  additive_exp=shift_exp->left_child;
        return analyze_additive_expression(additive_exp);
    }
}

var_node ir_gen::analyze_additive_expression(const std::shared_ptr<AST> & additive_exp)
{
    if(additive_exp->left_child->name=="multiplicative_expression")
    {
        std::shared_ptr<AST> multiplicative_exp=additive_exp->left_child;
        return analyze_multiplicative_expression(multiplicative_exp);
    }
}

var_node ir_gen::analyze_multiplicative_expression(const std::shared_ptr<AST> & multiplicative_exp)
{
    if(multiplicative_exp->left_child->name=="unary_expression")
    {
        std::shared_ptr<AST> unary_exp=multiplicative_exp->left_child;
        return analyze_unary_expression(unary_exp);
    }
}

var_node ir_gen:: analyze_unary_expression(const std::shared_ptr<AST> &  unary_exp)
{
    if(unary_exp->left_child->name=="postfix_expression")
    {
        std::shared_ptr<AST> postfix_exp=unary_exp->left_child;
        return analyze_postfix_expression(postfix_exp);

    }
}

var_node ir_gen::analyze_postfix_expression(const std::shared_ptr<AST> & postfix_exp)
{
    if(postfix_exp->left_child->name=="primary_expression")
    {
        std::shared_ptr<AST> primary_exp=postfix_exp->left_child;
        return analyze_primary_expression(primary_exp);
    }
}

var_node ir_gen::analyze_primary_expression(const std::shared_ptr<AST> & primary_exp)
{
    if(primary_exp->left_child->name=="IDENTIFIER")
    {
        //find it in the table built in var table in current block
        std::string var_name=primary_exp->left_child->content;

        var_node node=this->loopup_node(var_name);
        if(node.id==-1)
            error_msg="undefined variable "+var_name+".\n";
        return node;
    }

    else if(primary_exp->left_child->name=="TRUE"||primary_exp->left_child->name=="FALSE")
    {
        std::string temp_var_name="temp"+std::to_string(ir.num_var++);
        var_node new_var_node=create_temp_var(temp_var_name,"bool");
        this->block_stack.back().var_map.insert(std::make_pair(temp_var_name,new_var_node));

        if(primary_exp->left_child->name=="TRUE")
            ir.add_ir(temp_var_name+" := #1");
        else
            ir.add_ir(temp_var_name+" := #0");
        return new_var_node;


    }
    else if (primary_exp->left_child->name=="CONSTANT_INT")
    {
        std::string content=primary_exp->left_child->content;
        std::string temp_var_name="temp"+std::to_string(ir.num_var++);
        var_node new_var_node=create_temp_var(temp_var_name,"int");
        this->block_stack.back().var_map.insert(std::make_pair(temp_var_name,new_var_node));

        ir.add_ir(temp_var_name+" := #"+ content); // we will distinguish int and float here later.
        return new_var_node;
    }

    else if (primary_exp->left_child->name=="CONSTANT_DOUBLE")
    {
        std::string content=primary_exp->left_child->content;
        std::string temp_var_name="temp"+std::to_string(ir.num_var++);
        var_node new_var_node=create_temp_var(temp_var_name,"double");
        this->block_stack.back().var_map.insert(std::make_pair(temp_var_name,new_var_node));

        ir.add_ir(temp_var_name+" := #f"+ content); // we will distinguish int and float here later.
        return new_var_node;
    }

    else if(primary_exp->left_child->name=="(")
    {
        std::shared_ptr<AST> exp=primary_exp->left_child->right_child;
        return analyze_expression(exp);
    }
}

var_node ir_gen::analyze_expression(const std::shared_ptr<AST> & exp)
{
    
    
    if (exp->left_child->name=="assignment_expression")
        return analyze_assignment_expression(exp->left_child);
    else if(exp->left_child->name=="expression")
    {
        var_node new_var_node=analyze_expression(exp->left_child);
        // maybe a "," exists, so we do not return directly.
        var_node tmp_var_node=analyze_assignment_expression(exp->right_child->right_child);
        return new_var_node;
        // tmp_var_node 's value is not used , but some intermediate results matter.
    }
    
}


bool ir_gen::lookup_var(std::string var_name) // only find var in current block, used for checking multiple definition in the same block
{
    return this->block_stack.back().var_map.find(var_name)!=this->block_stack.back().var_map.end();
}

var_node ir_gen:: loopup_node(std::string var_name) // finding a variable in all hierarchies
{
    for(auto& block:block_stack)
    {
        if(block.var_map.find(var_name)!=block.var_map.end())
            return block.var_map[var_name];
    }
    // no result
    var_node tmp;
    tmp.id=-1;
    return tmp;
}

var_node ir_gen::create_temp_var(std::string var_name,std::string var_type)
{
    var_node new_var_node(var_name,var_type);
    return new_var_node;
}