/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

void Program::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    printf("\n");
}

SwitchStmt::SwitchStmt(Expr *e, CaseBlock *c, Default *d) {
    Assert(e != NULL && c != NULL);
    (expr=e)->SetParent(this);
    (caseBlock=c)->SetParent(this);
    defaultStmt=d;
    if (defaultStmt) defaultStmt->SetParent(this);
}

void SwitchStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1, "(expr) ");
    caseBlock->Print(indentLevel+1);
    if (defaultStmt) defaultStmt->Print(indentLevel+1);
}

Case::Case(Expr *i, List<Stmt*> *s) {
    Assert(i != NULL && s != NULL);
    (intConst=i)->SetParent(this);
    (stmtList=s)->SetParentAll(this);
}

void Case::PrintChildren(int indentLevel) {
    intConst->Print(indentLevel+1, "(constant) ");
    stmtList->PrintAll(indentLevel+1);
}

Default::Default(List<Stmt*> *s) {
    Assert(s != NULL);
    (stmtList=s)->SetParentAll(this);
}

void Default::PrintChildren(int indentLevel) {
    stmtList->PrintAll(indentLevel+1);
}

CaseBlock::CaseBlock(List<Case*> *c) {
    Assert(c != NULL);
    (caseList=c)->SetParentAll(this);
}
void CaseBlock::PrintChildren(int indentLevel) {
    caseList->PrintAll(indentLevel+1);
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

void StmtBlock::PrintChildren(int indentLevel) {
    decls->PrintAll(indentLevel+1);
    stmts->PrintAll(indentLevel+1);
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

void ForStmt::PrintChildren(int indentLevel) {
    init->Print(indentLevel+1, "(init) ");
    test->Print(indentLevel+1, "(test) ");
    step->Print(indentLevel+1, "(step) ");
    body->Print(indentLevel+1, "(body) ");
}

void WhileStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(body) ");
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) elseBody->SetParent(this);
}

void IfStmt::PrintChildren(int indentLevel) {
    test->Print(indentLevel+1, "(test) ");
    body->Print(indentLevel+1, "(then) ");
    if (elseBody) elseBody->Print(indentLevel+1, "(else) ");
}


ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::PrintChildren(int indentLevel) {
    expr->Print(indentLevel+1);
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

void PrintStmt::PrintChildren(int indentLevel) {
    args->PrintAll(indentLevel+1, "(args) ");
}

