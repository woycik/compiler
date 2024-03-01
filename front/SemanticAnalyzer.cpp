#include "SemanticAnalyzer.hpp"
#include <iostream>
#include <iterator>

void SemanticAnalyzer::setError(){
    error = true;
}

SemanticAnalyzer::SemanticAnalyzer() {}

bool SemanticAnalyzer::analyze(ProgramAll& program) {

    for (const auto& procedure : program.procedures.procedures) {
        saveProcedure(procedure->procHead);
    }    

    // Analiza procedur
    for (const auto& procedure : program.procedures.procedures) {
        analyzeProcedure(*procedure,procedure->procHead.procedure);
    }

    // Analiza funkcji głównej
    analyzeMain(program.main);

    if(error){
        return true;
    }
    return false;
}

void SemanticAnalyzer::analyzeMain(Main& main) {
    variableTable.clear(); 
    tabTable.clear();
    checkList.clear();
    analyzeDeclarations(main.declarations);
    analyzeCommands(main.commands,"", 0);

}

void SemanticAnalyzer::analyzeProcedure(Procedure& procedure,std::string invalidProcedure) {
    variableTable.clear(); 
    tabTable.clear(); 
    checkList.clear();
    analyzeProcHead(procedure.procHead);
    analyzeDeclarations(procedure.declarations);
    analyzeCommands(procedure.commands,invalidProcedure, 0);
}

void SemanticAnalyzer::analyzeProcHead(ProcHead& procHead) {
    analyzeArgsDecl(procHead.arguments);
}

void SemanticAnalyzer::saveProcedure(ProcHead& procHead){
    const std::string procName = procHead.procedure;
    if (procedureTable.find(procName) != procedureTable.end()) {
        setError();
        std::cerr << "Procedure '" << procName << "' already declared - error line: "<<procHead.lineNumber << std::endl;
    } else {
        std::vector<int> argVector;
        for(const auto& pair : procHead.arguments.varDeclarations){
            argVector.push_back(pair.second);
        }
        procedureTable[procName] = argVector;
    }
    
}


void SemanticAnalyzer::analyzeDeclarations(Declarations& declarations) {
    for (const auto& varDecl : declarations.varDeclarations) {
        if (variableTable.find(varDecl) != variableTable.end() || tabTable.find(varDecl) != tabTable.end()) {
            setError();
            std::cerr << "Variable '" << varDecl << "' already declared - error line: "<<declarations.lineNumber << std::endl;
        }
         else {
            variableTable[varDecl] = -1;
        }
    }

    for (const auto& tabDecl : declarations.tabDeclarations) {
        const auto& varName = tabDecl.first;
        const auto& size = tabDecl.second;
        if (tabTable.find(varName) != tabTable.end() || variableTable.find(varName) != variableTable.end()) {
            setError();
            std::cerr << "Variable '" << varName << "' already declared - error line: "<<declarations.lineNumber << std::endl;
        } else {
            tabTable[varName] = size;
        }
    }
}

void SemanticAnalyzer::analyzeArgsDecl(ArgsDecl& argsDecl) {
    for (const auto& varDecl : argsDecl.varDeclarations) {
        const std::string& name = varDecl.first;
        int value = varDecl.second;

        if (variableTable.find(name) != variableTable.end()) {
            setError(); 
            std::cerr << "Variable '" << name << "' already declared - error line: "<<argsDecl.lineNumber << std::endl;
        } else {
            if (value == 0) {
                variableTable[name] = 0;
            } else {
                tabTable[name] = 1;
            }
        }
    }
}


void SemanticAnalyzer::analyzeCommands(Commands& commands,std::string invalidProcedure, int depth) {
    for (const auto& command : commands.commands) {
        analyzeCommand(*command, invalidProcedure, depth);
    }
}

void SemanticAnalyzer::loopDepth(int depth, int variableDepth, std::string varName, long long lineNum){
    if(depth<variableDepth){
        std::cerr<<"Warning: Variable "<<varName<<" may not be inicialized : "<<lineNum<<std::endl;
    }
}

void SemanticAnalyzer::analyzeCommand(Command& command, std::string invalidProcedure, int depth) {
    if (dynamic_cast<AssignmentCommand*>(&command) != nullptr) {
        analyzeAssignmentCommand(static_cast<AssignmentCommand&>(command), depth);
    } else if (dynamic_cast<IfCommand*>(&command) != nullptr) {
        analyzeIfCommand(static_cast<IfCommand&>(command),invalidProcedure,depth);
    } else if (dynamic_cast<WhileCommand*>(&command) != nullptr) {
        analyzeWhileCommand(static_cast<WhileCommand&>(command),invalidProcedure, depth);
    } else if (dynamic_cast<RepeatCommand*>(&command) != nullptr) {
        analyzeRepeatCommand(static_cast<RepeatCommand&>(command),invalidProcedure, depth);
    } else if (dynamic_cast<ProcCallCommand*>(&command) != nullptr) {
        analyzeProcCallCommand(static_cast<ProcCallCommand&>(command),invalidProcedure, depth);
    } else if (dynamic_cast<ReadCommand*>(&command) != nullptr) {
        analyzeReadCommand(static_cast<ReadCommand&>(command),invalidProcedure);
    } else if (dynamic_cast<WriteCommand*>(&command) != nullptr) {
        analyzeWriteCommand(static_cast<WriteCommand&>(command),invalidProcedure, depth);
    }
}

void SemanticAnalyzer::analyzeAssignmentCommand(AssignmentCommand& assignmentCommand, int depth) {
    auto& identifier = assignmentCommand.identifier;
    auto& expression = assignmentCommand.expression;


    //Expression:
if (auto expressionIdentifier = dynamic_cast<MathExpression*>(&expression)) {
    auto& leftValue = expressionIdentifier->left;
    auto& rightValue = expressionIdentifier->right;

    if (auto expNameLeftString = dynamic_cast<StringValue*>(&leftValue)) {
        auto name1 = expNameLeftString->stringValue.getName();
        auto expItLeft = variableTable.find(name1);
        auto expTabItLeft = tabTable.find(name1);
 
        isDeclared(expNameLeftString->stringValue,assignmentCommand.lineNumber, depth);
    }
    else{
        auto number = dynamic_cast<NumberValue*>(&leftValue);
            if(number->number<0){
                setError(); 
                std::cerr << "Number cant be below zero - error line: "<< assignmentCommand.lineNumber << std::endl;

            }
        
    }

    if (auto expNameRightString = dynamic_cast<StringValue*>(&rightValue)) {
        auto name2 = expNameRightString->stringValue.getName();
        auto expItRight = variableTable.find(name2);
        auto expTabItRight = tabTable.find(name2); 
        isDeclared(expNameRightString->stringValue,assignmentCommand.lineNumber, depth);

    }
    else{
        auto number = dynamic_cast<NumberValue*>(&rightValue);
            if(number->number<0){
                setError(); 
                std::cerr << "Number cant be below zero - error line: "<< assignmentCommand.lineNumber << std::endl;

            }
        
    }
}
//ValueExpression
if (auto expressionIdentifier = dynamic_cast<ValueExpression*>(&expression)) {
    auto& value = expressionIdentifier->value;    
    if (auto expNameLeftString = dynamic_cast<StringValue*>(&value)) {
         isDeclared(expNameLeftString->stringValue, assignmentCommand.lineNumber, depth);

    }
}


//Identifiers
if (auto varIdentifier = dynamic_cast<VariableIdentifier*>(&identifier)) {
        auto varName = varIdentifier->name;
        auto varIt = variableTable.find(varName);
        auto tabIt = tabTable.find(varName);

        if(tabIt != tabTable.end()){
            setError();
            std::cerr << "Variable '" << varName << "' is array - error linr: "<<assignmentCommand.lineNumber << std::endl;

        }
        else if (varIt == variableTable.end() ) {
            setError();    
            std::cerr << "Variable '" << varName << "' not declared - error line: "<<assignmentCommand.lineNumber << std::endl;
        } 
        else if(varIt != variableTable.end()) {
            if(varIt->second==-1){
                varIt->second =depth;
            }
            else{
                if(depth<varIt->second){
                    varIt->second = depth;
                }
            }
        }
    }
     else if (auto arrayAccessIdentifier = dynamic_cast<ArrayAccessIdentifier*>(&identifier)) {
        auto varName = arrayAccessIdentifier->name;
        auto arrayIt = tabTable.find(varName);

        if (arrayIt == tabTable.end()) {
            setError(); 
            std::cerr <<"Variable '" << varName << "' not declared - error line: "<<assignmentCommand.lineNumber << std::endl;
        } 

    } else if (auto varAccessIdentifier = dynamic_cast<VariableArrayAccessIdentifier*>(&identifier)) {
            auto tabName = varAccessIdentifier->name;
            auto tabIt = tabTable.find(tabName);
            int value;

            if (tabIt == tabTable.end()) {
                setError(); 
                std::cerr <<" Variable '" << tabName << "' not declared - error line: "<<assignmentCommand.lineNumber << std::endl;
            }

            auto accessName = varAccessIdentifier->accessName;
            auto accessIt = variableTable.find(accessName);

            if (accessIt == variableTable.end()) {
                setError(); 
                std::cerr <<"Variable '" << accessName << "' not declared - error line: "<<assignmentCommand.lineNumber << std::endl;
            }
            else{
                
                if(accessIt->second==-1){
                    setError(); 
                    std::cerr << "Variable '" << accessName << "' not inicialized - error line: "<<assignmentCommand.lineNumber << std::endl;
   
                }
                else{
                    loopDepth(depth, accessIt->second,accessName,assignmentCommand.lineNumber);
                }
            }
    }
}

void SemanticAnalyzer::analyzeCondition(Condition& condition,int depth){
    analyzeStatement(condition.left,condition.right,condition.lineNumber, depth);
}

void SemanticAnalyzer::analyzeIfCommand(IfCommand& ifCommand,std::string invalidProcedure,int depth) {
    analyzeCondition(ifCommand.condition, depth);
    analyzeCommands(ifCommand.trueBranch,invalidProcedure, depth+1);
    if(ifCommand.falseBranch.commands.size()!=0){
            analyzeCommands(ifCommand.falseBranch,invalidProcedure, depth+1);
    }   
    checkVariables(depth+1);

}

void SemanticAnalyzer::analyzeWhileCommand(WhileCommand& whileCommand,std::string invalidProcedure,int depth) {
    analyzeCondition(whileCommand.condition, depth);
    analyzeCommands(whileCommand.body,invalidProcedure, depth+1);
    checkVariables(depth+1);
}

void SemanticAnalyzer::analyzeRepeatCommand(RepeatCommand& repeatCommand,std::string invalidProcedure, int depth) {
    analyzeCondition(repeatCommand.condition, depth);
    analyzeCommands(repeatCommand.body,invalidProcedure,depth+1);
    checkVariables(depth+1);
}

void SemanticAnalyzer::analyzeProcCallCommand(ProcCallCommand& procCallCommand, std::string invalidProcedure, int depth) {
    
    const std::string name = procCallCommand.procCall.procedure;
    auto procIt= procedureTable.find(name);
    if(procIt!=procedureTable.end()){
        if(name == invalidProcedure){
            setError(); 
            std::cerr<<"Recursion is forbidden - error line: "<<procCallCommand.lineNumber<<std::endl;
        }
        else{
            Args& args = procCallCommand.procCall.arguments;
            for (const auto &pair : procedureTable) {
                const std::string &procedureName = pair.first;

                if(procedureName == name){
                    const std::vector<int> &values = pair.second;
                    int size = args.identifiers.size();
                    int i=0;
                    if(size != values.size()){
                        setError(); 
                        std::cerr<<"Invalid number of arguments - error line : "<<procCallCommand.lineNumber<<std::endl;
                    }
                    for (int value : values) {
                        if(value ==0){
                            if(variableTable.find(args.identifiers[i])!=variableTable.end()){
                                if(variableTable[args.identifiers[i]]==-1){
                                    variableTable[args.identifiers[i]]=0;
                                }
                            }else{
                                setError(); 
                                std::cerr<<"Invalid data type in procedure: "<<args.identifiers[i]<< " - error line: "<<procCallCommand.lineNumber<<std::endl;

                            }
                        }
                        else{
                            if(tabTable.find(args.identifiers[i])==tabTable.end()){
                                setError(); 
                                std::cerr<<"Invalid data type in procedure: "<<args.identifiers[i]<<" - error line: "<<procCallCommand.lineNumber<<std::endl;
                            };
                        }
                        i++;
                    }

                }
            }
        }
    }
    else{
        setError(); 
        std::cerr<<"Unknown procedure "<<name<<" - error line: "<<procCallCommand.lineNumber<<std::endl;
    }

}


void SemanticAnalyzer::analyzeReadCommand(ReadCommand& readCommand,std::string invalidProcedure) {
    const auto &name = readCommand.identifier.getName();
    
    auto variableIt = variableTable.find(name);
    if (variableIt == variableTable.end()) {
        setError(); 
        std::cerr << "Invalid variable: " << name <<" - error line: "<<readCommand.lineNumber<< std::endl;
    } else {
        if(variableIt !=variableTable.end()){
            variableIt->second = 0; 
        }
    }
}




void SemanticAnalyzer::analyzeWriteCommand(WriteCommand& writeCommand,std::string invalidProcedure, int depth) {
    auto& value = writeCommand.value;
    if (dynamic_cast<NumberValue*>(&value) != nullptr) {
    } else if (auto stringValue = dynamic_cast<StringValue*>(&value)) {
        auto &identifier = stringValue->stringValue;
        
        if (auto varIdentifier = dynamic_cast<VariableIdentifier*>(&identifier)) {

            auto varName = varIdentifier->name;
            auto varIt = variableTable.find(varName);
            auto tabIt = tabTable.find(varName);

            if(tabIt != tabTable.end()){
                setError(); 
                std::cerr << "Error: Variable '" << varName << "' is array - error line: "<<writeCommand.lineNumber << std::endl;

            }
            else if (varIt == variableTable.end() ) {
                setError(); 
                std::cerr << "Error: Variable '" << varName << "' not declared - error line: "<<writeCommand.lineNumber << std::endl;
            } 
            else if(varIt != variableTable.end()) {
                if(varIt->second==-1){
                    setError(); 
                    std::cerr << "Error: Variable '" << varName << "' not inicialized - error line: "<<writeCommand.lineNumber << std::endl;
                }
                else{
                    loopDepth(depth,varIt->second,varName,writeCommand.lineNumber);
                }
            }
    }
     else if (auto arrayAccessIdentifier = dynamic_cast<ArrayAccessIdentifier*>(&identifier)) {
        auto varName = arrayAccessIdentifier->name;
        auto arrayIt = tabTable.find(varName);

        if (arrayIt == tabTable.end()) {
            setError(); 
            std::cerr << "Variable '" << varName << "' not declared - error line: "<<writeCommand.lineNumber << std::endl;
        } 

    } else if (auto varAccessIdentifier = dynamic_cast<VariableArrayAccessIdentifier*>(&identifier)) {
            auto tabName = varAccessIdentifier->name;
            auto tabIt = tabTable.find(tabName);
            int value;

            if (tabIt == tabTable.end()) {
                setError(); 
                std::cerr << "Variable '" << tabName << "' not declared - error line: "<<writeCommand.lineNumber << std::endl;
            }

            auto accessName = varAccessIdentifier->accessName;
            auto accessIt = variableTable.find(accessName);

            if (accessIt == variableTable.end()) {
                setError(); 
                std::cerr << "Variable '" << accessName << "' not declared - error line: "<<writeCommand.lineNumber << std::endl;
            }
            else{
                if(accessIt->second==-1){
                    setError();
                    std::cerr << "Variable '" << accessName << "' not inicialized - error line: "<<writeCommand.lineNumber << std::endl;
   
                }
                else{
                    loopDepth(depth, accessIt->second, accessName, writeCommand.lineNumber);
                }
            }
    }
    }
}




void SemanticAnalyzer::analyzeStatement(Value& leftValue, Value& rightValue, long long lineNumber,int depth){
    if (auto expNameLeftString = dynamic_cast<StringValue*>(&leftValue)) {
        auto name1 = expNameLeftString->stringValue.getName();
        auto expItLeft = variableTable.find(name1);
        auto expTabItLeft = tabTable.find(name1);
 
        if (expItLeft == variableTable.end() && expTabItLeft == tabTable.end()) {
            setError();
            std::cerr << "Variable '" << name1 << "' not declared - error line: "<<lineNumber << std::endl;
        } 
        if(expItLeft != variableTable.end()) {
            if (expItLeft->second == -1) {
                setError();
                std::cerr << "Variable '" << name1 << "' not initialized - error line: "<< lineNumber << std::endl;
            }
            else{
                loopDepth(depth,expItLeft->second, name1, lineNumber);
            }
        }
    }
    else{
        auto number = dynamic_cast<NumberValue*>(&leftValue);
            if(number->number<0){
                setError();
                std::cerr << "Number cant be below zero - error line: "<< lineNumber << std::endl;

            }
        
    }

    if (auto expNameRightString = dynamic_cast<StringValue*>(&rightValue)) {
        auto name2 = expNameRightString->stringValue.getName();
        auto expItRight = variableTable.find(name2);
        auto expTabItRight = tabTable.find(name2); 
        if (expItRight == variableTable.end() && expTabItRight == tabTable.end()) {
            setError();
            std::cerr << "Variable '" << name2 << "' not declared - error line: "<<lineNumber << std::endl;
        } 
        if(expItRight != variableTable.end() ) {
            if (expItRight->second == -1) {
                setError();
                std::cerr << "Variable '" << name2 << "' not initialized - error line "<<lineNumber << std::endl;
            }
            else{
                loopDepth(depth,expItRight->second, name2, lineNumber);

            }
        }
    }
    else{
        auto number = dynamic_cast<NumberValue*>(&rightValue);
            if(number->number<0){
                setError();
                std::cerr << "Number cant be below zero - error line: "<< lineNumber << std::endl;

            }
    }
}


void SemanticAnalyzer::isDeclared(Identifier &identifier, long long lineNumber, int depth){
    if (auto varIdentifier = dynamic_cast<VariableIdentifier*>(&identifier)) {

        auto varName = varIdentifier->name;
        auto varIt = variableTable.find(varName);
        auto tabIt = tabTable.find(varName);

        if(tabIt != tabTable.end()){
            setError();
            std::cerr << "Variable '" << varName << "' is array - error linr: "<<lineNumber << std::endl;

        }
        else if (varIt == variableTable.end() ) {
            setError();    
            std::cerr << "Variable '" << varName << "' not declared - error line: "<<lineNumber << std::endl;
        } 
        if(varIt != variableTable.end()) {
            
            if(varIt->second==-1 && depth==0){
                setError();    
                std::cerr << "Variable '" << varName << "' not inicialized - error line: "<<lineNumber << std::endl;
                
            }
            else if(varIt->second==-1 && depth>0){
                    addToCheckList(varName, depth);
                    std::cerr<<"Variable "<<varName<<" may not be inicialized : "<<lineNumber<<std::endl;


                }
            else{
                loopDepth(depth, varIt->second, varName,lineNumber);
            }
        }
    }
     else if (auto arrayAccessIdentifier = dynamic_cast<ArrayAccessIdentifier*>(&identifier)) {
        auto varName = arrayAccessIdentifier->name;
        auto arrayIt = tabTable.find(varName);

        if (arrayIt == tabTable.end()) {
            setError(); 
            std::cerr << "Variable '" << varName << "' not declared - error line: "<<lineNumber << std::endl;
        } 

    } else if (auto varAccessIdentifier = dynamic_cast<VariableArrayAccessIdentifier*>(&identifier)) {
            auto tabName = varAccessIdentifier->name;
            auto tabIt = tabTable.find(tabName);
            int value;

            if (tabIt == tabTable.end()) {
                setError(); 
                std::cerr << " Variable '" << tabName << "' not declared - error line: "<<lineNumber << std::endl;
            }

            auto accessName = varAccessIdentifier->accessName;
            auto accessIt = variableTable.find(accessName);

            if (accessIt == variableTable.end()) {
                setError(); 
                std::cerr << "Variable '" << accessName << "' not declared - error line: "<<lineNumber << std::endl;
            }
            else{
                if(accessIt->second==-1 && depth==0){
                    setError(); 
                    std::cerr << "Variable '" << accessName << "' not inicialized - error line: "<<lineNumber << std::endl;
   
                }
                else if(accessIt->second==-1 && depth>0){
                    addToCheckList(accessName, depth);
                    std::cerr<<"Variable "<<accessName<<" may not be inicialized : "<<lineNumber<<std::endl;

                }
                else{
                    loopDepth(depth, accessIt->second, accessName, lineNumber);
                }
            }
    }

}

void SemanticAnalyzer::addToCheckList(std::string name, int depth){
    checkList.push_back(std::make_pair(name, depth));

}

void SemanticAnalyzer::checkVariables(int depth){
    for(auto var : checkList){
        if(var.second>depth){
            auto varIt = variableTable.find(var.first);
            if(varIt->second ==-1){
                setError(); 
                std::cerr << "Variable '" << var.first << "' not declared in specified depth - error"<< std::endl;
            }

        }
    }

}
