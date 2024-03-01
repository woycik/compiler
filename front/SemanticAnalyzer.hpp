#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <unordered_map>
#include <vector>
#include "ast.hpp"



//Semas

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    bool analyze(ProgramAll& program);
    bool error = false;

private:
    std::unordered_map<std::string, int> variableTable;
    std::unordered_map<std::string, int> tabTable;
    std::unordered_map<std::string, std::vector<int>> procedureTable;
    std::vector<std::pair<std::string, int>> checkList;

    void analyzeMain(Main& main);
    void analyzeProcedure(Procedure& procedure, std::string invalidProcedure);
    void analyzeProcHead(ProcHead& procHead);
    void analyzeDeclarations(Declarations& declarations);
    void analyzeArgsDecl(ArgsDecl& argsDecl);
    void analyzeCommands(Commands& commands,std::string invalidProcedure, int depth);
    void analyzeCommand(Command& command,std::string invalidProcedure, int depth);
    void analyzeAssignmentCommand(AssignmentCommand& assignmentCommand, int depth);
    void analyzeIfCommand(IfCommand& ifCommand,std::string invalidProcedure,int depth);
    void analyzeWhileCommand(WhileCommand& whileCommand,std::string invalidProcedure,int depth);
    void analyzeRepeatCommand(RepeatCommand& repeatCommand,std::string invalidProcedure,int depth);
    void analyzeProcCallCommand(ProcCallCommand& procCallCommand,std::string invalidProcedure,int depth);
    void analyzeReadCommand(ReadCommand& readCommand,std::string invalidProcedure);
    void analyzeWriteCommand(WriteCommand& writeCommand,std::string invalidProcedure, int depth);
    void saveProcedure(ProcHead& procHead);
    void analyzeCondition(Condition& condition,int depth);
    void analyzeStatement(Value& left, Value& right, long long lineNumber,int depth);
    void isDeclared(Identifier &identifier, long long lineNumber, int depth);
    void setError();
    void loopDepth(int depth, int variableDepth, std::string varName, long long lineNum);
    void addToCheckList(std::string name, int depth);
    void checkVariables(int depth);

};

#endif // SEMANTIC_ANALYZER_HPP
