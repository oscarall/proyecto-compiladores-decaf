/* File: ast_stmt.h
 * ----------------
 * The Stmt class and its subclasses are used to represent
 * statements in the parse tree.  For each statment in the
 * language (for, if, return, etc.) there is a corresponding
 * node class for that construct. 
 *
 * pp3: You will need to extend the Stmt classes to implement
 * semantic analysis for rules pertaining to statements.
 */


#ifndef _H_ast_stmt
#define _H_ast_stmt

#include "list.h"
#include "hashtable.h"
#include "ast.h"

class Decl;
class VarDecl;
class Expr;
  
class Program : public Node
{
  protected:
     List<Decl*> *decls;
     Hashtable<Decl*> *symbolTable;
     
  public:
     Program(List<Decl*> *declList);
     void Check();
};

class Stmt : public Node
{
  public:
     Stmt() : Node() {}
     Stmt(yyltype loc) : Node(loc) {}
     virtual void Check(Hashtable <Decl*> * symbolTable) {}
};

class Default: public Stmt
{
  protected:
    List<Stmt*> *stmtList;

  public:
    Default(List<Stmt*> *stmtList);
    const char *GetPrintNameForNode() { return "Default"; }
    void Check(Hashtable <Decl*> * symbolTable) {}
};

class Case : public Stmt
{
  protected:
    Expr *intConst;
    List<Stmt*> *stmtList;
  
  public:
    Case(Expr *intConst, List<Stmt*> *stmtList);
    const char *GetPrintNameForNode() { return "Case"; }
    void Check(Hashtable <Decl*> * symbolTable) {}
};

class CaseBlock : public Stmt
{
  protected:
    List<Case*> *caseList;
  
  public:
    CaseBlock(List<Case*> *caseList);
    const char *GetPrintNameForNode() { return "CaseBlock"; }
    void Check(Hashtable <Decl*> * symbolTable) {}
};

class SwitchStmt : public Stmt
{
  protected:
    Expr *expr;
    CaseBlock *caseBlock;
    Default *defaultStmt;
  
  public:
    SwitchStmt(Expr *expr, CaseBlock *caseList, Default *defaultStmt);
    const char *GetPrintNameForNode() { return "SwitchStmt"; }
    void Check(Hashtable <Decl*> * symbolTable) {}
};

class StmtBlock : public Stmt 
{
  protected:
    List<VarDecl*> *decls;
    List<Stmt*> *stmts;
    
  public:
    StmtBlock(List<VarDecl*> *variableDeclarations, List<Stmt*> *statements);
    void Check(Hashtable <Decl*> * symbolTable);
};

  
class ConditionalStmt : public Stmt
{
  protected:
    Expr *test;
    Stmt *body;
  
  public:
    ConditionalStmt(Expr *testExpr, Stmt *body);
    void Check(Hashtable <Decl*> * symbolTable) { body->Check(symbolTable); }
};

class LoopStmt : public ConditionalStmt 
{
  public:
    LoopStmt(Expr *testExpr, Stmt *body)
            : ConditionalStmt(testExpr, body) {}
    void Check(Hashtable <Decl*> * symbolTable) { ConditionalStmt::Check(symbolTable); }
};

class ForStmt : public LoopStmt 
{
  protected:
    Expr *init, *step;
  
  public:
    ForStmt(Expr *init, Expr *test, Expr *step, Stmt *body);
    void Check(Hashtable <Decl*> * symbolTable) { LoopStmt::Check(symbolTable); }
};

class WhileStmt : public LoopStmt 
{
  public:
    WhileStmt(Expr *test, Stmt *body) : LoopStmt(test, body) {}
    void Check(Hashtable <Decl*> * symbolTable) { LoopStmt::Check(symbolTable); }
};

class IfStmt : public ConditionalStmt 
{
  protected:
    Stmt *elseBody;
  
  public:
    IfStmt(Expr *test, Stmt *thenBody, Stmt *elseBody);
    void Check(Hashtable <Decl*> * symbolTable) { ConditionalStmt::Check(symbolTable); }
};

class BreakStmt : public Stmt 
{
  public:
    BreakStmt(yyltype loc) : Stmt(loc) {}
    void Check(Hashtable <Decl*> * symbolTable) {}
};

class ReturnStmt : public Stmt  
{
  protected:
    Expr *expr;
  
  public:
    ReturnStmt(yyltype loc, Expr *expr);
    void Check(Hashtable <Decl*> * symbolTable) {}
};

class PrintStmt : public Stmt
{
  protected:
    List<Expr*> *args;
    
  public:
    PrintStmt(List<Expr*> *arguments);
    void Check(Hashtable <Decl*> * symbolTable) {}
};


#endif
