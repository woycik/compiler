#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>



class InstVector {
public:

    std::vector<long long> instructionPointer;
    int lastIndex=-1;

    int currentIndex=-1;
    InstVector() {};

    void insertToVector() {
        instructionPointer.push_back(-1);
        lastIndex++;
        currentIndex = lastIndex;
    }

    void setValue(long long value) {
        for(int i = lastIndex;i>=0;i--){
            if(instructionPointer[i]==-1){
                instructionPointer[i]=value;
                break;
            }
        }
    }



    long long getValue(long long pointer){
        return instructionPointer[pointer];
    }

    void incVectorValue(){
        instructionPointer[lastIndex]++;
    }
};

class AsmInstructions {
public:
    std::vector<std::string> instructionList;
    std::vector<std::pair<std::string, long long>> procedureList;
    std::vector<std::pair<std::string, long long>> returnList;

    long long k;

    InstVector instr;

    AsmInstructions() : k(0) {}

    void readInstr();
    void writeInstr();
    void loadInstr(std::string label);
    void storeInstr(std::string label);
    void addInstr(std::string label);
    void subInstr(std::string label);
    void getInstr(std::string label);
    void putInstr(std::string label);
    void rstInstr(std::string label);
    void incInstr(std::string label);
    void decInstr(std::string label);
    void shlInstr(std::string label);
    void shrInstr(std::string label);
    void jumpInstr(std::string label);
    void jposInstr(std::string label);
    void jzeroInstr(std::string label);
    void strkInstr(std::string label);
    void jumprInstr(std::string label);
    void haltInstr();

void insertToProcedureList(std::string name,long long value){
    procedureList.push_back(std::make_pair(name,value));
}

long long getProcedureList(std::string name) {
    for (auto f : procedureList) {
        if (f.first == name) {
            return f.second;
        }
    }
    return -1;
}

void insertToReturnList(std::string name,long long value){
    returnList.push_back(std::make_pair(name,value));
}

long long getReturnListValue(std::string name) {
    for (auto it = returnList.rbegin(); it != returnList.rend(); ++it) {
        if (it->first == name) {
            long long value = it->second;
            returnList.erase(std::prev(it.base()));
            return value;
        }
    }
    return -1;
}




void replaceValues() {
    int index=0;
    for (std::string &inst : instructionList) {
        if (inst == "JUMP -1" || inst == "JPOS -1" || inst == "JZERO -1") {
            std::istringstream iss(inst);
            std::string newInstr;
            iss >> newInstr;

            newInstr = newInstr + " " + std::to_string(instr.getValue(index));
            inst = newInstr;  // Update the specific element in instructionList
            index++;
        }

        else if (inst.substr(0, 5) == "JUMP ") {
            long long dest = getProcedureList(inst.substr(5));
            bool containsDigit = std::any_of(inst.begin() + 4, inst.end(), [](char c) {
                return std::isdigit(static_cast<unsigned char>(c));
                });  
            if(!containsDigit){              
                if(dest !=-1){
                    std::string newInstr = "JUMP " + std::to_string(dest);
                    inst = newInstr;
                }
                else{
                    dest = getReturnListValue(inst.substr(5));
                    std::string newInstr = "JUMP " + std::to_string(dest);
                    inst = newInstr;
                }
            }
        }
    }
}

};
