#ifndef __BLOCK_H__
#define __BLOCK_H__
#include <string>
#include <vector>
#include <map>


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
};

class Block
{
public:
    func_node func;
    bool is_func=false;
    std::map<std::string,var_node > var_map;
    std::map<std::string,array_node> arr_map;
    std::string break_label;
    std::string start_label;

    bool can_break=false;


};


#endif