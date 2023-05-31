#include <lexer.h>
#include <parser.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <util.h>
#include <ir_gen.h>
#include <target_gen.h>


#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>

//using namespace llvm;

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
            std::cout<<"mooc-CC lexer : some errors occur, see details in 'errors.log' .\n";
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
            std::cout<<"moon-CC lexer : lexical analysis done.             ++++++##\n";

            Parser parser;
            parser.read_grammer_Yacc("./src/Ansi_Yacc_C99.txt");
            parser.read_cache();

            std::shared_ptr<AST> root=nullptr;

            auto r=parser.check("./token_result.tmp",root);

            //syntax analysis fails
            if(std::get<0>(r)==false)
            {
                std::cout<<"mooc-CC parser : some errors occur, see details in 'errors.log' .\n";
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
                std::cout<<"moon-CC parser : syntax analysis done.             ++++####\n";
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
                    std::cout<<"moon-CC IR generator : semantic analysis done.     ++######\n";

                    //add the object code generation
                    {

                        target_gen target_generator("ir.mcc");
                        target_generator.print_parse_result();

                        target_generator.analyze_ir();
                        target_generator.output_target();
                        std::cout<<"moon-CC object code generator: object code done.   ########\n";
                    }
                }
                else   
                    std::cout<<"mooc-CC IR generator : some errors occur, see details in 'errors.log' .\n";
                
            }
        }
    }



    llvm::LLVMContext context;
    std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("my_module", context);

    // 定义函数参数类型
    llvm::Type* int32Type = llvm::Type::getInt32Ty(context);
    llvm::Type* argTypes[] = {int32Type, int32Type};

    // 创建函数类型
    llvm::FunctionType* funcType = llvm::FunctionType::get(int32Type, argTypes, false);

    // 创建函数对象
    llvm::Function* func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, "add", module.get());

    // 创建函数体基本块
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", func);

    // 创建IRBuilder对象，用于生成LLVM指令
    llvm::IRBuilder<> builder(context);
    builder.SetInsertPoint(entry);

    // 添加实现代码，调用llvm.add指令计算a+b并返回结果
    llvm::Value* a = &*func->arg_begin();
    llvm::Value* b = &*(func->arg_begin() + 1);
    llvm::Value* sum = builder.CreateAdd(a, b, "sum");
    builder.CreateRet(sum);

    // 输出LLVM IR代码
    //module->print(outs(), nullptr);

    std::error_code EC;
    llvm::raw_fd_ostream file("my_module.ll", EC);
    if (EC) {
        llvm::errs() << "Failed to open file: " << EC.message() << "\n";
        return 1;
    }
    module->print(file, nullptr);
        
        // parser.get_all_symbol_first();
        // parser.get_state_group_list();
        // parser.get_LR1_table();

        // parser.print_LR1_table();

        
    return 0;
}