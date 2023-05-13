#ifndef __BLOCK_H__
#define __BLOCK_H__
#include <string>
#include <vector>
#include <map>


class var_node
{
public:
    std::string name;
    std::string type;
    unsigned int id;
    var_node(std::string n,std::string t,unsigned int i=-1):name(n),type(t),id(i){};
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
    unsigned int num;
};

class Block
{
public:
    func_node func;
    bool is_func=false;
    std::map<std::string,var_node > var_map;
    std::map<std::string,array_node> arr_map;

    bool can_break=false;


};


#endif