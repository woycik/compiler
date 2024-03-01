#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <vector>
#include <string>


class ASTNode {
public:
    virtual ~ASTNode() {}
};

class Identifier : public ASTNode {
public:
    virtual ~Identifier() {}
    virtual std::string getName() const = 0;
};

class VariableIdentifier : public Identifier {
public:
    std::string name;

    VariableIdentifier(const std::string& name) : name(name) {}

    std::string getName() const override {
        return name;
    }
};

class ArrayAccessIdentifier : public Identifier {
public:
    long long index;
    std::string name;

    ArrayAccessIdentifier(const std::string& name, long long index) : name(name), index(index) {}

    std::string getName() const override {
        return name;
    }
};

class VariableArrayAccessIdentifier : public Identifier {
public:
    std::string accessName;
    std::string name;

    VariableArrayAccessIdentifier(const std::string& name, const std::string& accessName)
        : name(name), accessName(accessName) {}

    std::string getName() const override {
        return name;
    }
};

// Value
class Value : public ASTNode {
public:
    virtual ~Value() {}
};

class NumberValue : public Value {
public:
    long long number;

    NumberValue(long long number) : number(number) {}
};

class StringValue : public Value {
public:
    Identifier& stringValue;

    StringValue(Identifier& stringValue) : stringValue(stringValue) {}

    ~StringValue() {
        delete &stringValue;
    }
};

// Conditions

enum ConditionType {
    eq, neq, geq, leq, l, g
};

class Condition : public ASTNode {
public:
    Value& left;
    Value& right;
    ConditionType operation;
    long long lineNumber;

    Condition(Value& left, Value& right, ConditionType operation, long long lineNumber)
        : left(left), right(right), operation(operation), lineNumber(lineNumber) {}

    ~Condition() {
        delete &left;
        delete &right;
    }
};

// Expression

enum ExpressionType {
    add, sub, divi, mult, mod
};

class Expression : public ASTNode {
public:
    virtual ~Expression() {}
};

class MathExpression : public Expression {
public:
    Value& left;
    Value& right;
    ExpressionType operation;
    long long lineNumber;

    MathExpression(Value& left, Value& right, ExpressionType operation, long long lineNumber)
        : left(left), right(right), operation(operation), lineNumber(lineNumber) {}

    ~MathExpression() {
        delete &left;
        delete &right;
    }
};

class ValueExpression : public Expression {
public:
    Value& value;
    long long lineNumber;

    ValueExpression(Value& value, long long lineNumber) : value(value), lineNumber(lineNumber) {}

    ~ValueExpression() {
        delete &value;
    }
};

// Args
class Args : public ASTNode {
public:
    std::vector<std::string> identifiers;
    long long lineNumber;

    Args(long long lineNumber) : lineNumber(lineNumber) {}

    void addToArgs(std::string& arg) {
        identifiers.push_back(arg);
    }

};

// ArgsDecl
class ArgsDecl : public ASTNode {
public:
    std::vector<std::pair<std::string, long long>> varDeclarations;
    long long lineNumber;

    ArgsDecl(long long lineNumber) : lineNumber(lineNumber) {}

    void addToVarDecl(const std::string& name, long long value) {
        varDeclarations.push_back(std::make_pair(name, value));
    }

};

// Declarations
class Declarations : public ASTNode {
public:
    std::vector<std::string> varDeclarations;
    std::vector<std::pair<std::string, long long>> tabDeclarations;
    long long lineNumber;

    Declarations(long long lineNumber)
        : lineNumber(lineNumber) {}

    void addToVarDecl(const std::string& argDecl) {
        varDeclarations.push_back(argDecl);
    }

    void addToTabDecl(const std::string& argDecl, const long long length) {
        tabDeclarations.push_back({argDecl, length});
    }

};

// ProcCall
class ProcCall : public ASTNode {
public:
    const std::string& procedure;
    Args& arguments;
    long long lineNumber;

    ProcCall(std::string& procedure, Args& arguments, long long lineNumber)
        : procedure(procedure), arguments(arguments), lineNumber(lineNumber) {}

    ~ProcCall() {
        delete &arguments;
    }
};

// ProcHead
class ProcHead : public ASTNode {
public:
    const std::string& procedure;
    ArgsDecl& arguments;
    long long lineNumber;

    ProcHead(std::string& procedure, ArgsDecl& arguments, long long lineNumber)
        : procedure(procedure), arguments(arguments), lineNumber(lineNumber) {}

    ~ProcHead() {
        delete &arguments;
    }
};

// Command
class Command : public ASTNode {
public:
    virtual ~Command() {}
};

class Commands : public ASTNode {
public:
    std::vector<Command*> commands;

    Commands() {}

    void addToCommands(Command* com) {
        commands.push_back(com);
    }

    ~Commands() {
        for (auto& command : commands) {
            delete command;
        }
    }
};

class AssignmentCommand : public Command {
public:
    Identifier& identifier;
    Expression& expression;
    long long lineNumber;

    AssignmentCommand(Identifier& identifier, Expression& expression, long long lineNumber)
        : identifier(identifier), expression(expression), lineNumber(lineNumber) {}

    ~AssignmentCommand() {
        delete &identifier;
        delete &expression;
    }
};

class IfCommand : public Command {
public:
    Condition& condition;
    Commands& trueBranch;
    Commands& falseBranch;
    long long lineNumber;

    IfCommand(Condition& condition, Commands& trueBranch, Commands& falseBranch, long long lineNumber)
        : condition(condition), trueBranch(trueBranch), falseBranch(falseBranch), lineNumber(lineNumber) {}

    ~IfCommand() {
        delete &condition;
        delete &trueBranch;
        delete &falseBranch;
    }
};

class WhileCommand : public Command {
public:
    Condition& condition;
    Commands& body;
    long long lineNumber;

    WhileCommand(Condition& condition, Commands& body, long long lineNumber)
        : condition(condition), body(body), lineNumber(lineNumber) {}

    ~WhileCommand() {
        delete &condition;
        delete &body;
    }
};

class RepeatCommand : public Command {
public:
    Condition& condition;
    Commands& body;
    long long lineNumber;

    RepeatCommand(Commands& body, Condition& condition, long long lineNumber)
        : condition(condition), body(body), lineNumber(lineNumber) {}

    ~RepeatCommand() {
        delete &condition;
        delete &body;
    }
};

class ProcCallCommand : public Command {
public:
    ProcCall& procCall;
    long long lineNumber;

    ProcCallCommand(ProcCall& procCall, long long lineNumber)
        : procCall(procCall), lineNumber(lineNumber) {}

    ~ProcCallCommand() {
        delete &procCall;
    }
};

class ReadCommand : public Command {
public:
    Identifier& identifier;
    long long lineNumber;

    ReadCommand(Identifier& identifier, long long lineNumber)
        : identifier(identifier), lineNumber(lineNumber) {}

    ~ReadCommand() {
        delete &identifier;
    }
};

class WriteCommand : public Command {
public:
    Value& value;
    long long lineNumber;

    WriteCommand(Value& value, long long lineNumber)
        : value(value), lineNumber(lineNumber) {}

    ~WriteCommand() {
        delete &value;
    }
};

// Main
class Main : public ASTNode {
public:
    Declarations& declarations;
    Commands& commands;

    Main(Declarations& declarations, Commands& commands)
        : declarations(declarations), commands(commands) {}

    ~Main() {
        delete &declarations;
        delete &commands;
    }
};

// Procedures
class Procedure : public ASTNode {
public:
    ProcHead& procHead;
    Declarations& declarations;
    Commands& commands;

    Procedure(ProcHead& procHead, Declarations& declarations, Commands& commands)
        : procHead(procHead), declarations(declarations), commands(commands) {}

    ~Procedure() {
        delete &procHead;
        delete &declarations;
        delete &commands;
    }
};

class Procedures : public ASTNode {
public:
    std::vector<Procedure*> procedures;

    Procedures() {}

    void addProcedure(Procedure* proc) {
        procedures.push_back(proc);
    }

    ~Procedures() {
        for (auto& proc : procedures) {
            delete proc;
        }
    }
};

// Program
class ProgramAll : public ASTNode {
public:
    Procedures& procedures;
    Main& main;

    ProgramAll(Procedures& procedures, Main& main)
        : procedures(procedures), main(main) {}

    ~ProgramAll() {
        delete &procedures;
        delete &main;
    }
};

#endif
