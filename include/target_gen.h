#include<string>
#include <fstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <set>
#include <iostream>
typedef std::tuple<int,std::string , std::string, std::string ,std::string, std::string ,std::string >
 parse_return;


class target_gen
{
private:
    std::string file_name;
    std::vector<std::string> mips_list;
    std::set<int> basic_block_entries;

    parse_return parse(std::string ir);

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
    // total 13 types of IR

public:

    target_gen(const std::string &f_n):file_name(f_n){};

    target_gen():file_name("ir.mcc"){};

    void print_parse_result()
    {
        std::ifstream infile(this->file_name);
        std::string ir;

        std::ofstream ofile("test.txt");
        char x[40];
        while(getline(infile,ir))
        {
            auto  r=parse(ir);
            ofile<<std::get<0>(r)<<" ";
            int cnt=1;

            if(std::get<1>(r)!="")
                ofile<<" | "<<std::get<1>(r)<<" ";
            if(std::get<2>(r)!="")
                ofile<<" | "<<std::get<2>(r)<<" ";
            if(std::get<3>(r)!="")
                ofile<<" | "<<std::get<3>(r)<<" ";
            if(std::get<4>(r)!="")
                ofile<<" | "<<std::get<4>(r)<<" ";
            if(std::get<5>(r)!="")
                ofile<<" | "<<std::get<5>(r)<<" ";
            if(std::get<6>(r)!="")
                ofile<<" | "<<std::get<6>(r)<<" ";
            

            ofile<<"\n";
            
        }
        infile.close();
        ofile.close();

        get_block_entries();
    }


public:

    
    
};