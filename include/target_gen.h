#include<string>
#include <fstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <set>
#include <iostream>
#include <map>
#include <stack>
#include <set>
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
    std::vector<int> block_stack;

    // a map which maps the vars to register (may be not exists)
    std::map<std::string , int> var2reg;

    // a map which maps the register to vars it restores
    std::vector<std::set<std::string>> reg2vars=std::vector<std::set<std::string>>(40);//first 16 means the number register (t0-t7 s0-s7),second 16 for float f1-f16,then 4 a0-a3, for int param, f17-f20 for double param


    int num_block=0; //record the nesting relationships

    std::vector<std::string> target_code_list;

    void get_block_entries();
    //type 
    //0:3 FUNCTION funcname :   ok        done
    //1:2 param name     ok               done
    //2:5 x := x1 op x2                   done
    //3:2 RETURN x     ok                 done
    //4:3 ARRAY array_name length   ok   // no need to gen target code ( already in syb tbl)
    //5:3 x := #y   ok                    done
    //6:3 x := y    ok                    done
    //7:2 arg x     ok                    done
    //8:4 := call func    ok               done
    //9:3 Label labelname :    ok          done
    //10:6 if x1 op x2 goto label          done
    //11:2 GOTO label              ok      done
    //12:5 x := &arr_name + index  ok    pending
    //13:2 BEGIN LOOP                      done
    //14:2 END LOOP                        done
    //15:2 BEGIN IF                        done
    //16:2 END IF                          done
    //17:2 BEGIN ELSE                      done
    //18:2 END ELSE                        done
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

    int get_register(std::string);

    int get_register4param(std::string);

    int get_index(std::string var_name);

    std::string reg_index2name(int index);

    void refresh_register();

    void parse_sym_tbl();

    void output_target();

    void analyze_ir();
public:

    
    
};