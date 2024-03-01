%{
    #include <iostream>
    #include <string>
    #include <vector>
    
    extern int yylex();
    extern int yyparse();
    void yyerror(const char *s);
    extern int yylineno;
    std::vector<std::string> commandList = {};

%}


%token <token> END IF THEN ELSE ENDIF 
%token <token> WHILE DO ENDWHILE TO IS UNTIL PROCEDURE REPEAT IN PROGRAM
%token <token> READ WRITE
%token <token> PLUS MINUS MULT DIV MOD
%token <token> EQ LE GE EXCLAMATION
%token <token> RBRACKET LBRACKET COLON SEMICOLON COMMA LQBRACKET RQBRACKET T
%token <token> ERROR
%token <number> num
%token <pIdentifier> pidentifier   
%type <identifier_> identifier
%type <value_> value
%type <condition_> condition
%type <expression_> expression
%type <args_> args
%type <argsDecl_> args_decl
%type <declarations_> declarations
%type <ProcCall_> proc_call
%type <ProcHead_> proc_head
%type <Command_> command
%type <Commands_> commands
%type <Main_> main
%type <Procedures_> procedures
%type <ProgramAll_> program_all

%union {
    int token;
    Identifier *identifier_;
    Value* value_;
    Expression* expression_;
    Condition* condition_;
    Args* args_;
    ArgsDecl* argsDecl_;
    Declarations* declarations_;
    ProcCall* ProcCall_;
    ProcHead* ProcHead_;
    Command* Command_;
    Commands* Commands_;
    Procedures* Procedures_;
    Main* Main_;
    long long number;
    std::string *pIdentifier;
    ProgramAll* ProgramAll_;

}

%code requires {
    #include "./front/ast.hpp"
    #include "./front/SemanticAnalyzer.hpp"
    #include "./back/CodeGenerator.hpp"
}

%%
program_all  : procedures main {
    $$ = new ProgramAll(*$1,*$2);
    SemanticAnalyzer sa;
    CodeGenerator codeGen;
    if(!sa.analyze(*$$)){
        codeGen.generateCode(*$$);
        commandList = codeGen.getCommandList();
    }

    };

procedures   : procedures PROCEDURE proc_head IS declarations IN commands END
             { auto proc = new Procedure(*$3, *$5, *$7);
               $1->addProcedure(proc); }
             | procedures PROCEDURE proc_head IS IN commands END
              { auto proc = new Procedure(*$3, *new Declarations(yylineno), *$6); 
              $1->addProcedure(proc); }
             |{ $$ = new Procedures(); }
             ;


main         : PROGRAM IS declarations IN commands END {$$ = new Main(*$3,*$5);}
             | PROGRAM IS IN commands END {$$ = new Main( *new Declarations(yylineno),*$4);}
             ;

commands     : commands command { $1->addToCommands($2);}
             | command {$$ =new  Commands(); $$->addToCommands($1);}
             ;

command      : identifier COLON EQ expression SEMICOLON {$$ =new  AssignmentCommand(*$1,*$4,yylineno);}
             | IF condition THEN commands ELSE commands ENDIF {$$ = new IfCommand(*$2,*$4,*$6,yylineno);}
             | IF condition THEN commands ENDIF {$$ = new IfCommand(*$2,*$4, *new Commands(),yylineno);}
             | WHILE condition DO commands ENDWHILE {$$ =new  WhileCommand(*$2,*$4,yylineno);}
             | REPEAT commands UNTIL condition SEMICOLON {$$ = new RepeatCommand(*$2,*$4,yylineno);}
             | proc_call SEMICOLON {$$ = new ProcCallCommand(*$1,yylineno);}
             | READ identifier SEMICOLON {$$ = new ReadCommand(*$2,yylineno);}
             | WRITE value SEMICOLON {$$ = new WriteCommand(*$2,yylineno);}
             ;

proc_head    : pidentifier LBRACKET args_decl RBRACKET {
    $$ =  new ProcHead(*$1,*$3,yylineno);
}

proc_call    : pidentifier LBRACKET args RBRACKET {$$ =new ProcCall(*$1,*$3,yylineno);}

declarations : declarations COMMA pidentifier {$1->addToVarDecl(*$3);}
             | declarations COMMA pidentifier LQBRACKET num RQBRACKET {$1->addToTabDecl(*$3,$5);}
             | pidentifier {$$ =new Declarations(yylineno); $$->addToVarDecl(*$1);}
             | pidentifier LQBRACKET num RQBRACKET {$$ =new Declarations(yylineno); $$->addToTabDecl(*$1,$3);}
             ;

args_decl    : args_decl COMMA pidentifier  {$1->addToVarDecl(*$3,0);}
             | args_decl COMMA T pidentifier {$1->addToVarDecl(*$4,1);}
             | pidentifier {$$ = new ArgsDecl(yylineno); $$->addToVarDecl(*$1,0); }
             | T pidentifier {$$ = new ArgsDecl(yylineno); $$->addToVarDecl(*$2,1);}
             ;

args         : args COMMA pidentifier {$1->addToArgs(*$3);}
             | pidentifier {$$ = new Args(yylineno);$$->addToArgs(*$1);}
             ;

expression   : value    {$$ = new ValueExpression(*$1,yylineno);}   
             | value PLUS value {$$ = new MathExpression(*$1,*$3,add,yylineno);}
             | value MINUS value {$$ = new MathExpression(*$1,*$3,sub,yylineno);}
             | value MULT value {$$ = new MathExpression(*$1,*$3,mult,yylineno);}
             | value DIV value {$$ = new MathExpression(*$1,*$3,divi,yylineno);}
             | value MOD value {$$ = new MathExpression(*$1,*$3,mod,yylineno);}
             ;


condition    : value EQ value {$$ = new Condition(*$1,*$3,eq,yylineno);}
             | value EXCLAMATION EQ value {$$ = new Condition(*$1,*$4,neq,yylineno);}
             | value GE value {$$ = new Condition(*$1,*$3,g,yylineno);}
             | value LE value {$$ = new Condition(*$1,*$3,l,yylineno);}
             | value GE EQ  value {$$ = new Condition(*$1,*$4,geq,yylineno);}
             | value LE EQ value {$$ = new Condition(*$1,*$4,leq, yylineno);}
             ;


value        : num {$$ = new NumberValue($1);}
             | identifier {$$ = new StringValue(*$1);}
             ;

identifier   :  pidentifier { $$ = new VariableIdentifier(*$1); }
             | pidentifier LQBRACKET num RQBRACKET { $$ = new ArrayAccessIdentifier(*$1, $3); }
             | pidentifier LQBRACKET pidentifier RQBRACKET {$$ = new VariableArrayAccessIdentifier(*$1, *$3);}
             ;
%%

void yyerror(char const *s) {
        std::cout << s << " - unknown token (line " << yylineno<< ")" << std::endl;
        exit(0);
}
