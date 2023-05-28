#include <target_gen.h>

parse_return target_gen::parse(std::string ir)
{
    parse_return return_value;
    int type=0;
    std::string re1="";
    std::string re2="";
    std::string re3="";
    std::string re4="";
    std::string re5="";
    std::string re6="";

    unsigned int start=0,end=0;
    int cnt=0;
    end=ir.find(" ",start);
    while(end!=-1)
    {
        std::string re=ir.substr(start,end-start);
        switch(cnt)
        {
            case 0:
            {
                re1=std::get<1>(return_value)=re;
                break;
            }
            case 1:
            {
                re2=std::get<2>(return_value)=re;
                break;
            }
            case 2:
            {
                re3=std::get<3>(return_value)=re;
                break;
            }
            case 3:
            {
                re4=std::get<4>(return_value)=re;
                break;
            }
            case 4:
            {
                re5=std::get<5>(return_value)=re;
                break;
            }
            case 5:
            {
                re6=std::get<6>(return_value)=re;
                break;
            }

        }
        cnt++;
        start=end+1;
        if(start>=ir.size())
            break;
        end=ir.find(" ",start);
    }

    // add the last token
    if(cnt==1)
    {
        re2=std::get<2>(return_value)=ir.substr(start,ir.size()+1-start);
        cnt++;
    }
    else if(cnt==2)
    {
        re3=std::get<3>(return_value)=ir.substr(start,ir.size()+1-start);
        cnt++;
    }
    else if(cnt==3)
    {
        re4=std::get<4>(return_value)=ir.substr(start,ir.size()+1-start);
        cnt++;
    }
    else if(cnt==4)
    {
        re5=std::get<5>(return_value)=ir.substr(start,ir.size()+1-start);
        cnt++;
    }
    else if(cnt==5)
    {
        re6=std::get<6>(return_value)=ir.substr(start,ir.size()+1-start);
        cnt++;
    }




    if(cnt==3) // 3 components
    {
        if(re1=="FUNCTION")
            std::get<0>(return_value)=0;
        else if(re1=="ARRAY")
            std::get<0>(return_value)=4;
        else if(re1=="LABEL")
            std::get<0>(return_value)=9;
        else if(re3[0]=='#')
            std::get<0>(return_value)=5;
        else 
            std::get<0>(return_value)=6;

    }
    else if(cnt==2)
    {
        if(re1=="param")
            std::get<0>(return_value)=1;
        else if(re1=="RETURN")
            std::get<0>(return_value)=3;
        else if(re1=="arg")
            std::get<0>(return_value)=7;
        else if(re1=="GOTO")
            std::get<0>(return_value)=11;
        else if(re1=="BEGIN"&&re2=="LOOP")
            std::get<0>(return_value)=13;
        else if(re1=="END"&&re2=="LOOP")
            std::get<0>(return_value)=14;
        else if(re1=="BEGIN"&&re2=="IF")
            std::get<0>(return_value)=15;
        else if(re1=="END"&&re2=="IF")
            std::get<0>(return_value)=16;
        else if(re1=="BEGIN"&&re2=="ELSE")
            std::get<0>(return_value)=17;
        else if(re1=="END"&&re2=="ELSE")
            std::get<0>(return_value)=18;
        
        


    }
    else if(cnt==4)
        std::get<0>(return_value)=8;
    else if(cnt==5)
    {
        if(re3[0]=='&')// array 
            std::get<0>(return_value)=12;
        else
            std::get<0>(return_value)=2;
    }
    else if(cnt==6)
        std::get<0>(return_value)=10;

    return return_value;

    

}
//type 
//0:3 FUNCTION funcname :   ok
//1:2 param name     ok
//2:5 x := x1 op x2
//3:2 RETURN x     ok
//4:3 ARRAY array_name length   ok
//5:3 x := #y   ok
//6:3 x := y    ok
//7:2 arg x     ok
//8:4 := call func    ok
//9:3 Label labelname :    ok
//10:6 if x1 op x2 goto label
//11:2 GOTO label              ok
//12:5 x := &arr_name + index  ok
//13:2 BEGIN LOOP
//14:2 END LOOP
//15:2 BEGIN IF
//16:2 END IF
//17:2 BEGIN ELSE
//18:2 END ELSE
// total 19 types of IR

void target_gen::get_block_entries()
{
    std::ifstream infile(this->file_name);
    if(!infile.is_open())
    {
        std::cerr<<"can not file object code file\n";
        return;
    }
    std::string buff;
    int cnt=0;
    while(getline(infile,buff))
    {
        if(std::get<0>(parse(buff))==0||std::get<0>(parse(buff))==9) // function definition or label
        {
            this->basic_block_entries.insert(cnt);

        }
        else if(std::get<0>(parse(buff))==11)  //goto
        {
            this->basic_block_entries.insert(cnt+1);
            //this->basic_block_entries.insert(cnt+1);
        }
        cnt++;
            
    }
    infile.close();

}

void target_gen:: parse_sym_tbl()
{
    std::fstream tbl_file("block_table_cache1",std::ios::in);
    if(tbl_file.is_open()==false)
    {
        std::cerr<<"can not load symbol table.\n";
        return ;
    }
    std::string tmp;
    while(getline(tbl_file,tmp))
    {
        int start=0;
        int end=tmp.find(" ");
        std::string token=tmp.substr(start,end-start);
        if(token=="BLOCK") // a new block
        {
            start=end+1;
            end=tmp.find(" ",start);
            int block_id=stoi(tmp.substr(start,end-start));

            start=end+1;
            end=tmp.find(" ",start);
            int block_size=stoi(tmp.substr(start,end-start));

            while(block_id>=block2vars.size())
            {
                block2vars.emplace_back(std::vector<var_info>());
            }
            while(block_id>=block2size.size())
                block2size.emplace_back(0);

            block2size.at(block_id)=block_size;

            bool read_var=true;
            while(true)
            {
                getline(tbl_file,tmp);
                start=0;
                end=tmp.find(" ",start);
                std::string token=tmp.substr(start,end-start);
                if(token=="END")
                    break; // end a block
                else
                {
                    if(token=="VAR")
                        continue;
                    else if(token=="ARR")
                    {
                        read_var=false;
                        continue;
                    }
                    else // a var of an array
                    {
                        int id=stoi(token);
                        start=end+1;
                        end=tmp.find(" ",start); //cuz type can be read directly from ir , we do not record here.
                        start=end+1;
                        end=tmp.find(" ",start);
                        if(read_var) //
                        {
                            int offset=stoi(tmp.substr(start,end-start));
                            block2vars.at(block_id).emplace_back(var_info(0,0,offset,id));

                        }
                        else
                        {
                            int length=stoi(tmp.substr(start,end-start));
                            start=end+1;
                            end=tmp.find(" ",start);
                            int offset=stoi(tmp.substr(start,end-start));
                            block2vars.at(block_id).emplace_back(var_info(1,length,offset,id));

                        }

                    }
                }
            }


        }


        else if(token=="NUM")
        {
            start=end+1;
            end=tmp.find(" ",start);
            int num_block=stoi(tmp.substr(start,end-start));
            break;
        }
    }
    
    tbl_file.close();

}

int target_gen::get_register(std::string var_name)
{
    // return value is between 0-31, on behalf on t0-t7 s0-s7 f1-f16(for float)
    int reg_id;
    if(var_name.at(var_name.size()-1)=='t')
    {
        for(int i=0;i<16;i++)
            if(reg2vars.at(i).empty()==true)
            {
                reg_id=i;
                break;

            }
    } //type int
        
    else  //double
        for(int i=16;i<32;i++)
            if(reg2vars.at(i).empty()==true)
            {
                reg_id=i;
                break;

            }

    return -1;
}

void target_gen::analyze_ir()
{
    std::fstream ir_file(this->file_name,std::ios::in);
    if(ir_file.is_open()==false)
    {
        std::cerr<<"can not open ir file.\n";
        return ;
    }

    std::string ir;
    while(getline(ir_file,ir))
    {
        auto r=this->parse(ir);

        if(std::get<0>(r)==0) //function call
        {
            //notice that the params passed to current function are prepared, so there is no need to allocate memory for them
            block_stack.emplace_back(this->num_block++);
            //restore the 
            target_code_list.emplace_back(std::get<1>(r)+" :");
            target_code_list.emplace_back("addi $sp $sp -8");
            target_code_list.emplace_back("sw $ra 8($sp)");
            target_code_list.emplace_back("sw $fp 4($sp)");
            target_code_list.emplace_back("move $fp $sp");// update the frame pointer

            // allocate space for the variables defined in this function
            int id=this->num_block-1;
            int size = block2size.at(id);
            target_code_list.emplace_back("addi $sp $sp -"+std::to_string(size)); //allocate memory for variables and arrays


        }

        else if(std::get<0>(r)==1) //param
        {
            std::string var_name=std::get<2>(r);

            int reg_id=0;
            if(var_name[var_name.size()-1]=='t')//int ,find in 
            {
                for(int i=32;i<36;i++)
                    if(reg2vars.at(i).empty()==false)
                    {
                        reg_id=i;
                        break;
                    }
                std::string reg_name=reg_index2name(reg_id);

                int offset=get_index(var_name);
                target_code_list.emplace_back("sw "+reg_name+" "+std::to_string(offset)+"($sp)");

                reg2vars.at(reg_id).clear();
            }
            else
            {
                for(int i=36;i<40;i++) // in f17-f20
                    if(reg2vars.at(i).empty()==false)
                    {
                        reg_id=i;
                        break;
                    }
                std::string reg_name=reg_index2name(reg_id);

                int offset=get_index(var_name);
                target_code_list.emplace_back("s.s "+reg_name+" "+std::to_string(offset)+"($sp)");

                reg2vars.at(reg_id).clear(); // the register is no longer occupied

            }


        }
        
        else if(std::get<0>(r)==2) //x := x1 op x2
        {
            std::string dst=std::get<1>(r);
            std::string op=std::get<4>(r);
            std::string a=std::get<3>(r);
            std::string b=std::get<5>(r);

            std::string left_reg_name;
            std::string right_reg_name;
            std::string target_reg_name;
            int target_reg_id=0,left_reg_id=0,right_reg_id;
            
            if(a[0]=='t')// left operand is a tmp
            {
                left_reg_id=var2reg[a];
                left_reg_name=reg_index2name(left_reg_id);

                auto ite=var2reg.find(a);
                var2reg.erase(ite);
            }
            else  // user-defined
            {
                left_reg_id=get_register(a); 
                left_reg_name=reg_index2name(left_reg_id);

                int offset=get_index(a);
                reg2vars.at(left_reg_id).insert(a); // this can not be deleted for we will allocate reg later
                if(a[a.size()-1]=='t')//int
                    target_code_list.emplace_back("lw "+left_reg_name+" "+std::to_string(offset)+"($sp)");
                else
                    target_code_list.emplace_back("l.s "+left_reg_name+" "+std::to_string(offset)+"($sp)");

            }

            if(b[0]=='t')// right operand is a tmp
            {
                right_reg_id=var2reg[b];
                right_reg_name=reg_index2name(left_reg_id);

                auto ite=var2reg.find(b);
                var2reg.erase(ite);
            }
            else  // user-defined
            {
                right_reg_id=get_register(b); 

                int offset=get_index(b);
                reg2vars.at(right_reg_id).insert(a);
                if(b[b.size()-1]=='t')//int
                    target_code_list.emplace_back("lw "+right_reg_name+" "+std::to_string(offset)+"($sp)");
                else
                    target_code_list.emplace_back("l.s "+right_reg_name+" "+std::to_string(offset)+"($sp)");
            }
            

            if(dst[0]=='t')//tmp, we need a register
            {
                
                if(var2reg.find(dst)!=var2reg.end())// already in register( in my setting, this situation is impossible
                    target_reg_id=var2reg[dst];
                else
                {
                    target_reg_id=get_register(dst);
                    var2reg.insert(std::make_pair(dst,target_reg_id));
                    reg2vars.at(target_reg_id).insert(dst);
                }
            }
            else // user defined
            {
                target_reg_id=get_register(dst); // no need to register
            }
            target_reg_name=reg_index2name(target_reg_id);

            if(op=="+")
            {   
                if(dst[dst.size()-1]=='t')
                    target_code_list.emplace_back("add "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
                else
                    target_code_list.emplace_back("add.s "+target_reg_name+" "+left_reg_name+" "+right_reg_name);

            }
            else if(op=="-")
            {
                if(dst[dst.size()-1]=='t')
                    target_code_list.emplace_back("sub "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
                else
                    target_code_list.emplace_back("sub.s "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
            }
            else if(op=="*")
            {
                if(dst[dst.size()-1]=='t')
                    target_code_list.emplace_back("mul "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
                else
                    target_code_list.emplace_back("mul.s "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
            }
            else if(op=="/")
            {
                if(dst[dst.size()-1]=='t')
                    target_code_list.emplace_back("div "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
                else
                    target_code_list.emplace_back("div.s "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
            }
            else if(op=="<")
                target_code_list.emplace_back("slt "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
            
            else if(op==">")
                target_code_list.emplace_back("slt "+target_reg_name+" "+right_reg_name+" "+left_reg_name);

            else if(op==">>")
                target_code_list.emplace_back("sra "+target_reg_name+" "+right_reg_name+" "+left_reg_name);
            else if(op=="<<")
                target_code_list.emplace_back("sll "+target_reg_name+" "+right_reg_name+" "+left_reg_name);
            
            //if user-defined ,write in memory
            if(dst[0]=='v')
            {
                int offset=get_index(dst);
                if(dst[dst.size()-1]=='t')
                    target_code_list.emplace_back("lw "+target_reg_name+" "+std::to_string(offset)+"($sp)");
                else
                    target_code_list.emplace_back("l.s "+target_reg_name+" "+std::to_string(offset)+"($sp)");

            }
            
            // clear the occupance of reg a and reg b,no matter tmp or user-defined
            reg2vars.at(left_reg_id).clear();
            reg2vars.at(right_reg_id).clear();

        }

        else if(std::get<0>(r)==3)  //return x
        {
            if(var2reg.find(std::get<2>(r))!=var2reg.end())
             // if the return value already in register, no need to visit memory
            {
                //$f0 and $v0 are specially for return values
                if(std::get<2>(r)[std::get<2>(r).size()-1]=='e')//float
                {
                    target_code_list.emplace_back("mov.s $f0 "+reg_index2name(var2reg[std::get<2>(r)]));
                }
                else
                    target_code_list.emplace_back("move $v0 "+reg_index2name(var2reg[std::get<2>(r)]));
            }
            else  // just in the memory
            {
                int offset=get_index(std::get<2>(r));
                if(std::get<2>(r)[std::get<2>(r).size()-1]=='e')//float
                {
                    target_code_list.emplace_back("l.s $f0 "+std::to_string(offset)+"(sp)"); // return a[0] may not works(sad)
                }
                else
                    target_code_list.emplace_back("lw $v0 "+std::to_string(offset)+"(sp)");
            }

            // restore the scene, when this function is invoked.
            int block_id=block_stack.back();
            int size=block2size[block_id];

            target_code_list.emplace_back("addi $sp $sp "+std::to_string(size));
            //fp and ra!
            target_code_list.emplace_back("lw $fp 4($sp)");
            target_code_list.emplace_back("lw $ra 4($sp)");
            // free the memory occupied by ra and fp
            target_code_list.emplace_back("addi $sp 8");

        }

        else if(std::get<0>(r)==5)//x := #y
        {
            std::string imm_num=std::get<2>(r).substr(1);
            std::string var_name=std::get<1>(r);

            int reg_id=get_register(var_name); 
            std::string reg_name=reg_index2name (reg_id);

            if(var_name[0]=='t')// only register register relationship for temp!
            {
                var2reg.insert(std::make_pair(var_name,reg_id));
                reg2vars.at(reg_id).insert(var_name);
            }

            if(var_name[var_name.size()-1]=='t')//int
                target_code_list.emplace_back("li "+reg_name+" "+imm_num);
            else
                target_code_list.emplace_back("li.s "+reg_name+" "+imm_num);
            
            if(var_name[0]=='v')// user-defined variable , do not need to load into reg
            {
                int offset=get_index(var_name);
                if(var_name[var_name.size()-1]=='t')//int
                {
                    target_code_list.emplace_back("sw "+reg_name+" "+std::to_string(offset)+"($sp)");
                }
                else 
                { 
                    target_code_list.emplace_back("s.s "+reg_name+" "+std::to_string(offset)+"($sp)");
                }

            }
        }

        else if(std::get<0>(r)==6) //x := y
        {
            std::string right_value=std::get<2>(r);
            std::string left_value=std::get<1>(r);
            if(var2reg.find(right_value)!=var2reg.end()) // we have it in register ,y is temp
            {
                int reg_id=var2reg[right_value];
                std::string reg_name=reg_index2name (reg_id);
                if(left_value[0]=='v')
                {
                    int offset=get_index(left_value);
                    if(left_value[left_value.size()-1]=='t')
                        target_code_list.emplace_back("sw "+reg_name+" "+std::to_string(offset)+"($sp)");
                    else 
                        target_code_list.emplace_back("s.s "+reg_name+" "+std::to_string(offset)+"($sp)");
                }
                else // x=tmp
                {
                    int left_reg_id=get_register(left_value);
                    std::string left_reg_name=reg_index2name(left_reg_id);
                    if(left_value[left_value.size()-1]=='t')
                        target_code_list.emplace_back("move "+left_reg_name+" "+reg_name);
                    else 
                        target_code_list.emplace_back("mov.s "+left_reg_name+" "+reg_name);

                    var2reg.insert(std::make_pair(left_value,left_reg_id));  //new temp ,register
                    reg2vars.at(left_reg_id).insert(left_value);


                }

                // now y is used ,delete its info!
                auto ite=var2reg.find(right_value);
                var2reg.erase(ite);

                reg2vars.at(reg_id).clear();

            }
            else //y is a user-defined var
            {
                int reg_id=get_register(right_value);

                reg2vars.at(reg_id).insert(right_value);  // cuz we will allocate register later, we make a mark here
                std::string reg_name=reg_index2name (reg_id);
                int offset=get_index(right_value);  //offset of y

                // load y into register first
                if(right_value[right_value.size()-1]=='t')//int
                    target_code_list.emplace_back("lw "+reg_name+" "+std::to_string(offset)+"(sp)");
                else
                    target_code_list.emplace_back("l.s "+reg_name+" "+std::to_string(offset)+"(sp)");

                    

                if(left_value[0]=='v')
                {
                    int offset=get_index(left_value);
                    if(left_value[left_value.size()-1]=='t')
                        target_code_list.emplace_back("sw "+reg_name+" "+std::to_string(offset)+"(sp)");
                    else 
                        target_code_list.emplace_back("s.s "+reg_name+" "+std::to_string(offset)+"(sp)");
                }
                else // x=tmp
                {
                    int left_reg_id=get_register(left_value);
                    std::string left_reg_name=reg_index2name(left_reg_id);
                    if(left_value[left_value.size()-1]=='t')
                        target_code_list.emplace_back("move "+left_reg_name+" "+reg_name);
                    else 
                        target_code_list.emplace_back("mov.s "+left_reg_name+" "+reg_name);

                    var2reg.insert(std::make_pair(left_value,left_reg_id));  //new temp ,register
                    reg2vars.at(left_reg_id).insert(left_value);
                }

                reg2vars.at(reg_id).clear(); //delete the occupance
            }

            
        }

        else if(std::get<0>(r)==7)  //arg
        {
            std::string var_name=std::get<2>(r);
            if(var_name[0]=='t')//tmp ,  find it in the register
            {
                int reg_id=var2reg[var_name]; 
                std::string reg_name=reg_index2name(reg_id);
                int arg_reg_id=get_register4param(var_name);
                std::string arg_reg_name=reg_index2name(arg_reg_id);

                if(var_name[var_name.size()-1]=='t')
                    target_code_list.emplace_back("move "+arg_reg_name+" "+reg_name);
                else
                    target_code_list.emplace_back("mov.s "+arg_reg_name+" "+reg_name);

                //var2reg.insert(std::make_pair(var_name,arg_reg_id));  can not do that ,cuz the key already has value reg_name
                reg2vars.at(arg_reg_id).insert(var_name);

            }
            else// user-defined ,find it in stack
            {
                int offset=get_index(var_name);

                int arg_reg_id=get_register4param(var_name);
                std::string arg_reg_name=reg_index2name(arg_reg_id);

                if(var_name[var_name.size()-1]=='t')
                    target_code_list.emplace_back("lw "+arg_reg_name+" "+std::to_string(offset)+"($sp)");
                else
                    target_code_list.emplace_back("l.s "+arg_reg_name+" "+std::to_string(offset)+"($sp)");
                
                reg2vars.at(arg_reg_id).insert(var_name);
            }
        }

        else if(std::get<0>(r)==8) //x=call func
        {
            target_code_list.emplace_back("addi $sp $sp -8");
            target_code_list.emplace_back("sw $ra 8($sp)");
            target_code_list.emplace_back("sw $fp 4($sp)");

            std::string funname=std::get<4>(r);
            target_code_list.emplace_back("jal "+funname);

            target_code_list.emplace_back("lw $fp 4($sp)");
            target_code_list.emplace_back("lw $ra 4($sp)");
            // free the memory occupied by ra and fp
            target_code_list.emplace_back("addi $sp 8");

            // here the return value is already in f0 or v0
            std::string left_var=std::get<1>(r);
            if(left_var[0]=='t') //temp
            {
                int left_reg_id=get_register(left_var);
                std::string left_reg_name=reg_index2name(left_reg_id);

                var2reg.insert(std::make_pair(left_var,left_reg_id));  //new temp ,register
                reg2vars.at(left_reg_id).insert(left_var);

                if(left_var[left_var.size()-1]=='t')//int
                    target_code_list.emplace_back("move "+left_reg_name+" $v0");
                else
                    target_code_list.emplace_back("mov.s "+left_reg_name+" $f0");

            }
            else //user-defined
            {
                int offset=get_index(left_var);
                if(left_var[left_var.size()-1]=='t')//int
                    target_code_list.emplace_back("sw $v0 "+std::to_string(offset)+"($sp)");
                else
                    target_code_list.emplace_back("s.s $f0 "+std::to_string(offset)+"($sp)");
            }


        }

        else if(std::get<0>(r)==9)  
        {
            std::string labelname=std::get<1>(r);
            target_code_list.emplace_back(labelname+" :");
        }

        else if(std::get<0>(r)==10) // if x1 op x2 goto label
        {
            // we just support int type for now, support for float is on the way
            std::string a=std::get<2>(r);
            std::string b=std::get<4>(r);
            std::string op=std::get<3>(r);
            std::string label_name=std::get<6>(r);

            std::string left_reg_name;
            std::string right_reg_name;
            std::string target_reg_name;
            int target_reg_id=0,left_reg_id=0,right_reg_id;
            
            if(a[0]=='t')// left operand is a tmp
            {
                left_reg_id=var2reg[a];
                left_reg_name=reg_index2name(left_reg_id);

                auto ite=var2reg.find(a);
                var2reg.erase(ite);
            }
            else  // user-defined
            {
                left_reg_id=get_register(a); 
                left_reg_name=reg_index2name(left_reg_id);

                int offset=get_index(a);
                reg2vars.at(left_reg_id).insert(a); // this can not be deleted for we will allocate reg later
                target_code_list.emplace_back("lw "+left_reg_name+" "+std::to_string(offset)+"($sp)");
            }

            if(b[0]=='t')// right operand is a tmp
            {
                right_reg_id=var2reg[b];
                right_reg_name=reg_index2name(left_reg_id);

                auto ite=var2reg.find(b);
                var2reg.erase(ite);
            }
            else  // user-defined
            {
                right_reg_id=get_register(b); 

                int offset=get_index(b);
                reg2vars.at(right_reg_id).insert(a);
                target_code_list.emplace_back("lw "+right_reg_name+" "+std::to_string(offset)+"($sp)");
            }

            if(op=="==")
                target_code_list.emplace_back("beq "+left_reg_name+" "+right_reg_name+" "+label_name);
            else if(op=="!=")
                target_code_list.emplace_back("bne "+left_reg_name+" "+right_reg_name+" "+label_name);
            else if(op==">")
            {
                target_reg_id=get_register("tempx_int"); // tempx_int is just a placeholder for allocating register
                target_reg_name=reg_index2name(target_reg_id);
                target_code_list.emplace_back("slt "+target_reg_name+" "+right_reg_name+" "+left_reg_name);
                target_code_list.emplace_back("bne $zero"+right_reg_name+" "+label_name);
            }
            else if(op=="<")
            {
                target_reg_id=get_register("tempx_int"); // tempx_int is just a placeholder for allocating register
                target_reg_name=reg_index2name(target_reg_id);
                target_code_list.emplace_back("slt "+target_reg_name+" "+left_reg_name+" "+right_reg_name);
                target_code_list.emplace_back("bne $zero"+right_reg_name+" "+label_name);
            }


            reg2vars.at(left_reg_id).clear();
            reg2vars.at(right_reg_id).clear();
        }

        else if(std::get<0>(r)==11)//goto label
        {
            std::string labelname=std::get<2>(r);
            target_code_list.emplace_back("j "+labelname);
        }
        else if(std::get<0>(r)==13) //begin a loop!
        {
            block_stack.emplace_back(this->num_block++);
            int size=block2size.at(this->num_block-1);
            target_code_list.emplace_back("add $sp $sp -"+std::to_string(size));
        }
        else if(std::get<0>(r)==14)//end a loop
        {
            int id=block_stack.back();
            int size=block2size.at(id);
            target_code_list.emplace_back("add $sp $sp "+std::to_string(size));//recycle the memory
        }
        else if(std::get<0>(r)==15) //begin if!
        {
            block_stack.emplace_back(this->num_block++);
            int size=block2size.at(this->num_block-1);
            target_code_list.emplace_back("add $sp $sp -"+std::to_string(size));
        }
        else if(std::get<0>(r)==16)//end if
        {
            int id=block_stack.back();
            int size=block2size.at(id);
            target_code_list.emplace_back("add $sp $sp "+std::to_string(size));//recycle the memory
        }
        else if(std::get<0>(r)==17) //begin else!
        {
            block_stack.emplace_back(this->num_block++);
            int size=block2size.at(this->num_block-1);
            target_code_list.emplace_back("add $sp $sp -"+std::to_string(size));
        }
        else if(std::get<0>(r)==18)//end else
        {
            int id=block_stack.back();
            int size=block2size.at(id);
            target_code_list.emplace_back("add $sp $sp "+std::to_string(size));//recycle the memory
        }

    }

    ir_file.close();
    

}

void target_gen::output_target()
{
    std::fstream target_code_file("target.s",std::ios::out);
    if(target_code_file.is_open()==false)
    {
        std::cerr<<"can not open target code file.\n";
        return ;
    }

    for(auto &target_code:target_code_list)
        target_code_file<<target_code<<'\n';


    target_code_file.close();
}

int target_gen::get_index(std::string var_name)
{
    //note that the var_name can be temp or user-defined var , if var_name is user-defined , return value will be the 
    // positive offset from $sp, else ,the return value will be a register index;

    int i=0,id;
    if(var_name[var_name.size()-1]=='t')
    {
        while(var_name.at(var_name.size()-5-i)<'9'&&var_name.at(var_name.size()-5-i)>'0')
            i++;
        id=stoi(var_name.substr(var_name.size()-5-i,i));
    }
    else
    {
        while(var_name.at(var_name.size()-8-i)<'9'&&var_name.at(var_name.size()-8-i)>'0')
            i++;
        id=stoi(var_name.substr(var_name.size()-8-i,i));
    }

    if(var_name[0]=='v'||var_name[0]=='a')//var or array;
    {
        
        bool tag=false;
        int offset;
        for(int j=block_stack.size()-1;j>=0;j--)
        {
            for(auto & var:block2vars.at(block_stack.at(j)))// note thatblock_stack.at(j) is the id of a block
            {

                if(var.id==id&&((var.is_arr==0&&var_name[0]=='v')||(var.is_arr==1&&var_name[0]=='a'))) 
                {
                    offset+=block2size.at(block_stack.at(j))-var.offset;  // we trace the offset inversely!
                    tag=true;
                    break;
                }
            }

            if(tag)
                break;
            else
                offset+=block2size.at(block_stack.at(j));// more outer block( the id number is smaller)
        }
        return offset;
    }

    else if(var_name[0]=='t')//temp
    {
        int register_id;
        if(var2reg.find(var_name)==var2reg.end()) // not find
        {
            register_id=get_register(var_name);
            var2reg.insert(std::make_pair(var_name,register_id));
            reg2vars.at(register_id).insert(var_name);
        }
        else
            register_id=var2reg[var_name];
        return register_id;
    }

    
}

void target_gen:: refresh_register()
{

}

std::string target_gen::reg_index2name(int index)
{
    std::string re;
    if(index<16)
    {
        if(index<=7)
            re="$t"+std::to_string(index);
        else
            re="$s"+std::to_string(index-8);
    }
    else if(index<32)
        re="$f"+std::to_string(index-15);  //f1-f16
    else if(index<36) // register for parameters
        re="$a"+std::to_string(index-32);
    else 
        re="$f"+std::to_string(index-19);
    return re;
}

int target_gen::get_register4param(std::string param_name)
{
    if(param_name[param_name.size()-1]=='t')//int
    {
        //allocate a0-a3
        for(int i=32;i<36;i++)
            if(reg2vars.at(i).empty()==false)
            {
                return i;
                break;
            }
    }
    else
    {
        for(int i=36;i<40;i++)
            if(reg2vars.at(i).empty()==false)
            {
                return i;
                break;
            }
    }

    return -1;// no available register 4 params ,almost impossible
}