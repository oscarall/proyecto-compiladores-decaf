/* File: ast_type.h
 * ----------------
 * In our parse tree, Type nodes are used to represent and
 * store type information. The base Type class is used
 * for built-in types, the NamedType for classes and interfaces,
 * and the ArrayType for arrays of other types.  
 *
 * pp3: You will need to extend the Type classes to implement
 * the type system and rules for type equivalency and compatibility.
 */
 
#ifndef _H_ast_type
#define _H_ast_type

#include "ast.h"
#include "list.h"
#include <iostream>
#include "hashtable.h"
#include "ast_decl.h"
#include "errors.h"


class Type : public Node 
{
  protected:
    char *typeName;

  public :
    static Type *intType, *doubleType, *boolType, *voidType,
                *nullType, *stringType, *errorType;

    Type(yyltype loc) : Node(loc) {}
    Type(const char *str);
    
    virtual void PrintToStream(std::ostream& out) { out << typeName; }
    friend std::ostream& operator<<(std::ostream& out, Type *t) { t->PrintToStream(out); return out; }
    virtual bool IsEquivalentTo(Type *other) { return this == other; }
    virtual void Check(Hashtable <Decl*> * symbolTable, reasonT reason) {}
};

class NamedType : public Type 
{
  protected:
    Identifier *id;
    
  public:
    NamedType(Identifier *i);
    Identifier * GetIdentifier() { return id; }
    void PrintToStream(std::ostream& out) { out << id; }
    void Check(Hashtable <Decl*> * symbolTable, reasonT reason);
    bool IsEquivalentTo(Type *other);
};

class ArrayType : public Type 
{
  protected:
    Type *elemType;

  public:
    ArrayType(yyltype loc, Type *elemType);
    Type * GetType() { return elemType; }
    void PrintToStream(std::ostream& out) { out << elemType << "[]"; }
    bool IsEquivalentTo(Type *other);
    void Check(Hashtable <Decl*> * symbolTable, reasonT reason);
};

 
#endif
