#include <iostream>
#include <cstdio>
#include <fstream>
#include "./front/SemanticAnalyzer.hpp"
#include "./back/CodeGenerator.hpp"
#include "./front/ast.hpp"


extern FILE *yyin;
extern int yyparse();
extern void yyerror(const char* s);
extern std::vector<std::string> commandList;

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Invalid number of arguments"<<std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open input file"<<std::endl;
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Unable to open input file"<<std::endl;
        return 1;
    }

    yyparse();

    std::ofstream outputFile(argv[2]);
    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file"<<std::endl;
        fclose(yyin);
        return 1;
    }

    for (const auto& command : commandList) {
        outputFile << command << std::endl;
    }

    outputFile.close();

    fclose(yyin);

    return 0;
}