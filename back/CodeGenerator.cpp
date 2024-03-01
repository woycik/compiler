#include "CodeGenerator.hpp"
#include <iostream>
#include <iterator>
#include <bitset>
#include <regex>





void CodeGenerator::generateCode(ProgramAll& program) {
    // Analiza procedur
    if(program.procedures.procedures.size()>0){
        assembler.jumpInstr("main");
    }
    for (const auto& procedure : program.procedures.procedures) {
        memory.insertToMemory(memory.getMemoryPointer(),"",procedure->procHead.procedure,false);
    }
    for (const auto& procedure : program.procedures.procedures) {
        generateCodeProcedure(*procedure);
    }
    
    // Analiza funkcji głównej
    generateCodeMain(program.main);
    assembler.haltInstr();
    assembler.replaceValues();

    program.~ProgramAll();
}

std::vector<std::string> CodeGenerator::getCommandList(){
    return assembler.instructionList;
}

void CodeGenerator::generateCodeMain(Main& main) {
    assembler.insertToProcedureList("main",assembler.k);
    generateCodeDeclarations(main.declarations, "main");
    generateCodeCommands(main.commands, "main");
}



void CodeGenerator::generateCodeCondition(Condition& condition, ConditionType operation, std::string parent) {
    if (operation == eq) {
        loadValueToA(condition.left,parent);
        assembler.putInstr("e"); //w b jest aktualnie left
        loadValueToA(condition.right,parent);
        assembler.putInstr("d"); // right jest w d i a
        assembler.subInstr("e"); //right - left
        assembler.putInstr("f");

        assembler.getInstr("e"); //wyciagam left i daje do a
        assembler.subInstr("d");

        assembler.addInstr("f");

        assembler.jposInstr("-1"); 
    } else if (operation == l) {
        loadValueToA(condition.right, parent);
        assembler.putInstr("f");

        loadValueToA(condition.left, parent);
        assembler.incInstr("a");
        assembler.subInstr("f");
        assembler.jposInstr("-1");   // Wstaw odpowiednią etykietę
    } else if (operation == g) {
        loadValueToA(condition.right, parent);
        assembler.putInstr("f");

        loadValueToA(condition.left, parent);
        assembler.subInstr("f");

        assembler.jzeroInstr("-1");   // Wstaw odpowiednią etykietę
    } else if (operation == leq) {
        loadValueToA(condition.right, parent);
        assembler.putInstr("f");
        loadValueToA(condition.left, parent);
        assembler.subInstr("f");

        assembler.jposInstr("-1");   // Wstaw odpowiednią etykietę
    } else if (operation == geq) {
        loadValueToA(condition.right, parent);
        assembler.putInstr("f");

        loadValueToA(condition.left, parent);
        assembler.incInstr("a");
        assembler.subInstr("f");
        assembler.jzeroInstr("-1");   // Wstaw odpowiednią etykietę


    } else if (operation == neq) {
        loadValueToA(condition.left, parent);
        assembler.putInstr("e"); //w b jest aktualnie left
        loadValueToA(condition.right, parent);
        assembler.putInstr("d");
        assembler.subInstr("e"); //right - left
        assembler.putInstr("f");

        assembler.getInstr("e"); //wyciagam left i daje do a
        assembler.subInstr("d");

        assembler.addInstr("f");

        assembler.jzeroInstr("-1"); 
    }

}



void CodeGenerator::generateCodeDeclarations(Declarations& declarations, std::string parent) {

    for(auto var : declarations.varDeclarations){
        memory.insertToMemory(memory.getMemoryPointer(),parent,var,false);
    }

    for(auto tab : declarations.tabDeclarations){
        for(int i=0;i<tab.second;i++){
            memory.insertToMemory(memory.getMemoryPointer(),parent,tab.first,false);
        }
    }
}

void CodeGenerator::generateCodeCommands(Commands& commands, std::string parent) {
    for (const auto& command : commands.commands) {
        generateCodeCommand(*command, parent);
    }
}

void CodeGenerator::generateCodeCommand(Command& command, std::string parent) {
    if (dynamic_cast<AssignmentCommand*>(&command) != nullptr) {
        generateCodeAssignmentCommand(static_cast<AssignmentCommand&>(command), parent);
    } else if (dynamic_cast<IfCommand*>(&command) != nullptr) {
        generateCodeIfCommand(static_cast<IfCommand&>(command), parent);
    } else if (dynamic_cast<WhileCommand*>(&command) != nullptr) {
        generateCodeWhileCommand(static_cast<WhileCommand&>(command), parent);
    } else if (dynamic_cast<RepeatCommand*>(&command) != nullptr) {
        generateCodeRepeatCommand(static_cast<RepeatCommand&>(command), parent);
    } else if (dynamic_cast<ProcCallCommand*>(&command) != nullptr) {
        generateCodeProcCallCommand(static_cast<ProcCallCommand&>(command), parent);
    } else if (dynamic_cast<ReadCommand*>(&command) != nullptr) {
        generateCodeReadCommand(static_cast<ReadCommand&>(command), parent);
    } else if (dynamic_cast<WriteCommand*>(&command) != nullptr) {
        generateCodeWriteCommand(static_cast<WriteCommand&>(command), parent);
    }
}



void CodeGenerator::setRegister(std::string name, long long number) {
    std::bitset<64> binaryRepresentation(number);
    std::string binaryString = binaryRepresentation.to_string();

    long long size= binaryString.find_first_of('1');
    
    if(size==-1){
        size=0;
    }else{
        size = 64-size;
    }

    assembler.rstInstr(name);

    for (long  i = size - 1; i >= 0; --i) {

        long long bit = (number >> i) & 1;
        if(i==size-1){
            assembler.incInstr(name);
        }
        if(bit==1 && i!=size-1){
            assembler.shlInstr(name);
            assembler.incInstr(name);
        }
        if(bit==0){
            assembler.shlInstr(name);
        }
    }
}

void CodeGenerator::assignRegisterValue(std::string registerName, long long value){
    setRegister(registerName, value);
    
}


void CodeGenerator::multiplyLogarithmically(){

    assembler.putInstr("c"); //wkładam wartosc a do c

    assembler.rstInstr("a");
    assembler.putInstr("d");


    assembler.instr.insertToVector();
    long long m = assembler.k;

        assembler.getInstr("e"); //wyciagam b
        assembler.jzeroInstr("-1");//jezeli >0 to jescze raz pętla

        assembler.getInstr("e"); //biore b i daje do a
        assembler.shrInstr("a");
        assembler.shlInstr("a");
        assembler.incInstr("a");
        assembler.subInstr("e");

        assembler.instr.insertToVector();
        assembler.jposInstr("-1");
        // if value in register b %2 ==1
        assembler.getInstr("d"); // bioew d w którym zapisywany jest wynik
        assembler.addInstr("c"); // dadaje c w którym jest a
        
        assembler.putInstr("d");    //zapisuje w d
        assembler.instr.setValue(assembler.k);  
        assembler.shlInstr("c");
        assembler.shrInstr("e");
        
        assembler.jumpInstr(std::to_string(m));
        assembler.instr.setValue(assembler.k);

        assembler.getInstr("d");
} 
    



void CodeGenerator::divideLogarithmically() {

    assembler.putInstr("c"); // Wkładam wartość a do c
    assembler.getInstr("e"); //dodane

    assembler.instr.insertToVector(); //dodane
    assembler.jzeroInstr("-1"); //dodane


    assembler.rstInstr("a");
    assembler.putInstr("d"); // quotient


    long long m = assembler.k;
    assembler.getInstr("c"); // get a
    assembler.incInstr("a"); // a+1
    assembler.subInstr("e"); //a+1-b

    assembler.instr.insertToVector();
    assembler.jzeroInstr("-1");     //While a>=b

    assembler.rstInstr("a");
    assembler.incInstr("a"); 
    assembler.putInstr("b"); //multiple = 1

    assembler.getInstr("e");
    assembler.putInstr("f"); //divisor=b


    //---------
    long long n = assembler.k;

    assembler.getInstr("c");    //a+1-divisor
    assembler.incInstr("a");
    assembler.subInstr("f");
    assembler.instr.insertToVector();
    assembler.jzeroInstr("-1");


    assembler.decInstr("a"); //a-divizor
    assembler.putInstr("c"); //zapis w c
    assembler.getInstr("d"); //quodient = quodient+multiple
    assembler.addInstr("b");
    assembler.putInstr("d");    //zapis w d
    assembler.shlInstr("f");    // divizor=divisor<<1
    assembler.shlInstr("b");    //multifle=multiple<<1


    //----------

    assembler.jumpInstr(std::to_string(n));
    assembler.instr.setValue(assembler.k);

    assembler.jumpInstr(std::to_string(m));
    assembler.instr.setValue(assembler.k);

    assembler.getInstr("d");
    assembler.instr.setValue(assembler.k); //dodane

}


void CodeGenerator::moduloLogagarithmically(){

    assembler.putInstr("c");
    assembler.instr.insertToVector();
    assembler.jzeroInstr("-1");

    assembler.getInstr("e");
    assembler.instr.insertToVector();
    assembler.jzeroInstr("-1");


    long long n = assembler.k;
    assembler.getInstr("c");
    assembler.incInstr("a");
    assembler.subInstr("e");
    assembler.instr.insertToVector();
    assembler.jzeroInstr("-1");

    assembler.getInstr("e");
    assembler.putInstr("d");//tempB przypisanie

    long long m = assembler.k;
    assembler.getInstr("c");//while a>=tempB
    assembler.incInstr("a");
    assembler.subInstr("d");

    assembler.instr.insertToVector();
    assembler.jzeroInstr("-1");

    assembler.getInstr("c");
    assembler.subInstr("d");
    assembler.putInstr("c");

    assembler.shlInstr("d"); //tempB<<1

    //jumpy

    //zakończ while
    assembler.jumpInstr(std::to_string(m));
    assembler.instr.setValue(assembler.k);

    assembler.jumpInstr(std::to_string(n));
    assembler.instr.setValue(assembler.k);

    assembler.instr.setValue(assembler.k);
    assembler.instr.setValue(assembler.k);

    assembler.getInstr("c");


}


void CodeGenerator::generateCodeAssignmentCommand(AssignmentCommand& assignmentCommand, std::string parent) {

    if(auto identifier = dynamic_cast<VariableIdentifier*>(&assignmentCommand.identifier)){
        std::string name  = identifier->name;
        loadVarAddressToA(name, parent);
        assembler.putInstr("g");
        generateCodeValueExpression(assignmentCommand.expression, parent);
        assembler.storeInstr("g");

      
    }
    else if( auto identifier_ = dynamic_cast<ArrayAccessIdentifier*>(&assignmentCommand.identifier)){
        std::string name  = identifier_->name;
        long long index = identifier_->index;
        loadArrayToA(name,index,parent);
        assembler.putInstr("g");
        generateCodeValueExpression(assignmentCommand.expression, parent);
        assembler.storeInstr("g");
        
    }
    else{
        auto var_idenfifier = dynamic_cast<VariableArrayAccessIdentifier*>(&assignmentCommand.identifier);
        std::string name = var_idenfifier->name;
        std::string accessName = var_idenfifier->accessName;
        loadAccesArrayToA(name,accessName,parent);
        assembler.putInstr("g");
        generateCodeValueExpression(assignmentCommand.expression, parent);
        assembler.storeInstr("g");
    }
    
}



void CodeGenerator::generateCodeIfCommand(IfCommand& ifCommand, std::string parent) {
    assembler.instr.insertToVector();
    generateCodeCondition(ifCommand.condition, ifCommand.condition.operation, parent);
    generateCodeCommands(ifCommand.trueBranch, parent);
    if(ifCommand.falseBranch.commands.size()!=0){
        assembler.jumpInstr("-1");
        assembler.instr.setValue(assembler.k);
        assembler.instr.insertToVector();
        generateCodeCommands(ifCommand.falseBranch, parent);
        assembler.instr.setValue(assembler.k);

    }
    else{
        assembler.instr.setValue(assembler.k);
    }
}

void CodeGenerator::generateCodeWhileCommand(WhileCommand& whileCommand, std::string parent) {
    long long m = assembler.k;
    assembler.instr.insertToVector();
    generateCodeCondition(whileCommand.condition, whileCommand.condition.operation, parent);
    generateCodeCommands(whileCommand.body, parent);
    assembler.instr.insertToVector();
    assembler.jumpInstr("-1"); 
    assembler.instr.setValue(m);
    assembler.instr.setValue(assembler.k);

}

void CodeGenerator::generateCodeRepeatCommand(RepeatCommand& repeatCommand,  std::string parent) {
    long long m = assembler.k;
    generateCodeCommands(repeatCommand.body, parent);
    assembler.instr.insertToVector();
    generateCodeCondition(repeatCommand.condition, repeatCommand.condition.operation, parent);
    assembler.instr.setValue(m);
}


void CodeGenerator::generateCodeReadCommand(ReadCommand& readCommand,  std::string parent) {
    long long address=0;

    if(auto varidentifier = dynamic_cast<VariableIdentifier*>(&readCommand.identifier)){
        std::string name  = varidentifier->name;
        loadVarAddressToA(name,parent);

    }
    else if( auto identifier_ = dynamic_cast<ArrayAccessIdentifier*>(&readCommand.identifier)){
        std::string name  = identifier_->name;
        long long index = identifier_->index;
        loadArrayToA(name,index,parent);
    }
    else{
        auto var_idenfifier = dynamic_cast<VariableArrayAccessIdentifier*>(&readCommand.identifier);
        std::string name = var_idenfifier->name;
        loadAccesArrayToA(name,var_idenfifier->accessName,parent);
    }

    assembler.putInstr("c");
    assembler.readInstr(); 
    assembler.storeInstr("c");

}


void CodeGenerator::generateCodeWriteCommand(WriteCommand& writeCommand, std::string parent) {
    loadValueToA(writeCommand.value, parent);
    assembler.writeInstr();
}


long long CodeGenerator::generateCodeValueExpression(Expression& expression,  std::string parent) {
    if (auto constantExpression = dynamic_cast<ValueExpression*>(&expression)) {

        if(auto numValue = dynamic_cast<NumberValue*>(&constantExpression->value)){
            std::string registerName = "a";
            long long number = numValue->number;
            assignRegisterValue(registerName, number);
            return number;
        }
        else{
            auto varValue = dynamic_cast<StringValue*>(&constantExpression->value);
            if(auto varIdentifier = dynamic_cast<VariableIdentifier*>(&varValue->stringValue)){
                loadVarAddressToA(varIdentifier->name, parent);
                assembler.loadInstr("a"); 
 
            }
            else if(auto array = dynamic_cast<ArrayAccessIdentifier*>(&varValue->stringValue)){
                loadArrayToA(array->name,array->index,parent);
                assembler.loadInstr("a");
            }
            else if( auto arrayIdentifier = dynamic_cast<VariableArrayAccessIdentifier*>(&varValue->stringValue)){
                loadAccesArrayToA(arrayIdentifier->name,arrayIdentifier->accessName,parent);
                assembler.loadInstr("a");
            }
        }
        
    } 
    else if (auto binaryExpression = dynamic_cast<MathExpression*>(&expression)) {
        if(binaryExpression->operation==divi){
            if(auto right = dynamic_cast<NumberValue*>(&binaryExpression->right)){
                if(right->number && (right->number & (right->number - 1)) == 0) {
                    loadValueToA(binaryExpression->left, parent);
                    long long n=right->number;
                    while(n!=1){
                        assembler.shrInstr("a");
                        n=n/2;
                    }
                }
                else{
                    loadValueToA(binaryExpression->right, parent); 
                    assembler.putInstr("e"); 
                    loadValueToA(binaryExpression->left, parent); 
                    divideLogarithmically();

                }
            }
            else{
                loadValueToA(binaryExpression->right, parent);
                assembler.putInstr("e");
                loadValueToA(binaryExpression->left, parent);
                divideLogarithmically();

            }
        }


        else if(binaryExpression->operation==mult){
            if(auto right = dynamic_cast<NumberValue*>(&binaryExpression->right) ){
                if(right->number && (right->number & (right->number - 1)) == 0) {
                    
                    loadValueToA(binaryExpression->left, parent);
                    long long n=1;
                    while(n!=right->number){
                        assembler.shlInstr("a");
                        n=n*2;
                    }
                }
                else{
                    loadValueToA(binaryExpression->right, parent);
                    assembler.putInstr("e");
                    loadValueToA(binaryExpression->left, parent);
                    multiplyLogarithmically();
                }

            }
            else if(auto left = dynamic_cast<NumberValue*>(&binaryExpression->left) ){
                if(left->number && (left->number & (left->number - 1)) == 0) {
                    loadValueToA(binaryExpression->right, parent);
                    long long n=0;
                    while(n!=left->number){
                        assembler.shlInstr("a");
                        n=n*2;
                    }
                }
                else{
                    loadValueToA(binaryExpression->right, parent);
                    assembler.putInstr("e");
                    loadValueToA(binaryExpression->left, parent);
                    multiplyLogarithmically();
                }
            }
            else{
                loadValueToA(binaryExpression->right, parent); 
                assembler.putInstr("e"); 
                loadValueToA(binaryExpression->left, parent);
                multiplyLogarithmically();

            }
        }
        else if(binaryExpression->operation==mod){
            loadValueToA(binaryExpression->right, parent); 
            assembler.putInstr("e"); 
            loadValueToA(binaryExpression->left, parent);
            moduloLogagarithmically();

        }
        else{
            loadValueToA(binaryExpression->right, parent);
            assembler.putInstr("e"); 
            loadValueToA(binaryExpression->left, parent); 
                if (binaryExpression->operation == add) {
                    assembler.addInstr("e"); 
                } 

                else if(binaryExpression->operation == sub){
                    assembler.subInstr("e");
                }
        }
    }
    
return 0;   
}


long long CodeGenerator::loadValueToA(Value& value,  std::string parent) {
    if (auto n = dynamic_cast<NumberValue*>(&value)) {
        setRegister("a", n->number);
    } else {
        auto v = dynamic_cast<StringValue*>(&value);

        if (auto i = dynamic_cast<VariableIdentifier*>(&v->stringValue)) {
            loadVarAddressToA(i->name, parent);
            assembler.loadInstr("a");


        } else if (auto m = dynamic_cast<ArrayAccessIdentifier*>(&v->stringValue)) {
            long long index = m->index;
            loadArrayToA(m->name, index,parent);
            assembler.loadInstr("a");


        } else {
            auto j = dynamic_cast<VariableArrayAccessIdentifier*>(&v->stringValue);
            loadAccesArrayToA(j->name,j->accessName,parent);
            assembler.loadInstr("a");
            

        }
    }

    return 0;
}



///PROCEDURES
void CodeGenerator::generateCodeProcedure(Procedure& procedure) {
    assembler.insertToProcedureList(procedure.procHead.procedure,assembler.k);
    generateCodeProcHead(procedure.procHead,procedure.procHead.procedure);
    generateCodeDeclarations(procedure.declarations, procedure.procHead.procedure);
    generateCodeCommands(procedure.commands, procedure.procHead.procedure);

    long long procAddress = memory.getAddressForIdentifier("",procedure.procHead.procedure);
    getFromMemory(procAddress);
    setRegister("c", procAddress);
    assembler.addInstr("c");
    assembler.incInstr("a");
    assembler.incInstr("a");
    assembler.incInstr("a");
    assembler.incInstr("a");
    assembler.jumprInstr("a");
}

void CodeGenerator::generateCodeProcHead(ProcHead& procHead, std::string parent) {
    std::string name = procHead.procedure;
    for(auto arg : procHead.arguments.varDeclarations){
        memory.insertToMemory(memory.getMemoryPointer(),procHead.procedure,arg.first,true);
    }
}

void CodeGenerator::generateCodeProcCallCommand(ProcCallCommand& procCallCommand, std::string parent) {
    int i=0;
    std::vector<std::string> args = memory.getItemsByParent(procCallCommand.procCall.procedure);
    long long size = procCallCommand.procCall.arguments.identifiers.size();
    for(std::string arg : procCallCommand.procCall.arguments.identifiers){
        if(isNumber(arg)){
            setRegister("a",std::stoll(arg));
            memory.setArgReference(procCallCommand.procCall.procedure, args[i], false);
            long long procArgAddress = memory.getAddressForIdentifier(procCallCommand.procCall.procedure, args[i]);
            saveInMemory(procArgAddress);

        }
        else{
            getAddressFromArguments(arg,parent); 
            long long procArgAddress = memory.getAddressForIdentifier(procCallCommand.procCall.procedure, args[i]);
            saveInMemory(procArgAddress);

        }
        i++;
    }
    long long procAddress = memory.getAddressForIdentifier("",procCallCommand.procCall.procedure);
    assembler.strkInstr("a");
    saveInMemory(procAddress);
    assembler.jumpInstr(procCallCommand.procCall.procedure);

}


void CodeGenerator::getFromMemory(long long address){
    setRegister("b",address);
    assembler.loadInstr("b");
}

void CodeGenerator::saveInMemory(long long address){
    setRegister("b",address);
    assembler.storeInstr("b");
}


void CodeGenerator::getAddressFromReference(std::string name,std::string parent){
    long long address = memory.getAddressForIdentifier(parent, name);
    getFromMemory(address);
}


void CodeGenerator::getValueFromReference(std::string name,std::string parent){
    getAddressFromReference(name, parent);
    assembler.loadInstr("a");
}


void CodeGenerator::loadVarAddressToA(std::string name, std::string parent) {
    long long address;

        if(memory.isArgReference(parent,name)){
            getAddressFromReference(name,parent); 
        }else{
            address = memory.getAddressForIdentifier(parent, name);
            setRegister("a",address);
        }
    }

    void CodeGenerator::loadArrayToA(std::string name, long long index, std::string parent){
        long long address;
        if(memory.isArgReference(parent,name)){
            setRegister("c", index);
            getAddressFromReference(name,parent);
            assembler.addInstr("c");

        }
        else{
            address = memory.getAddressForIdentifier(parent,name) + index;
            setRegister("a", address);

        }
    }
    

    void CodeGenerator::loadAccesArrayToA(std::string name, std::string accessName, std::string parent){
        long long address;
        if(memory.isArgReference(parent,name)){
            if(memory.isArgReference(parent, accessName)){

                getAddressFromReference(accessName, parent); 
                assembler.loadInstr("a"); 
                assembler.putInstr("c");
                getAddressFromReference(name,parent);
                assembler.addInstr("c");
            }
            else{

                long long accessAddress = memory.getAddressForIdentifier(parent,accessName);
                setRegister("c", accessAddress);
                assembler.loadInstr("c");
                assembler.putInstr("c");
                getAddressFromReference(name,parent);
                assembler.addInstr("c"); 

            }
        }
        else{

            if(memory.isArgReference(parent, accessName)){
                getAddressFromReference(accessName, parent); 
                assembler.loadInstr("a");
                assembler.putInstr("c");
                address = memory.getAddressForIdentifier(parent,name);
                setRegister("a", address);
                assembler.addInstr("c");
            }
            else{
                long long accessAddress = memory.getAddressForIdentifier(parent,accessName);
                setRegister("c", accessAddress);
                assembler.loadInstr("c");
                assembler.putInstr("c");
                address = memory.getAddressForIdentifier(parent,name);
                setRegister("a", address);
                assembler.addInstr("c");
            }
        }
    }

void CodeGenerator::getAddressFromArguments(std::string identifier, std::string parent) {
    if (isString(identifier)) {
        std::smatch match;
        if (isArray(identifier) && std::regex_match(identifier, match, std::regex("^\\w+\\[(\\d+)\\]$"))) {
            long long index = std::stoll(match[1].str());
            loadArrayToA(identifier, index, parent);
        } else if (isArrayWithIndex(identifier) && std::regex_match(identifier, match, std::regex("^\\w+\\[(\\w+)\\]$"))) {
            std::string accesName = match[1].str();
            loadAccesArrayToA(identifier, accesName, parent);
        } else {
            loadVarAddressToA(identifier, parent);
        }
    } 
}

    bool CodeGenerator::isNumber(const std::string& str) {
        return std::all_of(str.begin(), str.end(), ::isdigit);
    }

    bool CodeGenerator::isString(const std::string& str) {
        return std::all_of(str.begin(), str.end(), ::isalpha);
    }

    bool CodeGenerator::isArray(const std::string& str) {
        std::regex pattern("^\\w+\\[\\d+\\]$");
        return std::regex_match(str, pattern);
    }

    bool CodeGenerator::isArrayWithIndex(const std::string& str) {
        std::regex pattern("^\\w+\\[\\w+\\]$");
        return std::regex_match(str, pattern);
    }


