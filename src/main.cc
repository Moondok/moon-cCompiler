#include <lexer.h>
#include <parser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <util.h>

int main(int argc,char* argv[])
{
    std::cout<<"the num of params: "<<argc<<"\n";
    std::cout<<argv[1]<<'\n';

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

    std::cout<<std::get<0>(re)<<' '<<std::get<1>(re);

    Parser parser;
    parser.read_grammer_Yacc("./src/Ansi_Yacc_C99.txt");

    parser.read_cache();
    // parser.get_all_symbol_first();
    // parser.get_state_group_list();
    // parser.get_LR1_table();

    // parser.print_LR1_table();

    std::shared_ptr<AST> root=nullptr;

    auto r=parser.check("./token_result.tmp",root);
    std::ofstream o("tree.json");
    print_tree(root,o,0);

    std::cout<<std::get<0>(r)<<' '<<std::get<1>(r);
    std::cout<<std::get<2>(r)<<' '<<std::get<3>(r);

    return 0;
}