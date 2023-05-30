#include <lexer.h>
#include <parser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <util.h>
#include <ir_gen.h>
#include <target_gen.h>

int main(int argc,char* argv[])
{

    std::cout<<"moon-cC version 1.0 (Ubuntu-20.04.01)   Copyright 2023  Kaixu Chen\n";
    // std::cout<<"the num of params: "<<argc<<"\n";
    // std::cout<<argv[1]<<'\n';

    bool is_dump_tree=false; //-d
    bool is_mips=false;  //-m 
    bool is_llvm=false;  //-l
    bool is_legal=false;// only when this parameter is true, we start to analyze

    if(argc==4)
    {
        //std::cout<<argv[2]<<'\n';
        if(argv[2][0]=='-'&&argv[2][1]=='m')
        {
            //std::cout<<"dump-tree required.\n";
            is_mips=true;
            if(argv[3][0]=='-'&&argv[3][1]=='d')
            {
                is_dump_tree=true;
                is_legal=true;
            }
            else
                is_legal=false;
        }
        else if(argv[2][0]=='-'&&argv[2][1]=='l')
        {
            is_llvm=true;
            if(argv[3][0]=='-'&&argv[3][1]=='d')
            {
                is_dump_tree=true;
                is_legal=true;
            }
            else
                is_legal=false;
        }
    }
    else if(argc==3)
    {
        if(argv[2][0]=='-'&&argv[2][1]=='m')
        {
            is_mips=true;
            is_legal=true;
        }
        else if(argv[2][0]=='-'&&argv[2][1]=='l')
        {
            is_llvm=true;
            is_legal=true;
        }
    }
    
    if(!is_legal)
    {
        std::cout<<"the parameters you input is illegal , the usage is as follow:\n";
        print_rule();
        return 0;
    }
    else
    {
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

        //lexer
        if(!std::get<0>(re))
        {
            std::cout<<"mooc-cC lexer : some errors occur, see details in 'errors.log' .\n";
            std::ofstream o("errors.log");
            if(o.is_open()==false)
            {
                std::cout<<"can not open the file.\n";
            }
            else
            {
                o<<"LEXICAL ERRORS\n";
                o<<"some error occurs at location line "<<std::get<2>(re)<<" column "<<std::get<3>(re)<<" : "<<std::get<1>(re);
                o.close();
            }
        }
        else
        {
            std::cout<<"moon-cC lexer : lexical analysis done.             ++++++##\n";

            Parser parser;
            parser.read_grammer_Yacc("./src/Ansi_Yacc_C99.txt");
            parser.read_cache();

            std::shared_ptr<AST> root=nullptr;

            auto r=parser.check("./token_result.tmp",root);

            //syntax analysis fails
            if(std::get<0>(r)==false)
            {
                std::cout<<"mooc-cC parser : some errors occur, see details in 'errors.log' .\n";
                std::ofstream o("errors.log");
                if(o.is_open()==false)
                {
                    std::cout<<"can not open the file.\n";
                }
                else
                {
                    o<<"SYNTAX ERRORS\n";
                    o<<"some error occurs at location line "<<std::get<2>(r)<<" column "<<std::get<3>(r)<<" : "<<std::get<1>(r);
                    o.close();
                }
                    
            }
            //syntax analysis success
            else
            {
                std::cout<<"moon-cC parser : syntax analysis done.             ++++####\n";
                if(is_dump_tree) //dump the tree 
                {
                    std::ofstream o("tree.json");
                    print_tree(root,o,0);
                }
                ir_gen ir_generator;
                if(root!=nullptr)
                    ir_generator.analyze_tree(root);

                bool ir_gen_re=ir_generator.get_result();
                if(ir_gen_re)
                {
                    std::cout<<"moon-cC IR generator : semantic analysis done.     ++######\n";

                    //add the object code generation
                    {

                        target_gen target_generator("ir.mcc");
                        target_generator.print_parse_result();

                        target_generator.analyze_ir();
                        target_generator.output_target();
                        std::cout<<"moon-cC object code generator: object code done.   ########\n";
                    }
                }
                else   
                    std::cout<<"mooc-cC IR generator : some errors occur, see details in 'errors.log' .\n";
                
            }
        }
    }


    
    // parser.get_all_symbol_first();
    // parser.get_state_group_list();
    // parser.get_LR1_table();

    // parser.print_LR1_table();

    
    return 0;
}