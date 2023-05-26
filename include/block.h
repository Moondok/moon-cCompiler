#ifndef __BLOCK_H__
#define __BLOCK_H__
#include <string>
#include <vector>
#include <map>
#include <fstream>

class var_node
{
public:
    bool is_addr; // a pointer
    bool is_constant_int=true;  //special setting for array's initialization
    int real_value; //same as above ,special setting for array's declaration

    std::string name;
    std::string type;
    int id=-1;
    std::string bool_str="";
    var_node(std::string n,std::string t,unsigned int i=-1,bool is_arr=false):name(n),type(t),id(i),is_addr(is_arr){};
    var_node():name(""),type(""),id(-1){};

    bool operator<(const var_node a) const
    {
        return this->id<a.id;
    }
};

class func_node
{
public:
    bool is_defined=false;
    std::string name;
    std::string rtype;
    std::vector<var_node> para_list;

};

class array_node
{
public:
    std::string name;
    std::string type;
    unsigned int id;
    unsigned int num; //capacity
    array_node(){};
    array_node(std::string n,std::string t,unsigned int i=-1,unsigned int nu=-1):name(n),type(t),id(i),num(nu){};

    bool operator<(const array_node a)const
    {
        return this->id<a.id;
    }
};

class Block
{
public:
    func_node func;
    bool is_func=false;
    std::map<std::string,var_node > var_map;

    std::map<var_node, int> var2memory_location;// map the user-defined variables to the memory locations 5-26
    std::map<array_node, int> arr2memory_location; // map the user-defined arrays to the memory locations 5-26


    std::map<std::string,array_node> arr_map;
    std::string break_label;
    std::string start_label;
    int id;  // the unique signature of a block
    int block_size=0;  // the capacity of memory we need.
    bool can_break=false;
    Block(int i):id(i){};

    void dump_block(std::fstream & ofile)
    {
        ofile<<"BLOCK "<<this->id<<'\n';
        ofile<<"VAR\n";
        for(auto & var : var2memory_location)
        {
            //ofile<<1;


            // remember! the variable we registered in the table are all user-defined variables, not temp, not temp!
            // thus we can find the real name from the format like 'varx'
            ofile<<var.first.id<<" "<<var.first.type<<" "<<var.second<<" \n";
        }
        ofile<<"ARR\n";
        for(auto & arr:arr2memory_location)
        {
            ofile<<arr.first.id<<" "<<arr.first.type<<" "<<arr.first.num<<" "<<arr.second<<" \n";// note that the first.num is the capacity of array
        }
    }

};


#endif