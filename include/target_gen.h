#include<string>
#include <fstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <set>
#include <iostream>
#include <map>
#include <stack>
typedef std::tuple<int,std::string , std::string, std::string ,std::string, std::string ,std::string >
 parse_return;


class target_gen
{
private:
    struct var_info
    {
        int is_arr=0;
        int length=0; // specially for array
        int offset=0;
        int id=-1; 
        //note that the array and variable have different id systems 
        var_info(int is_,int l,int o,int i):is_arr(is_),length(l),offset(o),id(i){};
        var_info(){};
    };
    
    std::string file_name;
    std::vector<std::string> mips_list;
    std::set<int> basic_block_entries;

    parse_return parse(std::string ir);

    // a map which map the block to a vector , the content in the vector is the struct info which contanins variable and offsets
    std::vector<std::vector<var_info>> block2vars;

    // a map which maps the block to its size;
    std::vector<int> block2size;

    // a stack which records the nesting relationship of blocks
    std::stack<int> block_stack;

    // a map which maps the vars to register (may be not exists)
    std::map<std::string , int> var2reg;

    int num_block=0; //record the nesting relationships

    std::vector<std::string> target_code_list;

    void get_block_entries();
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

public:

    target_gen(const std::string &f_n):file_name(f_n)
    {
        parse_sym_tbl();
    };

    target_gen():file_name("ir.mcc")
    {
        parse_sym_tbl();
    };

    //two tool function
    void print_parse_result()
    {
        std::ifstream infile(this->file_name);
        std::string ir;

        // std::ofstream ofile("test.txt");
        // char x[40];
        // while(getline(infile,ir))
        // {
        //     auto  r=parse(ir);
        //     ofile<<std::get<0>(r)<<" ";
        //     int cnt=1;

        //     if(std::get<1>(r)!="")
        //         ofile<<" | "<<std::get<1>(r)<<" ";
        //     if(std::get<2>(r)!="")
        //         ofile<<" | "<<std::get<2>(r)<<" ";
        //     if(std::get<3>(r)!="")
        //         ofile<<" | "<<std::get<3>(r)<<" ";
        //     if(std::get<4>(r)!="")
        //         ofile<<" | "<<std::get<4>(r)<<" ";
        //     if(std::get<5>(r)!="")
        //         ofile<<" | "<<std::get<5>(r)<<" ";
        //     if(std::get<6>(r)!="")
        //         ofile<<" | "<<std::get<6>(r)<<" ";
            

        //     ofile<<"\n";
            
        // }
        infile.close();
        //ofile.close();

        get_block_entries();
    }

    int get_register();

    void parse_sym_tbl();

    void output_target();

    void analyze_ir();
public:

    
    
};