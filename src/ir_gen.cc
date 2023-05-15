#include <ir_gen.h>

void ir_gen::analyze_tree(const std::shared_ptr<AST> &root)
{
    if (root==nullptr)
        return ;

    else if (root->name=="declaration")
        analyze_declaration(root);

    else if (root->name=="statement")
        analyze_statement(root);
    
    else if(root->name=="function_definition")
        analyze_function_definition(root);
    
    else
    {
        analyze_tree(root->left_child);
        analyze_tree(root->right_child);
    }
}

void ir_gen::analyze_iteration_statement(const std::shared_ptr<AST> & root)
{
    if(root->left_child->name=="WHILE")
    {
        Block new_block;
        new_block.can_break=true;
        block_stack.emplace_back(new_block);

        std::shared_ptr<AST> expression=root->left_child->right_child->right_child;
        std::shared_ptr<AST> statement=expression->right_child->right_child;

        std::string label1=ir.gen_label_name();
        std::string label2=ir.gen_label_name();
        std::string label3=ir.gen_label_name();

        block_stack.back().break_label=label3;

        ir.add_ir("LABEL "+label1+" :");

        var_node condition_var=analyze_expression(expression);

        if(condition_var.type=="bool")
            ir.add_ir("IF "+condition_var.bool_str+" GOTO "+label2);
        else 
        {
            std::string temp_zero_name="temp"+std::to_string(ir.num_temp++);
            var_node temp_node=this->create_temp_var(temp_zero_name,"int");
            ir.add_ir(temp_zero_name+" := #0");
            ir.add_ir("IF "+ir.get_node_name(condition_var)+" != "+temp_zero_name+ " GOTO "+label2);

        }

        ir.add_ir("GOTO "+label3); //terminate
        ir.add_ir("LABEL "+label2+ " :");//sequently execute
        analyze_statement(statement);

        ir.add_ir("GOTO "+label1);// go to judge
        ir.add_ir("LABEL "+label3+" :"); // end 

        block_stack.pop_back();

    }

    else if(root->left_child->name=="DO")
    {
        Block new_block;
        new_block.can_break=true;
        block_stack.emplace_back(new_block);

        std::shared_ptr<AST> statement=root->left_child->right_child;
        std::shared_ptr<AST> exp=statement->right_child->right_child->right_child;

        std::string label1=ir.gen_label_name();
        std::string label2=ir.gen_label_name();

        block_stack.back().break_label=label2;

        //label1 is the beginning , consistency for continue
        ir.add_ir("LABEL "+label1+" :");

        analyze_statement(statement);

        var_node condition_var=analyze_expression(exp);

        if(condition_var.type=="bool")
            ir.add_ir("IF "+condition_var.bool_str+" GOTO "+label1);
        else 
        {
            std::string temp_zero_name="temp"+std::to_string(ir.num_temp++);
            var_node temp_node=this->create_temp_var(temp_zero_name,"int");
            ir.add_ir(temp_zero_name+" := #0");
            ir.add_ir("IF "+ir.get_node_name(condition_var)+" != "+temp_zero_name+" GOTO "+label1);

        }
        ir.add_ir("LABEL "+label2+ " :");
        block_stack.pop_back();
    }
}

void ir_gen::analyze_compound_statement(const std::shared_ptr<AST> &root)
{
    analyze_tree(root);
}

void ir_gen::analyze_function_definition(const std::shared_ptr<AST> &root)
{
    std::shared_ptr<AST> type_specifier=root->left_child;
    std::shared_ptr<AST> declarator=root->left_child->right_child;
    std::shared_ptr<AST> compound_statement=root->left_child->right_child->right_child;

    std::string rtype=type_specifier->left_child->content;
    std::string func_name=declarator->left_child->left_child->content;

    bool has_declaration=false;
    func_node declared_func;
    if(this->func_pool.find(func_name)!=func_pool.end())
    {
        if(func_pool[func_name].is_defined==true)
            error_msg="function "+func_name+" is already defined.\n";
        else
        {
            has_declaration=true;
            declared_func=func_pool[func_name]; // temporary variable for compare paralist
            //func_pool.erase(func_pool.find(func_name));

        }

    }
    Block new_fun_block;
    new_fun_block.is_func=true;
    new_fun_block.func.name=func_name;
    new_fun_block.func.rtype=rtype;

    block_stack.emplace_back(new_fun_block);
    if(has_declaration==false)
        func_pool.insert(std::make_pair(func_name,new_fun_block.func));
    
    ir.add_ir("FUNCTION "+func_name+" :");

    if(declarator->left_child->right_child->right_child->name=="parameter_list")
        analyze_parameter_list(declarator->left_child->right_child->right_child,func_name,true);//parameters written in blocks
    
    func_node func=func_pool[func_name];

    if(has_declaration)
    {
        if(func.rtype!=declared_func.rtype)
            error_msg="the return type when function declared is not match when it is defined.\n";
        if(func.para_list.size()!=declared_func.para_list.size())
            error_msg="the number of parameters when function declared is not match when it is defined.\n";

        for(unsigned int i=0;i<func.para_list.size();i++)
            if(func.para_list[i].type!=declared_func.para_list[i].type)
                error_msg="the parameter "+func.para_list[i].name+" 's type does not match the function declared before.\n";
            
    }

    new_fun_block.func=func;

    analyze_compound_statement(compound_statement);

    block_stack.pop_back();
}

void ir_gen:: analyze_declaration(const std::shared_ptr<AST> &root) // the return type is not consistent to the demo
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

void ir_gen::analyze_init_declarator_list(std::shared_ptr<AST> root, std::string type)
{
    if(root->left_child->name=="init_declarator_list")
        analyze_init_declarator_list(root->left_child,type);
    else if(root->left_child->name=="init_declarator")
        analyze_init_declarator(root->left_child,type);
    
    if(root->right_child->name==",")
        analyze_init_declarator(root->right_child->right_child,type);
}

void ir_gen::analyze_init_declarator(const std::shared_ptr<AST> & root, std::string var_type) //analyze_declararor is hidden in this function implicitly
{
    std::shared_ptr<AST> declarator=root->left_child;

    if(declarator->right_child==nullptr) // no initialization
    {
        if(declarator->left_child->name=="IDENTIFIER")
        {
            std::string var_name=declarator->left_child->content;
            if(!lookup_current_var(var_name))
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
            if(declarator->left_child->right_child->name=="(") // this is function definition
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

                if(declarator->left_child->right_child->right_child->name!=")") // func with parameters
                    analyze_parameter_list(declarator->left_child->right_child->right_child,func_name,false);

            }

            else if (declarator->left_child->right_child->name=="[")
            {
                std::string arr_name=declarator->left_child->left_child->content;
                std::string arr_type=var_type;
                std::shared_ptr<AST> assign_exp=declarator->left_child->right_child->right_child;
                var_node rnode=analyze_assignment_expression(assign_exp);


                // this is an array's definition, we need an constant expression , to be done!
                if(rnode.type!="int")
                    error_msg="the size of an array must be an constant int.\n";

                // for the array's size is an constant ,i do not plan to add perform a calculate ir here.
                array_node new_array_node(arr_name,arr_type,ir.num_arr++); // to be done: add the representation of capacity.
                ir.add_ir("ARRAY "+ir.gen_array_name(new_array_node)+" "+rnode.name);//to be done: here rnode.name is just a placeholder, we need a string representing capacity here!!!
            }
        }
    }

    else if(declarator->right_child->name=="=")
    {
        var_node new_var_node;
        if(declarator->left_child->name=="IDENTIFIER")
        {
            std::string new_var_name=declarator->left_child->content;
            if(!lookup_current_var(new_var_name))
            {
                new_var_node.name=new_var_name;
                new_var_node.type=var_type;
                new_var_node.id=ir.num_var++;
                this->block_stack.back().var_map.insert(std::make_pair(new_var_name,new_var_node));
            }
            else
                error_msg="multiple definitions for variable "+new_var_name+".\n";
        }
        std::shared_ptr<AST> initializer=declarator->right_child->right_child;
        if(initializer->left_child->name=="assignment_expression")
        {
            var_node rnode=this->analyze_assignment_expression(initializer->left_child);

            if(rnode.type!=var_type)
                error_msg="must use same type to perform initialization.\n";
            std::string re="var"+std::to_string(new_var_node.id)+=" := ";
            if(rnode.id==-1)
                re+=rnode.name;
            else
                re+="var"+std::to_string(rnode.id);
            ir.add_ir(re);
        }
    }
}

void ir_gen::analyze_parameter_list(const std::shared_ptr<AST>& root,std::string func_name, bool definite)
{
    if(root->left_child->name=="parameter_list")
        analyze_parameter_list(root->left_child,func_name,definite);
    else
        analyze_parameter_declaration(root->left_child,func_name,definite);
    
    if(root->left_child->right_child->name==",")
    {
        analyze_parameter_declaration(root->left_child->right_child->right_child,func_name,definite);

    }
}

void ir_gen::analyze_parameter_declaration(const std::shared_ptr<AST>& root,const std::string &func_name, bool definite)
{
    if(root->left_child->left_child->name=="VOID")
        this->error_msg="can not define a variable with \'void\' type\n";

    std::shared_ptr<AST> declarator=root->left_child->right_child; // we do not take abstract declarator for now
    std::string var_name=declarator->left_child->content; // the parameter is a single parameter instead of complex ones
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
    var_node left_node=analyze_unary_expression(assign_exp->left_child);
    var_node right_node=analyze_assignment_expression(assign_exp->right_child->right_child);
    var_node tmp_node;
    std::string op=assign_exp->left_child->right_child->left_child->name;
    if(op=="=")
        tmp_node=right_node;
    else
    {
        std::string temp_name="temp"+std::to_string(ir.num_temp++);
        tmp_node=create_temp_var(temp_name,left_node.type);
        block_stack.back().var_map.insert(std::make_pair(temp_name,tmp_node));

        if(left_node.type!=right_node.type)
            error_msg="two operands must have the same type.\n";

        if(op=="MUL_ASSIGN")
            ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"*"));
        else if(op=="DIV_ASSIGN")
            ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"/"));
        else if(op=="MOD_ASSIGN")
        {
            if(left_node.type!="int")
                error_msg="two operands for '%' operation must be int.\n";
            ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"%"));
        }
        else if(op=="ADD_ASSIGN")
            ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"+"));
        else if(op=="SUB_ASSIGN")
            ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"-"));
        else
        {
            if(left_node.type!="int")
                error_msg="two operands for this operation must be int.\n";
            if(op=="LEFT_ASSIGN")
                ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"<<"));
            else if(op=="RIGHT_ASSIGN")
                ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,">>"));
            else if(op=="AND_ASSIGN")
                ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"&"));
            else if(op=="OR_ASSIGN")
                ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"|"));
            else if(op=="XOR_ASSIGN")
                ir.add_ir(ir.gen_binary_operation_ir(temp_name,left_node,right_node,"^"));
        }
    }
    std::string new_code="var"+std::to_string(left_node.id)+" := ";
    if(tmp_node.id==-1)
        new_code+=tmp_node.name;
    else
        new_code+="var"+std::to_string(tmp_node.id);
    
    return left_node;
    
}

var_node ir_gen::analyze_logical_or_expression(const std::shared_ptr<AST>& logical_or_exp)
{
    if(logical_or_exp->left_child->name=="logical_and_expression")
    {
        std::shared_ptr<AST> logical_and_exp=logical_or_exp->left_child;
        return analyze_logical_and_expression(logical_and_exp);
    }
    var_node operand1=analyze_logical_or_expression(logical_or_exp->left_child);
    var_node operand2=analyze_logical_and_expression(logical_or_exp->left_child->right_child->right_child);

    if(operand1.type!="bool"||operand2.type!="bool")
        error_msg="logical or operation can only be performed between bool type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,"||")); //logical and operator &&
    result_node.bool_str=ir.get_node_name(operand1)+" || "+ir.get_node_name(operand2);
    
    return result_node;
}

var_node ir_gen::analyze_logical_and_expression(const std::shared_ptr<AST> &logical_and_exp)
{
    if(logical_and_exp->left_child->name=="inclusive_or_expression")
    {
        std::shared_ptr<AST> inclusive_or_exp=logical_and_exp->left_child;
        return analyze_inclusive_or_expression(inclusive_or_exp);
    }
    var_node operand1=analyze_logical_and_expression(logical_and_exp->left_child);
    var_node operand2=analyze_exclusive_or_expression(logical_and_exp->left_child->right_child->right_child);

    if(operand1.type!="bool"||operand2.type!="bool")
        error_msg="logical and operation can only be performed between bool type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,"&&")); //logical and operator &&
    result_node.bool_str=ir.get_node_name(operand1)+" && "+ir.get_node_name(operand2);
    
    return result_node;
}

var_node ir_gen::analyze_inclusive_or_expression(const std::shared_ptr<AST> &inclusive_or_exp)
{
    if(inclusive_or_exp->left_child->name=="exclusive_or_expression")
    {
        std::shared_ptr<AST> exclusive_or_exp=inclusive_or_exp->left_child;
        return this->analyze_exclusive_or_expression(exclusive_or_exp);
    }
    var_node operand1=analyze_inclusive_or_expression(inclusive_or_exp->left_child);
    var_node operand2=analyze_exclusive_or_expression(inclusive_or_exp->left_child->right_child->right_child);

    if(operand1.type!="int"||operand2.type!="int")
        error_msg="inclusive or operation can only be performed between int type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,inclusive_or_exp->left_child->right_child->name));

    return result_node;
}

var_node ir_gen::analyze_exclusive_or_expression(const std::shared_ptr<AST> &exclusive_or_exp)
{
    if(exclusive_or_exp->left_child->name=="and_expression")
    {
        std::shared_ptr<AST> and_exp=exclusive_or_exp->left_child;
        return analyze_and_expression(and_exp);
    }
    var_node operand1=analyze_exclusive_or_expression(exclusive_or_exp->left_child);
    var_node operand2=analyze_and_expression(exclusive_or_exp->left_child->right_child->right_child);

    if(operand1.type!="int"||operand2.type!="int")
        error_msg="exclusive or operation can only be performed between int type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,exclusive_or_exp->left_child->right_child->name));

    return result_node;
}

var_node ir_gen::analyze_and_expression(const std::shared_ptr<AST> & and_exp)
{
    if(and_exp->left_child->name=="equality_expression")
    {
        std::shared_ptr<AST> equality_exp=and_exp->left_child;
        return analyze_equality_expression(equality_exp);
    }
    var_node operand1=analyze_and_expression(and_exp->left_child);
    var_node operand2=analyze_equality_expression(and_exp->left_child->right_child->right_child);

    if(operand1.type!="int"||operand2.type!="int")
        error_msg="and operation can only be performed between int type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,and_exp->left_child->right_child->name));

    return result_node;
}

var_node ir_gen::analyze_equality_expression(const std::shared_ptr<AST> &  equality_exp)
{
    if(equality_exp->left_child->name=="relational_expression")
    {
        std::shared_ptr<AST> relational_exp=equality_exp->left_child;
        return analyze_relational_expression(relational_exp);
    }
    std::string op=equality_exp->left_child->right_child->name;
    op=(op=="EQ_OP")?"==":op;
    op=(op=="NE_OP")?"!=":op;

    var_node operand1=analyze_equality_expression(equality_exp->left_child);
    var_node operand2=analyze_relational_expression(equality_exp->left_child->right_child->right_child);

    if(operand1.type!=operand2.type)
        error_msg="can not compare variables in different type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,"bool");

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,op));

    // 
    result_node.bool_str=ir.get_node_name(operand1)+" "+op+" "+ir.get_node_name(operand2);// for selection statement

    return result_node;
}

var_node ir_gen::analyze_relational_expression(const std::shared_ptr<AST> & relational_exp)
{
    if(relational_exp->left_child->name=="shift_expression")
    {
        std::shared_ptr<AST> shift_exp=relational_exp->left_child;
        return analyze_shift_expression(shift_exp);
    }
    std::string op=relational_exp->left_child->right_child->name;
    op=(op=="LE_OP")?"<=":op;
    op=(op=="GE_OP")?">=":op;

    var_node operand1=analyze_relational_expression(relational_exp->left_child);
    var_node operand2=analyze_shift_expression(relational_exp->left_child->right_child->right_child);

    if(operand1.type!=operand2.type)
        error_msg="can not compare variables in different type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,"bool");//  the result of comparison

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,op));

    // 
    result_node.bool_str=ir.get_node_name(operand1)+" "+op+" "+ir.get_node_name(operand2);// for selection statement

    return result_node;

}

var_node ir_gen::analyze_shift_expression(const std::shared_ptr<AST> & shift_exp)
{
    if(shift_exp->left_child->name=="additive_expression")
    {
        std::shared_ptr<AST>  additive_exp=shift_exp->left_child;
        return analyze_additive_expression(additive_exp);
    }
    std::string op;
    if(shift_exp->left_child->right_child->name=="LEFT_OP")
        op=="<<";
    else op==">>";
    var_node operand1=analyze_shift_expression(shift_exp->left_child);
    var_node operand2=analyze_additive_expression(shift_exp->left_child->right_child->right_child);

    if(operand1.type!="int"||operand2.type!="int")
        error_msg="can only exexute shift calcucation in int type.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,op));

    return result_node;


}

var_node ir_gen::analyze_additive_expression(const std::shared_ptr<AST> & additive_exp)
{
    if(additive_exp->left_child->name=="multiplicative_expression")
    {
        std::shared_ptr<AST> multiplicative_exp=additive_exp->left_child;
        return analyze_multiplicative_expression(multiplicative_exp);
    }
    var_node operand1=analyze_additive_expression(additive_exp->left_child);
    var_node operand2=analyze_multiplicative_expression(additive_exp->left_child->right_child->right_child);

    if(operand1.type!=operand2.type)
        error_msg="can not exexute calcucation in between different operand.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,additive_exp->left_child->right_child->name));

    return result_node;

}

var_node ir_gen::analyze_multiplicative_expression(const std::shared_ptr<AST> & multiplicative_exp)
{
    if(multiplicative_exp->left_child->name=="unary_expression")
    {
        std::shared_ptr<AST> unary_exp=multiplicative_exp->left_child;
        return analyze_unary_expression(unary_exp);
    }
    var_node operand1=analyze_multiplicative_expression(multiplicative_exp->left_child);
    var_node operand2=analyze_unary_expression(multiplicative_exp->left_child->right_child->right_child);

    if(operand1.type!=operand2.type)
        error_msg="can not exexute calcucation in between different operand.\n";
    
    std::string result_temp_name="temp"+std::to_string(ir.num_temp++);
    var_node result_node=this->create_temp_var(result_temp_name,operand1.type);

    block_stack.back().var_map.insert(std::make_pair(result_temp_name,result_node));
    ir.add_ir(ir.gen_binary_operation_ir(result_temp_name,operand1,operand2,multiplicative_exp->left_child->right_child->name));

    return result_node;
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
            ir.add_ir(return_temp_name+" :="+temp_name+" ^ "+ir.get_node_name(rnode));//same for exclusive or
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

            ir.add_ir(temp_name+" :=&"+ir.gen_array_name(new_array_node)+" + "+ir.get_node_name(length_node));
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

bool ir_gen::lookup_current_var(std::string var_name) // only find var in current block, used for checking multiple definition in the same block
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