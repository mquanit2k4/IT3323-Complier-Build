/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab* symtab;
extern Token* currentToken;

static int on_search = 0;

/**
 * @function lookupObject: Look up an object by name in the symbol table.
 * 
 * @param name: A pointer to the name of the object to look up.
 * 
 * @return: A pointer to the Object if found.
 *          NULL if the object is not found in any scope.
 **/
Object* lookupObject(char *name) {
    Object* object = NULL;
    static Scope* currentScope = NULL;

    if (on_search == 0)
        currentScope = symtab->currentScope;

    while (currentScope != NULL) {
        object = findObject(currentScope->objList, name);

        currentScope = currentScope->outer;
        on_search = 1;

        if (object != NULL)
            return object;
    }

    object = findObject(symtab->globalObjectList, name);

    on_search = 0;

    return object;
}

/**
 * @function checkFreshIdent: Check if an identifier is fresh (not yet declared) in the current scope.
 * 
 * @param name: A pointer to the name of the identifier to check.
 * 
 * @return: Nothing. Throws ERR_DUPLICATE_IDENT error if the identifier already exists.
 **/
void checkFreshIdent(char *name) {
    Object * obj = findObject(symtab->currentScope->objList, name);
    if (obj != NULL)
        error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

/**
 * @function checkDeclaredIdent: Check if an identifier has been declared.
 * 
 * @param name: A pointer to the name of the identifier to check.
 * 
 * @return: A pointer to the declared Object.
 *          Throws ERR_UNDECLARED_IDENT error if the identifier is not declared.
 **/
Object* checkDeclaredIdent(char* name) {
    on_search = 0;

    Object *obj = lookupObject(name);

    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

/**
 * @function checkDeclaredConstant: Check if a constant has been declared.
 * 
 * @param name: A pointer to the name of the constant to check.
 * 
 * @return: A pointer to the declared constant Object.
 *          Throws ERR_UNDECLARED_CONSTANT error if the constant is not declared.
 **/
Object* checkDeclaredConstant(char* name) {
    Object *obj = NULL;

    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_CONSTANT)
            break;
    } while (obj != NULL);

     
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

    return obj;
}

/**
 * @function checkDeclaredType: Check if a type has been declared.
 * 
 * @param name: A pointer to the name of the type to check.
 * 
 * @return: A pointer to the declared type Object.
 *          Throws ERR_UNDECLARED_TYPE error if the type is not declared.
 **/
Object* checkDeclaredType(char* name) {
    Object *obj = NULL;

     
    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_TYPE)
            break;
    } while (obj != NULL);

    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

/**
 * @function checkDeclaredVariable: Check if a variable has been declared.
 * 
 * @param name: A pointer to the name of the variable to check.
 * 
 * @return: A pointer to the declared variable Object.
 *          Throws ERR_UNDECLARED_VARIABLE error if the variable is not declared.
 **/
Object* checkDeclaredVariable(char* name) {
    Object *obj = NULL;

    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_VARIABLE)
            break;
    } while (obj != NULL);
     
    on_search = 0;

    if (obj == NULL)
        error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

/**
 * @function checkDeclaredFunction: Check if a function has been declared.
 * 
 * @param name: A pointer to the name of the function to check.
 * 
 * @return: A pointer to the declared function Object.
 *          Throws ERR_UNDECLARED_FUNCTION error if the function is not declared.
 **/
Object* checkDeclaredFunction(char* name) {
    Object *obj = NULL;

    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_FUNCTION)
            break;
    } while (obj != NULL);

    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);

    return obj;
}

/**
 * @function checkDeclaredProcedure: Check if a procedure has been declared.
 * 
 * @param name: A pointer to the name of the procedure to check.
 * 
 * @return: A pointer to the declared procedure Object.
 *          Throws ERR_UNDECLARED_PROCEDURE error if the procedure is not declared.
 **/
Object* checkDeclaredProcedure(char* name) {
    Object *obj = NULL;

    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && obj->kind == OBJ_PROCEDURE)
            break;
    } while (obj != NULL);

    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);

    return obj;
}

/**
 * @function checkDeclaredLValueIdent: Check if an identifier can be used as an L-value (assignable).
 * 
 * @param name: A pointer to the name of the identifier to check.
 * 
 * @return: A pointer to the declared Object (function, parameter, or variable).
 *          Throws ERR_UNDECLARED_IDENT error if the identifier is not declared or cannot be used as L-value.
 **/
Object* checkDeclaredLValueIdent(char* name) {
    Object *obj = NULL;

    on_search = 0;

    do {
        obj = lookupObject(name);
        if (obj != NULL && (obj->kind == OBJ_FUNCTION || obj->kind == OBJ_PARAMETER || obj->kind == OBJ_VARIABLE))
            break;
    } while (obj != NULL);
     
    on_search = 0;

    if (obj == NULL)
            error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

    return obj;
}
