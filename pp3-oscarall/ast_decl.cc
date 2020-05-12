/* File: ast_decl.cc
 * -----------------
 * Implementation of Decl node classes.
 */
#include "ast_decl.h"
#include "ast_type.h"
#include "ast_stmt.h"
#include "hashtable.h"
#include "errors.h"
        
         
Decl::Decl(Identifier *n) : Node(*n->GetLocation()) {
    Assert(n != NULL);
    (id=n)->SetParent(this); 
}

void Decl::Check(Hashtable <Decl*> * symbolTable) {
    
}

Hashtable<Decl*> * GetSymbolTableDecl(List<Decl*> * declist) {
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

Hashtable<Decl*> * GetSymbolTableDecl(List<VarDecl*> * declist) {
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

Hashtable<Decl *> * UnifySymbolTables(Hashtable<Decl *> * parent, Hashtable<Decl *> * child) {
  Hashtable<Decl *> * unifiedSymbolTable = new Hashtable<Decl *>();

  Iterator<Decl*> parentIterator = parent->GetIterator();
  Iterator<Decl*> childIterator = child->GetIterator();

  Decl *decl;
  while ((decl = parentIterator.GetNextValue()) != NULL) {
    unifiedSymbolTable->Enter(decl->GetId(), decl, false);
  }

  while ((decl = childIterator.GetNextValue()) != NULL) {
    unifiedSymbolTable->Enter(decl->GetId(), decl, false);
  }

  return unifiedSymbolTable;
}

void FnDecl::Check(Hashtable <Decl*> * symbolTable) { 
    Hashtable<Decl*> * formalsSymbolTable = GetSymbolTableDecl(formals);

    returnType->Check(symbolTable, LookingForType);

    int declNumber = formals->NumElements();

    for(int i = 0; i < declNumber; i++) {
        VarDecl *decl = formals->Nth(i);
        decl->Check(symbolTable);
    }

    Hashtable<Decl *> * unifiedSymbolTable = UnifySymbolTables(symbolTable, formalsSymbolTable);

    if (body) body->Check(unifiedSymbolTable);
}

void VarDecl::Check(Hashtable <Decl*> * symbolTable) {
    type->Check(symbolTable, LookingForType);
}

void ClassDecl::Check(Hashtable <Decl*> * symbolTable) {
    if (hasChecked) {
        return;
    }
    hasChecked = true;
    if (extends) {
        extends->Check(symbolTable, LookingForClass);
        Decl * decl = symbolTable->Lookup(extends->GetIdentifier()->GetName());
        if (decl) { 
            parentClass = dynamic_cast<ClassDecl *>(decl);
            parentClass->Check(symbolTable);
        }
    }

    int numberOfInterfaces = implements->NumElements();
    interfaces = new List<InterfaceDecl*>;

    for(int i = 0; i < numberOfInterfaces; i++) {
        NamedType * interface = implements->Nth(i);
        interface->Check(symbolTable, LookingForInterface);
        Decl * decl = symbolTable->Lookup(interface->GetIdentifier()->GetName());
        if (decl) {
            InterfaceDecl * intDecl = dynamic_cast<InterfaceDecl *>(decl);
            if (intDecl) interfaces->Append(intDecl);
        }
    }

    CreateSymbolTable();
    Hashtable<Decl*> * unifiedSymbolTable = UnifySymbolTables(symbolTable, classSymbolTable);

    int numberOfMembers = members->NumElements();

    for(int i = 0; i < numberOfMembers; i++) {
        members->Nth(i)->Check(unifiedSymbolTable);
    }
}

Hashtable <Decl*> * InterfaceDecl::GetSymbolTable() {
    Hashtable<Decl*> * symbolTable = new Hashtable<Decl*>;
    int declNumber = members->NumElements();

    for(int i = 0; i < declNumber; i++) {
        Decl *decl = members->Nth(i);
        char *id = decl->GetId();
        if (!(symbolTable->Lookup(id))) {
            symbolTable->Enter(id, decl);
        }
    }

    return symbolTable;
}

Hashtable<Decl*> * ClassDecl::GetInheritedSymbolTable() {
    Hashtable<Decl*> * inheritedSymbolTable = new Hashtable<Decl*>;
    if (parentClass) {
        Iterator<Decl*> iter = parentClass->GetSymbolTable()->GetIterator();
        Decl * decl;
        while((decl = iter.GetNextValue()) != NULL) {
            inheritedSymbolTable->Enter(decl->GetId(), decl);
        }
    }

    int numberOfInterfaces = interfaces->NumElements();
    
    for(int i = 0; i < numberOfInterfaces; i++) {
        Decl * decl = interfaces->Nth(i);
        InterfaceDecl * intDecl = dynamic_cast<InterfaceDecl*>(decl);
        if (intDecl) {
            Iterator<Decl *> iter = intDecl->GetSymbolTable()->GetIterator();
            Decl * decl;
            while ((decl = iter.GetNextValue()) != NULL) {
                inheritedSymbolTable->Enter(decl->GetId(), decl);
            }
        }
    }

    return inheritedSymbolTable;
}

void ClassDecl::CreateSymbolTable() {
    classSymbolTable = GetInheritedSymbolTable();
    Hashtable<Decl*> * currentClassSymbolTable = GetSymbolTableDecl(members);


    Iterator<Decl*> iter = currentClassSymbolTable->GetIterator();
    Decl *decl;
    while ((decl = iter.GetNextValue()) != NULL) {
        char * id = decl->GetId();
        Decl * symbolDecl = classSymbolTable->Lookup(id);
        if (symbolDecl) {
            VarDecl * varDecl = dynamic_cast<VarDecl *>(decl);
            FnDecl * fnDecl = dynamic_cast<FnDecl *>(decl);
            
            if (varDecl) ReportError::DeclConflict(varDecl, symbolDecl);

            if (fnDecl) {
                FnDecl * inheritedFnDecl = dynamic_cast<FnDecl*>(symbolDecl);
                if (inheritedFnDecl) {
                    if (!inheritedFnDecl->Compare(fnDecl)) {
                        ReportError::OverrideMismatch(fnDecl);
                    } else {
                        classSymbolTable->Enter(id, fnDecl, false);
                    }
                } else {
                    ReportError::DeclConflict(fnDecl, symbolDecl);
                }
            }
        } else {
            classSymbolTable->Enter(id, decl);
        }
    }
    
}

bool FnDecl::Compare(FnDecl * other) {
    Type * otherType = other->GetType();

    if (!returnType->IsEquivalentTo(otherType)) {
        return false;
    }
    List<VarDecl*> * otherFormals = other->GetFormals();
    int numberOfFormals = formals->NumElements();
    int numberOfOtherFormals = otherFormals->NumElements();
    if (numberOfFormals != numberOfOtherFormals) {
        return false;
    }

    for (int i = 0; i < numberOfFormals; i++) {
        VarDecl * formal = formals->Nth(i);
        VarDecl * otherFormal = otherFormals->Nth(i);
        
        if (!formal->GetType()->IsEquivalentTo(otherFormal->GetType())) {
            return false;
        }    
    }

    return true;
}

VarDecl::VarDecl(Identifier *n, Type *t) : Decl(n) {
    Assert(n != NULL && t != NULL);
    (type=t)->SetParent(this);
}
  

ClassDecl::ClassDecl(Identifier *n, NamedType *ex, List<NamedType*> *imp, List<Decl*> *m) : Decl(n) {
    // extends can be NULL, impl & mem may be empty lists but cannot be NULL
    Assert(n != NULL && imp != NULL && m != NULL);     
    extends = ex;
    if (extends) extends->SetParent(this);
    (implements=imp)->SetParentAll(this);
    (members=m)->SetParentAll(this);
    hasChecked = false;
}


InterfaceDecl::InterfaceDecl(Identifier *n, List<Decl*> *m) : Decl(n) {
    Assert(n != NULL && m != NULL);
    (members=m)->SetParentAll(this);
}

	
FnDecl::FnDecl(Identifier *n, Type *r, List<VarDecl*> *d) : Decl(n) {
    Assert(n != NULL && r!= NULL && d != NULL);
    (returnType=r)->SetParent(this);
    (formals=d)->SetParentAll(this);
    body = NULL;
}

void FnDecl::SetFunctionBody(Stmt *b) { 
    (body=b)->SetParent(this);
}



