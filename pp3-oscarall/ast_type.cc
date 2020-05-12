/* File: ast_type.cc
 * -----------------
 * Implementation of type node classes.
 */
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include "errors.h"

 
/* Class constants
 * ---------------
 * These are public constants for the built-in base types (int, double, etc.)
 * They can be accessed with the syntax Type::intType. This allows you to
 * directly access them and share the built-in types where needed rather that
 * creates lots of copies.
 */

Type *Type::intType    = new Type("int");
Type *Type::doubleType = new Type("double");
Type *Type::voidType   = new Type("void");
Type *Type::boolType   = new Type("bool");
Type *Type::nullType   = new Type("null");
Type *Type::stringType = new Type("string");
Type *Type::errorType  = new Type("error"); 

Type::Type(const char *n) {
    Assert(n);
    typeName = strdup(n);
}



	
NamedType::NamedType(Identifier *i) : Type(*i->GetLocation()) {
    Assert(i != NULL);
    (id=i)->SetParent(this);
} 


ArrayType::ArrayType(yyltype loc, Type *et) : Type(loc) {
    Assert(et != NULL);
    (elemType=et)->SetParent(this);
}

bool NamedType::IsEquivalentTo(Type * other) {
    NamedType * otherNamedType = dynamic_cast<NamedType*>(other);

    if (otherNamedType) {
        return strcmp(id->GetName(), otherNamedType->id->GetName()) == 0;
    }

    return false;
}

bool ArrayType::IsEquivalentTo(Type * other) {
    ArrayType * otherArrayType = dynamic_cast<ArrayType*>(other);

    if (otherArrayType) {
        return elemType->IsEquivalentTo(otherArrayType->GetType());
    }

    return false;
}

void ArrayType::Check(Hashtable <Decl*> * symbolTable, reasonT reason = LookingForType) {
    elemType->Check(symbolTable, reason);
}

void NamedType::Check(Hashtable <Decl*> * symbolTable, reasonT reason) {
    char * identifier = id->GetName();
    Decl * decl = symbolTable->Lookup(identifier);

    if (!decl) {
        ReportError::IdentifierNotDeclared(id, reason);
        return;
    }
    ClassDecl * classDecl = dynamic_cast<ClassDecl *>(decl);
    InterfaceDecl * interfaceDecl = dynamic_cast<InterfaceDecl *>(decl);

    if ((reason == LookingForClass && !classDecl) || (reason == LookingForInterface && !interfaceDecl)) {
        ReportError::IdentifierNotDeclared(id, reason);
        return;
    }

    if (reason == LookingForType && (!classDecl && !interfaceDecl)) {
        ReportError::IdentifierNotDeclared(id, reason);
    }
}


