#ifndef CODEGENERATOR
#define CODEGENERATOR

#include <vector>
#include "./../front/ast.hpp"
#include "AsmInstruction.hpp"
#include "Memory.hpp"

class CodeGenerator{
    public:
        CodeGenerator() {};

        void generateCode(ProgramAll& program);
        std::vector<std::string> getCommandList();

        AsmInstructions assembler;
        Memory memory;
        

    private:
        std::vector<std::pair<std::string, long long>> memoryArray;
        void generateCodeMain(Main& main);
        void generateCodeProcedure(Procedure& procedure);
        void generateCodeProcHead(ProcHead& procHead, std::string parent);
        void generateCodeDeclarations(Declarations& declarations, std::string name);
        void generateCodeArgsDecl(ArgsDecl& argsDecl);
        void generateCodeCommands(Commands& commands, std::string parent);
        void generateCodeCommand(Command& command, std::string parent);
        void generateCodeAssignmentCommand(AssignmentCommand& assignmentCommand, std::string parent);
        void generateCodeIfCommand(IfCommand& ifCommand, std::string parent);
        void generateCodeWhileCommand(WhileCommand& whileCommand, std::string parent);
        void generateCodeRepeatCommand(RepeatCommand& repeatCommand, std::string parent);
        void generateCodeProcCallCommand(ProcCallCommand& procCallCommand, std::string parent);
        void generateCodeReadCommand(ReadCommand& readCommand, std::string parent);
        void generateCodeWriteCommand(WriteCommand& writeCommand, std::string parent);
        void saveProcedure(ProcHead& procHead);
        long long loadValueToA(Value& value, std::string parent);
        long long generateCodeValueExpression(Expression& expression,  std::string parent);
        void setRegister(std::string name, long long number);
        void assignRegisterValue(std::string registerName, long long value);
        void generateCodeCondition(Condition& condition, ConditionType operation, std::string parent);
        void divideLogarithmically();
        void multiplyLogarithmically();
        void getFromMemory(long long address);
        void saveInMemory(long long address);
        void resolveReference(std::string name, std::string parent);
        void getValueFromReference(std::string name, std::string parent);
        void loadVarAddressToA(std::string name, std::string parent);
        void loadArrayToA(std::string name, long long index, std::string parent);
        void loadAccesArrayToA(std::string name, std::string accescName, std::string parent);
        void getAddressFromArguments(std::string identifier, std::string parent);
        void getAddressFromReference(std::string name, std::string parent);
        bool isNumber(const std::string& str);
        bool isString(const std::string& str);
        bool isArray(const std::string& str);
        bool isArrayWithIndex(const std::string& str);
        void moduloLogagarithmically();


};

#endif //CodeGenerator