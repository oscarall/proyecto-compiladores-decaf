/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "hashtable.h"
#include "errors.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

Hashtable<Decl*> * GetSymbolTable(List<Decl*> * declist) {
    Hashtable<Decl*> * symbolTable = new Hashtable<Decl*>;
    int declNumber = declist->NumElements();

    for(int i = 0; i < declNumber; i++) {
        Decl *decl = declist->Nth(i);
        char * id = decl->GetId();
        if (!(symbolTable->Lookup(id))) {
            symbolTable->Enter(id, decl);
        } else {
            ReportError::DeclConflict(decl, symbolTable->Lookup(id));
        }
    }

    return symbolTable;
}

Hashtable<Decl*> * GetSymbolTable(List<VarDecl*> * declist) {
    Hashtable<Decl*> * symbolTable = new Hashtable<Decl*>;
    int declNumber = declist->NumElements();

    for(int i = 0; i < declNumber; i++) {
        Decl *decl = declist->Nth(i);
        char * id = decl->GetId();
        if (!(symbolTable->Lookup(id))) {
            symbolTable->Enter(id, decl);
        } else {
            ReportError::DeclConflict(decl, symbolTable->Lookup(id));
        }
    }

    return symbolTable;
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */

    int declNumber = decls->NumElements();

    Hashtable<Decl*> * symbolTable = GetSymbolTable(decls);

    for(int i = 0; i < declNumber; i++) {
        Decl *decl = decls->Nth(i);
        decl->Check(symbolTable);
    }
}

SwitchStmt::SwitchStmt(Expr *e, CaseBlock *c, Default *d) {
    Assert(e != NULL && c != NULL);
    (expr=e)->SetParent(this);
    (caseBlock=c)->SetParent(this);
    defaultStmt=d;
    if (defaultStmt) defaultStmt->SetParent(this);
}

Case::Case(Expr *i, List<Stmt*> *s) {
    Assert(i != NULL && s != NULL);
    (intConst=i)->SetParent(this);
    (stmtList=s)->SetParentAll(this);
}

Default::Default(List<Stmt*> *s) {
    Assert(s != NULL);
    (stmtList=s)->SetParentAll(this);
}

CaseBlock::CaseBlock(List<Case*> *c) {
    Assert(c != NULL);
    (caseList=c)->SetParentAll(this);
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::Check(Hashtable <Decl*> * symbolTable) {
    int numberOfDeclarations = decls->NumElements();
    Hashtable <Decl*> * blockSymbolTable = GetSymbolTable(decls);

    for (int i = 0; i < numberOfDeclarations; i++) {
        Decl * decl = decls->Nth(i);
        decl->Check(symbolTable);
    }

    Hashtable <Decl*> * unifiedSymbolTable = UnifySymbolTables(symbolTable, blockSymbolTable);
    int numberOfStatements = stmts->NumElements();

    for (int i = 0; i < numberOfStatements; i++) {
        Stmt * stmt = stmts->Nth(i);
        stmt->Check(unifiedSymbolTable);
    }
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}


