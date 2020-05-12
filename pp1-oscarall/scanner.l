/* File:  scanner.l
 * ----------------
 * Lex inupt file to generate the scanner for the compiler.
 */

%{

/* The text within this first region delimited by %{ and %} is assumed to
 * be C/C++ code and will be copied verbatim to the lex.yy.c file ahead
 * of the definitions of the yylex() function. Add other header file inclusions
 * or C++ variable declarations/prototypes that are needed by your code here.
 */

#include <string.h>
#include "scanner.h"
#include "utility.h" // for PrintDebug()
#include "errors.h"

/* Global variable: yylval
 * -----------------------
 * This global variable is how we get attribute information about the token
 * just scanned to the client. The scanner sets the global variable
 * appropriately and since it's global the client can just read it.  In the
 * future, this variable will be declared for us in the y.tab.c file
 * produced by Yacc, but for now, we declare it manually.
 */
YYSTYPE yylval;  // manually declared for pp1, later Yacc provides

/* Global variable: yylloc
 * -----------------------
 * This global variable is how we get position information about the token
 * just scanned to the client. (Operates similarly to yylval above)
 */
struct yyltype yylloc; // manually dclared for pp1, later Yacc provides

/* Macro: YY_USER_ACTION 
 * ---------------------
 * This flex built-in macro can be defined to provide an action which is
 * always executed prior to any matched rule's action. Basically, it is
 * a way of having a piece of code common to all actions factored out to
 * this routine.  We already defined it for you and left the empty
 * function DoBeforeEachAction ready for your use as needed. It will
 * be called once for each pattern scanned from the file, before
 * executing its action.
 */
static void DoBeforeEachAction(); 
void incrementColumns(char *);
#define YY_USER_ACTION DoBeforeEachAction();

%}

 /* The section before the first %% is the Definitions section of the lex
  * input file. Here is where you set options for the scanner, define lex
  * states, and can set up definitions to give names to regular expressions
  * as a simple substitution mechanism that allows for more readable
  * entries in the Rules section later. 
  */


%%             /* BEGIN RULES SECTION *
\"([^\\"\n]|\\\")*\" { yylval.stringConstant = yytext;
                     return T_StringConstant; }
\"[^"\n]* {
            ReportError::UntermString(&yylloc, yytext);
          }
int {  
      return T_Int;
    }
void  {  
        return T_Void;
      }
string  {  
          return T_String;
        }
class {  
        return T_Class;
      }
this  {  
        return T_This;
      }
extends {  
          return T_Extends;
        }
while {  
        return T_While;
      } 
if  {  
      return T_If;
    }
break {  
        return T_Break;
      }
New {  
      return T_New;
    }
double  {  
          return T_Double;
        }
bool  {  
        return T_Bool;
      }
interface {  
            return T_Interface;
          }
null  {  
        return T_Null;
      }
implements  {  
              return T_Implements;
            }
for {  
      return T_For;
    }
else  {  
        return T_Else;
      }
return  {  
          return T_Return;
        }
NewArray  {  
            return T_NewArray;
          }
true  { 
        yylval.boolConstant = true; 
        return T_BoolConstant;
      }
false  { 
        yylval.boolConstant = false; 
        return T_BoolConstant;
      }
[a-zA-Z][a-zA-Z0-9_]* { 
                        strncpy(yylval.identifier, yytext, 31);
                        yylval.identifier[31] = '\0';
                        if (strcmp(yytext, yylval.identifier)) {
                          ReportError::LongIdentifier(&yylloc, yytext);
	                      }
                        return T_Identifier;
                      }
0[xX][0-9a-fA-F]+ { 
                    yylval.integerConstant = (int) strtol(yytext, NULL, 16); 
                    return T_IntConstant;
                  }
[0-9]+\.[0-9]*([Ee](\+|\-)?[0-9]+)?  { 
                                      yylval.doubleConstant = strtod(yytext, NULL); 
                                      return T_DoubleConstant;
                                    }
[0-9]+  { 
          yylval.integerConstant = atoi(yytext); 
          return T_IntConstant;
        }
\=\=  { 
        return T_Equal;
      }
\!\=  { 
        return T_NotEqual;
      }
\&\&  {
        return T_And;
      }
\|\|  { 
        return T_Or;
      }
\>\=  { 
        return T_GreaterEqual;
      }
\<\=  { 
        return T_LessEqual;
      }
[+\-*/%<>=!;,.[\](){}]  { 
                          return (int) yytext[0];
                        }
\n  { yylloc.first_line++; yylloc.first_column = 1; yylloc.last_column = 0; }
" " { }
. { 
    ReportError::UnrecogChar(&yylloc, yytext[0]);
  }
%%
/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated lex.yy.c file.
 * This section is where you put definitions of helper functions.
 */

void incrementColumns(char *yytext) {
  yylloc.first_column = yylloc.last_column + 1;
  yylloc.last_column += strlen(yytext);
}


/* Function: InitScanner
 * ---------------------
 * This function will be called before any calls to yylex().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the scanner (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yy_flex_debug that controls whether flex prints debugging information
 * about each token and what rule was matched. If set to false, no information
 * is printed. Setting it to true will give you a running trail that might
 * be helpful when debugging your scanner. Please be sure the variable is
 * set to false when submitting your final version.
 */
void InitScanner()
{
    PrintDebug("lex", "Initializing scanner");
    yylloc.first_line = 1;
    yylloc.first_column = 1;
    yy_flex_debug = false;
}


/* Function: DoBeforeEachAction()
 * ------------------------------
 * This function is installed as the YY_USER_ACTION. This is a place
 * to group code common to all actions.
 */
static void DoBeforeEachAction()
{
  incrementColumns(yytext);
}
