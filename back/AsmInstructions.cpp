#include "AsmInstruction.hpp"


// READ
void AsmInstructions::readInstr() {
    std::string inst = "READ";
    instructionList.push_back(inst);
    k++;
}

// WRITE
void AsmInstructions::writeInstr() {
    std::string inst = "WRITE";
    instructionList.push_back(inst);
    k++;

}

// LOAD
void AsmInstructions::loadInstr(std::string label) {
    std::string inst = "LOAD " + label;
    instructionList.push_back(inst);
    k++;

}

// STORE
void AsmInstructions::storeInstr(std::string label) {
    std::string inst = "STORE " + label;
    instructionList.push_back(inst);
    k++;
}

// ADD
void AsmInstructions::addInstr(std::string label) {
    std::string inst = "ADD "+label;
    instructionList.push_back(inst);
    k++;
}

// SUB
void AsmInstructions::subInstr(std::string label) {
    std::string inst = "SUB " + label;
    instructionList.push_back(inst);
    k++;
}

// GET
void AsmInstructions::getInstr(std::string label) {
    std::string inst = "GET "+ label ;
    instructionList.push_back(inst);
    k++;
}

// PUT
void AsmInstructions::putInstr(std::string label) {
    std::string inst = "PUT "+ label;
    instructionList.push_back(inst);
    k++;
}

// RST
void AsmInstructions::rstInstr(std::string label) {
    std::string inst = "RST " + label;
    instructionList.push_back(inst);
    k++;
}

// INC
void AsmInstructions::incInstr(std::string label) {
    std::string inst = "INC "+label;
    instructionList.push_back(inst);
    k++;
}

// DEC
void AsmInstructions::decInstr(std::string label) {
    std::string inst = "DEC "+ label;
    instructionList.push_back(inst);
    k++;
}

// SHL
void AsmInstructions::shlInstr(std::string label) {
    std::string inst = "SHL "+ label;
    instructionList.push_back(inst);
    k++;
}

// SHR
void AsmInstructions::shrInstr(std::string label) {
    std::string inst = "SHR "+ label;
    instructionList.push_back(inst);
    k++;
}

// JUMP
void AsmInstructions::jumpInstr(std::string label) {
    std::string inst = "JUMP " + label;
    instructionList.push_back(inst);
    k++;
}

// JPOS
void AsmInstructions::jposInstr(std::string label) {
    std::string inst = "JPOS " + label;
    instructionList.push_back(inst);
    k++;
}

// JZERO
void AsmInstructions::jzeroInstr(std::string label) {
    std::string inst = "JZERO " + label;
    instructionList.push_back(inst);
    k++;
}

// STRK
void AsmInstructions::strkInstr(std::string label) {
    std::string inst = "STRK "+label;
    instructionList.push_back(inst);
    k++;
}

// JUMPR
void AsmInstructions::jumprInstr(std::string reg) {
    std::string inst = "JUMPR " + reg;
    instructionList.push_back(inst);
    k++;
}

// HALT
void AsmInstructions::haltInstr() {
    std::string inst = "HALT";
    instructionList.push_back(inst);
    k++;
}
