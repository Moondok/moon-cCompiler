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

int target_gen::get_register()
{
    // return value is between 0-15, on behalf on t0-t7 s0-s7
    return 0;
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

        else if(std::get<0>(r)==2)
        {
            
        }

        else if(std::get<0>(r)==3)
        {
            if(var2reg.find(std::get<2>(r))!=var2reg.end())
             // if the return value already in register, no need to visit memory
            {
                
            }


        }

        else if(std::get<0>(r)==9)
        {
            std::string labelname=std::get<1>(r);
            target_code_list.emplace_back(labelname+" :");
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
            register_id=get_register();
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