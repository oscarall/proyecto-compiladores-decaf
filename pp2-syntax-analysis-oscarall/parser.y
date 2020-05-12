/* File: parser.y
 * --------------
 * Yacc input file to generate the parser for the compiler.
 *
 * pp2: your job is to write a parser that will construct the parse tree
 *      and if no parse errors were found, print it.  The parser should 
 *      accept the language as described in specification, and as augmented 
 *      in the pp2 handout.
 */

%{

/* Just like lex, the text within this first region delimited by %{ and %}
 * is assumed to be C/C++ code and will be copied verbatim to the y.tab.c
 * file ahead of the definitions of the yyparse() function. Add other header
 * file inclusions or C++ variable declarations/prototypes that are needed
 * by your code here.
 */
#include "scanner.h" // for yylex
#include "parser.h"
#include "errors.h"

void yyerror(char *msg); // standard error-handling routine

%}

/* The section before the first %% is the Definitions section of the yacc
 * input file. Here is where you declare tokens and types, add precedence
 * and associativity options, and so on.
 */
 
/* yylval 
 * ------
 * Here we define the type of the yylval global variable that is used by
 * the scanner to store attibute information about the token just scanned
 * and thus communicate that information to the parser. 
 *
 * pp2: You will need to add new fields to this union as you add different 
 *      attributes to your non-terminal symbols.
 */
%union {
    int integerConstant;
    bool boolConstant;
    char *stringConstant;
    double doubleConstant;
    char identifier[MaxIdentLen+1]; // +1 for terminating null
    Decl *decl;
    Type *type;
    VarDecl *varDecl;
    List<Decl*> *declList;
    Identifier *ident;
    FnDecl *fnDecl;
    List<VarDecl*> *formals;
    StmtBlock *stmtBlock;
    List<Stmt*> *stmtList;
    LValue *lValue;
    Expr *expr;
    Stmt *stmt;
    List<Expr*> *exprList;
    Call *call;
    ReturnStmt *returnStmt;
    IfStmt *ifStmt;
    PrintStmt *printStmt;
    WhileStmt *whileStmt;
    ForStmt *forStmt;
    BreakStmt *breakStmt;
    NamedType *namedType;
    List<NamedType*> *implements;
    ClassDecl *classDecl;
    InterfaceDecl *interfaceDecl;
    List<Case*> *caseList;
    Case *cas;
    Default *def;
    SwitchStmt *switchStmt;
}


/* Tokens
 * ------
 * Here we tell yacc about all the token types that we are using.
 * Yacc will assign unique numbers to these and export the #define
 * in the generated y.tab.h header file.
 */
%token   T_Void T_Bool T_Int T_Double T_String T_Class 
%token   T_LessEqual T_GreaterEqual T_Equal T_NotEqual T_Dims T_Decr T_Incr
%token   T_And T_Or T_Null T_Extends T_This T_Interface T_Implements
%token   T_While T_For T_If T_Else T_Return T_Break T_Switch T_Case T_Default
%token   T_New T_NewArray T_Print T_ReadInteger T_ReadLine

%token   <identifier> T_Identifier
%token   <stringConstant> T_StringConstant
%token   <integerConstant> T_IntConstant
%token   <doubleConstant> T_DoubleConstant
%token   <boolConstant> T_BoolConstant


%nonassoc ')'
%nonassoc T_Else

%nonassoc '='
%left T_Or
%left T_And
%nonassoc T_Equal T_NotEqual
%nonassoc T_GreaterEqual T_LessEqual '<' '>'
%left '+' '-'
%left '%' '*' '/'
%left '!' T_Incr T_Decr
%left '[' '.'
%left '('





/* Non-terminal types
 * ------------------
 * In order for yacc to assign/access the correct field of $$, $1, we
 * must to declare which field is appropriate for the non-terminal.
 * As an example, this first type declaration establishes that the DeclList
 * non-terminal uses the field named "declList" in the yylval union. This
 * means that when we are setting $$ for a reduction for DeclList ore reading
 * $n which corresponds to a DeclList nonterminal we are accessing the field
 * of the union named "declList" which is of type List<Decl*>.
 * pp2: You'll need to add many of these of your own.
 */
%type <declList>  DeclList Field FieldList PrototypeList Prototype
%type <decl>      Decl Fields PrototypeDecl
%type <varDecl>   VarDecl Variable
%type <type>      Type Void
%type <ident>     Ident
%type <fnDecl>    FuncDecl
%type <formals>   Formals VarList VarDeclList
%type <stmtBlock> StmtBlock
%type <stmtList>  StmtList
%type <lValue>    LValue
%type <expr>      Expr Constant OptExpr
%type <stmt>      Stmt
%type <exprList>  Actuals ExprList
%type <call>      Call
%type <returnStmt>ReturnStmt
%type <ifStmt>    IfStmt
%type <printStmt> PrintStmt
%type <whileStmt> WhileStmt
%type <forStmt>   ForStmt
%type <breakStmt> BreakStmt
%type <namedType> Extends
%type <implements>Implements IdenList
%type <classDecl> ClassDecl
%type <interfaceDecl>InterfaceDecl
%type <caseList>  CaseList
%type <cas>      Case
%type <def>       Default
%type <switchStmt>SwitchStmt

%%
/* Rules
 * -----
 * All productions and actions should be placed between the start and stop
 * %% markers which delimit the Rules section.
	 
 */
Program   :    DeclList            { 
                                      @1; 
                                      /* pp2: The @1 is needed to convince 
                                       * yacc to set up yylloc. You can remove 
                                       * it once you have other uses of @n*/
                                      Program *program = new Program($1);
                                      // if no errors, advance to next phase
                                      if (ReportError::NumErrors() == 0) 
                                          program->Print(0);
                                    }
          |    error                {
                                        ReportError::Formatted(&yylloc, "%s", "parse error");
                                        return 1;
                                    }
          ;

DeclList  :    Decl                 { ($$ = new List<Decl*>)->Append($1); }
          |    DeclList Decl        { ($$ = $1)->Append($2); }
          ;

Decl      :    VarDecl              { $$ = $1; }
          |    FuncDecl             { $$ = $1; }
          |    ClassDecl            { $$ = $1; }
          |    InterfaceDecl        { $$ = $1; }
          ;

InterfaceDecl: T_Interface Ident '{' Prototype '}'  { $$ = new InterfaceDecl($2, $4); }
          ;

Prototype :    PrototypeList        { $$ = $1; }
          |                         { $$ = new List<Decl*>; }
          ;

PrototypeList: PrototypeDecl        { ($$ = new List<Decl*>)->Append($1); }
          |    PrototypeList PrototypeDecl  { ($$ = $1)->Append($2); }
          ;

PrototypeDecl: Type Ident '(' Formals ')' ';' { $$ = new FnDecl($2, $1, $4); }
          |    Void Ident '(' Formals ')' ';' { $$ = new FnDecl($2, $1, $4); }
          ;

ClassDecl :    T_Class Ident Extends Implements '{' Field '}'   { $$ = new ClassDecl($2, $3, $4, $6); }
          ;

Extends   :    T_Extends Ident      { $$ = new NamedType($2); }
          |                         { $$ = NULL; }
          ;

Implements:    T_Implements IdenList    { $$ = $2; }
          |                         { $$ = new List<NamedType*>; }
          ;

IdenList  :    Ident                { ($$ = new List<NamedType*>)->Append(new NamedType($1));}
          |    IdenList ',' Ident   { ($$ = $1)->Append(new NamedType($3)); }
          ;

Field     :    FieldList            { $$ = $1; }
          |                         { $$ = new List<Decl*>; }

FieldList :    Fields               { ($$ = new List<Decl*>)->Append($1); }
          |    FieldList Fields     { ($$ = $1)->Append($2); }

Fields    :    VarDecl              { $$ = $1; }
          |    FuncDecl             { $$ = $1; }
          ;

VarDecl   :    Variable ';'         { $$ = $1; };
          ;

Variable  :    Type Ident           { $$ = new VarDecl($2, $1); }
          ;


Type      :    T_Int                { $$ = Type::intType; }
          |    T_Double             { $$ = Type::doubleType; }
          |    T_Bool               { $$ = Type::boolType; }
          |    T_String             { $$ = Type::stringType; }
          |    Ident                { $$ = new NamedType($1); }
          |    Type T_Dims          { $$ = new ArrayType(yylloc, $1); }
          ;

Ident     :    T_Identifier         { $$ = new Identifier(yylloc, $1);}
          ;

Void      :    T_Void               { $$ = Type::voidType; }
          ;

FuncDecl  :    Type Ident '(' Formals ')' StmtBlock { ($$ = new FnDecl($2, $1, $4))->SetFunctionBody($6); }
          |    Void Ident '(' Formals ')' StmtBlock { ($$ = new FnDecl($2, $1, $4))->SetFunctionBody($6); }
          ;

Formals   :    VarList              { $$ = $1; }
          |                         { $$ = new List<VarDecl*>; }
          ;


VarList   :    Variable             { ($$ = new List<VarDecl*>)->Append($1); }
          |    VarList ',' Variable { ($$ = $1)->Append($3); }
          ;


VarDeclList:   VarDecl              { ($$ = new List<VarDecl*>)->Append($1); }
           |   VarDeclList VarDecl     { ($$ = $1)->Append($2); }
           ;

StmtBlock :    '{' VarDeclList StmtList '}'  { $$ = new StmtBlock($2, $3); }
          |    '{' VarDeclList '}'  { $$ = new StmtBlock($2, new List<Stmt*>); }
          |    '{' StmtList '}'     { $$ = new StmtBlock(new List<VarDecl*>, $2); }
          |    '{' '}'              { $$ = new StmtBlock(new List<VarDecl*>, new List<Stmt*>); }
          ;

StmtList  :    Stmt                 { ($$ = new List<Stmt*>)->Append($1); } 
          |    StmtList Stmt        { ($$ = $1)->Append($2); }
          ;

Stmt      :    Expr ';'             { $$ = $1; }
          |    ';'                  { $$ = new EmptyExpr; }
          |    ReturnStmt           { $$ = $1; }
          |    IfStmt               { $$ = $1; }
          |    PrintStmt            { $$ = $1; }
          |    WhileStmt            { $$ = $1; }
          |    ForStmt              { $$ = $1; }
          |    BreakStmt            { $$ = $1; }
          |    SwitchStmt           { $$ = $1; }
          |    StmtBlock            { $$ = $1; }
          ;

SwitchStmt:    T_Switch '(' Expr ')' '{' CaseList Default '}'   { $$ = new SwitchStmt($3, new CaseBlock($6), $7); }
          |    T_Switch '(' Expr ')' '{' CaseList '}'   { $$ = new SwitchStmt($3, new CaseBlock($6), NULL); }
          ;

CaseList  :    Case                 { ($$ = new List<Case*>)->Append($1); }
          |    CaseList Case        { ($$ = $1)->Append($2); }
          ;

Case      :    T_Case T_IntConstant ':' StmtList    { $$ = new Case(new IntConstant(yylloc, $2), $4); }
          |    T_Case T_IntConstant ':' { $$ = new Case(new IntConstant(yylloc, $2), new List<Stmt*>); } 
          ; 


Default   :    T_Default ':' StmtList   { $$ = new Default($3); }
          ;

BreakStmt :    T_Break ';'          { $$ = new BreakStmt(yylloc); }

ReturnStmt:    T_Return ';'         { $$ = new ReturnStmt(yylloc, new EmptyExpr); }
          |    T_Return Expr ';'    { $$ = new ReturnStmt(yylloc, $2); }
          ;

WhileStmt :    T_While '(' Expr ')' Stmt    { $$ = new WhileStmt($3, $5); }
          ;

ForStmt   :    T_For '(' OptExpr ';' Expr ';' OptExpr ')' Stmt  { $$ = new ForStmt($3, $5, $7, $9); }
          ;

OptExpr   :    Expr                 { $$ = $1; }
          |                         { $$ = new EmptyExpr; }

IfStmt    :    T_If '(' Expr ')' Stmt           { $$ = new IfStmt($3, $5, NULL); }
          |    T_If '(' Expr ')' Stmt T_Else Stmt   { $$ = new IfStmt($3, $5, $7); }
          ;

Expr      :    LValue '=' Expr      { $$ = new AssignExpr($1, new Operator(yylloc, "="), $3); }
          |    Constant             { $$ = $1; }
          |    LValue               { $$ = $1; }
          |    T_This               { $$ = new This(yylloc); }
          |    Call                 { $$ = $1; }
          |    '(' Expr ')'         { $$ = $2; }
          |    Expr T_Equal Expr    { $$ = new EqualityExpr($1, new Operator(yylloc, "=="), $3); }
          |    Expr T_NotEqual Expr { $$ = new EqualityExpr($1, new Operator(yylloc, "!="), $3); }
          |    Expr '+' Expr        { $$ = new ArithmeticExpr($1, new Operator(yylloc, "+"), $3); }
          |    Expr '-' Expr        { $$ = new ArithmeticExpr($1, new Operator(yylloc, "-"), $3); }
          |    Expr '*' Expr        { $$ = new ArithmeticExpr($1, new Operator(yylloc, "*"), $3); }
          |    Expr '/' Expr        { $$ = new ArithmeticExpr($1, new Operator(yylloc, "/"), $3); }
          |    Expr '%' Expr        { $$ = new ArithmeticExpr($1, new Operator(yylloc, "%"), $3); }
          |    '-' Expr %prec '!'   { $$ = new ArithmeticExpr(new Operator(yylloc, "-"), $2); }
          |    Expr '<' Expr        { $$ = new RelationalExpr($1, new Operator(yylloc, "<"), $3); }
          |    Expr T_LessEqual Expr    { $$ = new RelationalExpr($1, new Operator(yylloc, "<="), $3); }
          |    Expr '>' Expr        { $$ = new RelationalExpr($1, new Operator(yylloc, ">"), $3); }
          |    Expr T_GreaterEqual Expr { $$ = new RelationalExpr($1, new Operator(yylloc, ">="), $3); }
          |    Expr T_And Expr      { $$ = new LogicalExpr($1, new Operator(yylloc, "&&"), $3); }
          |    Expr T_Or Expr       { $$ = new LogicalExpr($1, new Operator(yylloc, "||"), $3); }
          |    '!' Expr             { $$ = new LogicalExpr(new Operator(yylloc, "!"), $2); }
          |    T_ReadInteger '(' ')'    { $$ = new ReadIntegerExpr(yylloc); }
          |    T_ReadLine '(' ')'   { $$ = new ReadLineExpr(yylloc); }
          |    T_New '(' Ident ')'  { $$ = new NewExpr(yylloc, new NamedType($3)); }
          |    T_NewArray '(' Expr ',' Type ')'  { $$ = new NewArrayExpr(yylloc, $3, $5); }
          ;

LValue    :    Ident                { $$ = new FieldAccess(NULL, $1); }
          |    Expr '.' Ident       { $$ = new FieldAccess($1, $3); }
          |    Expr '[' Expr ']'    { $$ = new ArrayAccess(yylloc, $1, $3); }
          |    Expr T_Incr          { $$ = new PostfixExpr(yylloc, $1, new Operator(yylloc, "++")); }
          |    Expr T_Decr          { $$ = new PostfixExpr(yylloc, $1, new Operator(yylloc, "--")); }
          ;

Constant  :    T_IntConstant        { $$ = new IntConstant(yylloc, $1); }
          |    T_BoolConstant       { $$ = new BoolConstant(yylloc, $1); }
          |    T_StringConstant     { $$ = new StringConstant(yylloc, $1); }
          |    T_DoubleConstant     { $$ = new DoubleConstant(yylloc, $1); }
          |    T_Null               { $$ = new NullConstant(yylloc); }
          ;

Call      :    Ident '(' Actuals ')'   { $$ = new Call(yylloc, NULL, $1, $3); }
          |    Expr '.' Ident '(' Actuals ')'   { $$ = new Call(yylloc, $1, $3, $5); }
          ;

Actuals   :    ExprList             { $$ = $1; }
          |                         { $$ = new List<Expr*>; }
          ;

PrintStmt :    T_Print '(' ExprList ')' ';' { $$ = new PrintStmt($3); } 
          ;

ExprList  :    Expr                 { ($$ = new List<Expr*>)->Append($1); }
          |    ExprList ',' Expr    { ($$ = $1)->Append($3); }
          ;
%%

/* The closing %% above marks the end of the Rules section and the beginning
 * of the User Subroutines section. All text from here to the end of the
 * file is copied verbatim to the end of the generated y.tab.c file.
 * This section is where you put definitions of helper functions.
 */

/* Function: InitParser
 * --------------------
 * This function will be called before any calls to yyparse().  It is designed
 * to give you an opportunity to do anything that must be done to initialize
 * the parser (set global variables, configure starting state, etc.). One
 * thing it already does for you is assign the value of the global variable
 * yydebug that controls whether yacc prints debugging information about
 * parser actions (shift/reduce) and contents of state stack during parser.
 * If set to false, no information is printed. Setting it to true will give
 * you a running trail that might be helpful when debugging your parser.
 * Please be sure the variable is set to false when submitting your final
 * version.
 */
void InitParser()
{
   PrintDebug("parser", "Initializing parser");
   yydebug = false;
}
