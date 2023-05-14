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

//++a; 
var_node ir_gen:: analyze_unary_expression(const std::shared_ptr<AST> &  unary_exp)
{
    if(unary_exp->left_child->name=="postfix_expression")
    {
        std::shared_ptr<AST> postfix_exp=unary_exp->left_child;
        return analyze_postfix_expression(postfix_exp);
    }
    else if(unary_exp->left_child->name=="INC_OP")
    {
        var_node rnode=analyze_unary_expression(unary_exp->left_child->right_child);
        if(rnode.type!="int")
            error_msg="\'++\' operation only suits for int .\n";

        std::string temp_name="temp"+std::to_string(ir.num_temp++);
        var_node constant_node=this->create_temp_var(temp_name,"int");
        block_stack.back().var_map.insert(std::make_pair(temp_name,constant_node));
        ir.add_ir(temp_name+" := #1");

        //for ++a 's return value is a+1, there is no need to create a brand new variable
        ir.add_ir(ir.get_node_name(rnode)+" := "+ir.get_node_name(rnode)+" + "+temp_name);

        return rnode;
    }
    else if (unary_exp->left_child->name=="DEC_OP")
    {
        var_node rnode=analyze_unary_expression(unary_exp->left_child->right_child);
        if(rnode.type!="int")
            error_msg="\'--\' operation only suits for int .\n";

        std::string temp_name="temp"+std::to_string(ir.num_temp++);
        var_node constant_node=this->create_temp_var(temp_name,"int");
        block_stack.back().var_map.insert(std::make_pair(temp_name,constant_node));
        ir.add_ir(temp_name+" := #1");

        //for ++a 's return value is a+1, there is no need to create a brand new variable
        ir.add_ir(ir.get_node_name(rnode)+" := "+ir.get_node_name(rnode)+" - "+temp_name);

    }
    else if(unary_exp->left_child->name=="unary_operator")
    {
        std::string op=unary_exp->left_child->left_child->name;
        var_node rnode=analyze_unary_expression(unary_exp->left_child->right_child) ;// change to cast_expression later

        if(op=="+")
        {
            if(rnode.type!="int"&&rnode.type!="double"&&rnode.type!="float")
                error_msg="operator \'+\' can only be used in numerical tyoe.\n";
            return rnode;
        }
        else if(op=="-")
        {
            if(rnode.type!="int"&&rnode.type!="double"&&rnode.type!="float")
                error_msg="operator \'+\' can only be used in numerical tyoe.\n";
            
            std::string temp_name="temp"+std::to_string(ir.num_temp++);

            // create a zero node for 0-a=-a
            var_node zero_node=this->create_temp_var(temp_name,rnode.type);
            block_stack.back().var_map.insert(std::make_pair(temp_name,zero_node));

            ir.add_ir(temp_name+" := #0");

            std::string return_temp_name="temp"+std::to_string(ir.num_temp++);
            var_node new_node=create_temp_var(return_temp_name,rnode.type);
            block_stack.back().var_map.insert(std::make_pair(return_temp_name,new_node));
            ir.add_ir(return_temp_name+" :="+temp_name+" - "+ir.get_node_name(rnode));
            return new_node;
        }
        else if(op=="~") // only suitable for int ,implemented by xor 
        {
            if(rnode.type!="int")
                error_msg="operator \'~\' can only be used in numerical tyoe.\n";
            
            std::string temp_name="temp"+std::to_string(ir.num_temp++);

            // create a zero node for 0-a=-a
            var_node zero_node=this->create_temp_var(temp_name,rnode.type);
            block_stack.back().var_map.insert(std::make_pair(temp_name,zero_node));

            ir.add_ir(temp_name+" := #-2147483648"); //11111111(32 bits)

            std::string return_temp_name="temp"+std::to_string(ir.num_temp++);
            var_node new_node=create_temp_var(return_temp_name,rnode.type);
            block_stack.back().var_map.insert(std::make_pair(return_temp_name,new_node));
            // by performing 1111 ^ a ,inverse a's every bit.
            ir.add_ir(return_temp_name+" :="+temp_name+" ^ "+ir.get_node_name(rnode));
            return new_node;
        }
    
    }
    // *, & and ! is on the way.

}

//a++ a--, a(), a[] ,a
var_node ir_gen::analyze_postfix_expression(const std::shared_ptr<AST> & postfix_exp)
{
    if(postfix_exp->left_child->name=="primary_expression")
    {
        std::shared_ptr<AST> primary_exp=postfix_exp->left_child;
        return analyze_primary_expression(primary_exp);
    }
    else if (postfix_exp->left_child->right_child->name=="[")
    {
        std::string array_name=postfix_exp->left_child->left_child->left_child->content;
        std::shared_ptr<AST> exp=postfix_exp->left_child->right_child->right_child;
        var_node length_node=analyze_expression(exp);
        array_node new_array_node=loopup_array(array_name);
        if(new_array_node.num==-1)
            error_msg="undefined array "+array_name;

        std::string temp_name="temp"+std::to_string(ir.num_var);
        var_node new_temp_var(temp_name,new_array_node.type,ir.num_temp++,true);
        this->block_stack.back().var_map.insert(std::make_pair(temp_name,new_temp_var));

        if(new_array_node.type=="int"||new_array_node.type=="float"||new_array_node.type=="double")
        {
            
            std::string temp_index_name="temp"+std::to_string(ir.num_temp++);
            var_node temp_var2(temp_index_name,"int");// this is the index of the array
            this->block_stack.back().var_map.insert(std::make_pair(temp_index_name,temp_var2));

            if(new_array_node.type=="int"||new_array_node.type=="float")
            {
                std::string temp_constant_name="temp"+std::to_string(ir.num_temp++);// stores 4
                var_node temp_var3(temp_constant_name,"int");
                block_stack.back().var_map.insert(std::make_pair(temp_constant_name,temp_var3));

                ir.add_ir(temp_constant_name+" :=#4");
                ir.add_ir(temp_index_name+ " :="+ir.get_node_name(length_node)+" * "+temp_index_name);
            }

            else if(new_array_node.type=="double")
            {
                std::string temp_constant_name="temp"+std::to_string(ir.num_temp++);
                var_node temp_var3(temp_constant_name,"int");
                this->block_stack.back().var_map.insert(std::make_pair(temp_constant_name,temp_var3));

                ir.add_ir(temp_constant_name+" :=#8"); //type double
                ir.add_ir(temp_index_name+ " :="+ir.get_node_name(length_node)+" * "+temp_index_name);
            }
            //assign the value from an array

            ir.add_ir(temp_name+" :=&"+ir.get_array_name(new_array_node)+" + "+ir.get_node_name(length_node));
            return new_temp_var;
        }
        
        else if (postfix_exp->left_child->right_child->name=="(")
        {
            std::string func_name=postfix_exp->left_child->left_child->left_child->content;
            func_node func=func_pool[func_name];
            
            var_node new_var_node; // restore the return value of a function call.
            if(func_pool.find(func_name)==func_pool.end())
                error_msg="undefined function "+func_name;
            if(postfix_exp->left_child->right_child->right_child->name=="argument_expression_list")
            {
                std::shared_ptr<AST> argument_expression_list=postfix_exp->left_child->right_child->right_child;
                analyze_argument_expression_list(argument_expression_list,func_name);
            }

            if(func.rtype=="void")
                ir.add_ir("call "+func_name);
            else
            {
                std::string temp_name="temp"+std::to_string(ir.num_temp++);
                new_var_node=this->create_temp_var(temp_name,func.rtype);
                ir.add_ir(temp_name+"call "+func_name);
            }

            return new_var_node;

        }
        else if(postfix_exp->left_child->right_child->name=="INC_OP")
        {
            var_node current_node=analyze_postfix_expression(postfix_exp->left_child);
            if(current_node.type!="int")
                error_msg="\'++\' operation only suits for int .\n";
            
            std::string temp_name="temp"+std::to_string(ir.num_temp++);
            var_node return_node=this->create_temp_var(temp_name,"int");
            block_stack.back().var_map.insert(std::make_pair(temp_name,return_node));

            std::string temp_constant_name="temp"+std::to_string(ir.num_temp++);
            var_node constant_var_node=this->create_temp_var(temp_constant_name,"int");
            block_stack.back().var_map.insert(std::make_pair(temp_constant_name,constant_var_node));

            ir.add_ir(temp_constant_name+" := #1");

            ir.add_ir(temp_name=" :="+ir.get_node_name(current_node)); // the return value remains the value of vurrent var
            ir.add_ir(ir.get_node_name(current_node)+" := "+ir.get_node_name(current_node)+" + "+temp_constant_name);

            return return_node;
        }

        else if(postfix_exp->left_child->right_child->name=="DEC_OP")
        {
            var_node current_node=analyze_postfix_expression(postfix_exp->left_child);
            if(current_node.type!="int")
                error_msg="\'--\' operation only suits for int .\n";

            std::string temp_name="temp"+std::to_string(ir.num_temp++);
            var_node return_node=this->create_temp_var(temp_name,"int");
            block_stack.back().var_map.insert(std::make_pair(temp_name,return_node));

            std::string temp_constant_name="temp"+std::to_string(ir.num_temp++);
            var_node constant_var_node=this->create_temp_var(temp_constant_name,"int");
            block_stack.back().var_map.insert(std::make_pair(temp_constant_name,constant_var_node));

            ir.add_ir(temp_constant_name+" := #1");

            ir.add_ir(temp_name=" :="+ir.get_node_name(current_node)); // the return value remains the value of vurrent var
            ir.add_ir(ir.get_node_name(current_node)+" := "+ir.get_node_name(current_node)+" - "+temp_constant_name);

            return return_node;
        }
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

void ir_gen::analyze_argument_expression_list(const std::shared_ptr<AST> & node, std::string func_name)
{
    std::shared_ptr<AST> start=node->left_child;
    func_node func=this->func_pool[func_name];
    unsigned int cnt=0;
    //cuz we may have multiple arguments, so here we adopt a loop 
    while(start->name=="argument_expression_list")
    {
        // note that by this way , the argument in our IR are in the inverse order!
        var_node new_arg_node=analyze_assignment_expression(start->right_child->right_child);
        ir.add_ir(ir.gen_argument_ir(new_arg_node));

        start=start->left_child;
        cnt++;
        if(cnt>=func.para_list.size())
            error_msg="too many atguments passed to our function.\n";
        if(func.para_list[func.para_list.size()-cnt].type!=new_arg_node.type)
            error_msg="the type of argument you passed is not match the type you defined.\n";
    }

    var_node last_arg_node=analyze_assignment_expression(start);
    ir.add_ir(ir.gen_argument_ir(last_arg_node));
    cnt++;
    if(func.para_list[func.para_list.size()-cnt].type!=last_arg_node.type)
        error_msg="the type of argument you passed is not match the type you defined.\n";

    if(cnt!=func.para_list.size())
        error_msg="too less arguments passed in.\n";
        
}

array_node ir_gen::loopup_array(std::string name)
{
    for(auto &block:this->block_stack)
    {
        if(block.arr_map.find(name)!=block.arr_map.end())
            return block.arr_map[name];
    }
    array_node tmp;
    tmp.num=-1;
    return tmp;
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