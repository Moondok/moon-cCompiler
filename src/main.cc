#include <lexer.h>
#include <parser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <util.h>
#include <ir_gen.h>

int main(int argc,char* argv[])
{

    std::cout<<"moon-cC version 1.0 (Ubuntu-20.04.01)   Copyright 2023  Kaixu Chen\n";
    // std::cout<<"the num of params: "<<argc<<"\n";
    // std::cout<<argv[1]<<'\n';

    bool is_dump_tree=false;

    if(argc==3)
    {
        //std::cout<<argv[2]<<'\n';
        if(argv[2][0]=='-'&&argv[2][1]=='d')
        {
            std::cout<<"dump-tree required.\n";
            is_dump_tree=true;
        }
    }


    



    LexicalAnalyzer lexer;


    std::string in_path(argv[1]);

    std::ifstream infile(in_path);

    std::stringstream in_stream;
    in_stream<<infile.rdbuf();
    infile.close();

    std::string in_code,out_code;

    in_code=in_stream.str();
    int pre_line = 1, pre_col = 0;
    auto re=lexer.analyze(in_code,out_code,pre_line,pre_col);

    //std::cout<<std::get<0>(re)<<' '<<std::get<1>(re)<<'\n';
    std::cout<<"moon-cC lexer : lexical analysis done.             ++++++##\n";


    Parser parser;
    parser.read_grammer_Yacc("./src/Ansi_Yacc_C99.txt");

    parser.read_cache();
    // parser.get_all_symbol_first();
    // parser.get_state_group_list();
    // parser.get_LR1_table();

    // parser.print_LR1_table();

    std::shared_ptr<AST> root=nullptr;

    auto r=parser.check("./token_result.tmp",root);
    //std::cout<<std::get<0>(r)<<' '<<std::get<1>(r);
    //std::cout<<std::get<2>(r)<<' '<<std::get<3>(r);
    std::cout<<"moon-cC parser : syntax analysis done.             ++++####\n";
    
    if(is_dump_tree)
    {
        std::ofstream o("tree.json");
        print_tree(root,o,0);
    }

    //std::cout<<std::get<0>(r)<<' '<<std::get<1>(r);
    //std::cout<<std::get<2>(r)<<' '<<std::get<3>(r);

    ir_gen ir_generator;
    if(root!=nullptr)
        ir_generator.analyze_tree(root);

    bool ir_gen_re=ir_generator.get_result();
    std::cout<<"moon-cC IR generator : syntax analysis done.       ++######\n";

    if(ir_gen_re)
        std::cout<<"ir success\n";
    else   
        std::cout<<"ir not success\n";


    std::cout<<"moon-cC object code generator: object code done.   ########\n";

    return 0;
}