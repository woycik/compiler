%option noyywrap
%option yylineno
%{
 #include <string>
 #include "parser.tab.h"


%}


break_line      \\\n|\n
num             ([0-9]|[1-9][0-9]*)
pidentifier     [_a-z]+
comment         #.*$



%%
{comment}                   ;
{break_line}                ;
[[:blank:]]+                ;
{pidentifier}               {yylval.pIdentifier =  new std::string(yytext, yyleng); return pidentifier;}
{num}                       {yylval.number = atoll(yytext);return num;}

END             {return yylval.token=END;}
IF              {return yylval.token=IF;}
THEN            {return yylval.token=THEN;}
ELSE            {return yylval.token=ELSE;}
ENDIF           {return yylval.token=ENDIF;}
WHILE           {return yylval.token=WHILE;}
DO              {return yylval.token=DO;}
ENDWHILE        {return yylval.token=ENDWHILE;}
TO              {return yylval.token=TO;}
READ            {return yylval.token=READ;}
WRITE           {return yylval.token=WRITE;}
IN              {return yylval.token=IN;}
IS              {return yylval.token=IS;}
UNTIL           {return yylval.token=UNTIL;}
REPEAT          {return yylval.token=REPEAT;}
PROCEDURE       {return yylval.token=PROCEDURE;}
PROGRAM         {return yylval.token=PROGRAM;}
T               {return yylval.token=T;}
\+              {return yylval.token=PLUS;}
\-              {return yylval.token=MINUS;}
\*              {return yylval.token=MULT;}
\/              {return yylval.token=DIV;}
\%              {return yylval.token=MOD;}

\=              {return yylval.token=EQ;}
\<              {return yylval.token=LE;}
\>              {return yylval.token=GE;}
\!              {return yylval.token=EXCLAMATION;}


\(              {return yylval.token=LBRACKET;}
\)              {return yylval.token=RBRACKET;}
\[              {return yylval.token=LQBRACKET;}
\]              {return yylval.token=RQBRACKET;}
\:              {return yylval.token=COLON;}
\;              {return yylval.token=SEMICOLON;}
\,              {return yylval.token=COMMA;}
.               {return yylval.token=ERROR;}

%%


