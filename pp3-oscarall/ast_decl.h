/* File: ast_decl.h
 * ----------------
 * In our parse tree, Decl nodes are used to represent and
 * manage declarations. There are 4 subclasses of the base class,
 * specialized for declarations of variables, functions, classes,
 * and interfaces.
 *
 * pp3: You will need to extend the Decl classes to implement 
 * semantic processing including detection of declaration conflicts 
 * and managing scoping issues.
 */

#ifndef _H_ast_decl
#define _H_ast_decl

#include "ast.h"
#include "list.h"
#include "hashtable.h"

class Type;
class NamedType;
class Identifier;
class Stmt;

class Decl : public Node 
{
  protected:
    Identifier *id;
  
  public:
    Decl(Identifier *name);
    friend std::ostream& operator<<(std::ostream& out, Decl *d) { return out << d->id; }
    char * GetId() { return id->GetName(); }
    virtual void Check(Hashtable <Decl*> * symbolTable);
};

class VarDecl : public Decl 
{
  protected:
    Type *type;
    
  public:
    VarDecl(Identifier *name, Type *type);
    Type * GetType() { return type; }
    void Check(Hashtable <Decl*> * symbolTable);
};

class InterfaceDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    
  public:
    InterfaceDecl(Identifier *name, List<Decl*> *members);
    void Check(Hashtable <Decl*> * symbolTable) { Decl::Check(symbolTable); }
    Hashtable <Decl*> * GetSymbolTable();
};

class ClassDecl : public Decl 
{
  protected:
    List<Decl*> *members;
    NamedType *extends;
    List<NamedType*> *implements;
    ClassDecl * parentClass;
    List<InterfaceDecl*> * interfaces;
    bool hasChecked;
    Hashtable <Decl*> * classSymbolTable;

  public:
    ClassDecl(Identifier *name, NamedType *extends, 
    List<NamedType*> *implements, List<Decl*> *members);
    void Check(Hashtable <Decl*> * symbolTable);
    Hashtable<Decl*> * GetInheritedSymbolTable();
    Hashtable<Decl*> * GetSymbolTable() { return classSymbolTable; }
    void CreateSymbolTable();
};

class FnDecl : public Decl 
{
  protected:
    List<VarDecl*> *formals;
    Type *returnType;
    Stmt *body;
    
  public:
    FnDecl(Identifier *name, Type *returnType, List<VarDecl*> *formals);
    void SetFunctionBody(Stmt *b);
    void Check(Hashtable <Decl*> * symbolTable);
    Type * GetType() { return returnType; }
    List<VarDecl*> * GetFormals() { return formals; }
    bool Compare(FnDecl * other);
};

Hashtable<Decl *> * UnifySymbolTables(Hashtable<Decl *> * parent, Hashtable<Decl *> * child);

#endif
