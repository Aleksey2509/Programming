#ifndef CONTAINER_H
#define CONTAINER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef intptr_t elem_t;
typedef size_t err_t;
typedef size_t iter_t;

typedef struct ContainerMethods
{
    err_t  (*init)(void*, size_t, const elem_t* );
    err_t  (*destroy)(void* );

    iter_t (*createIter)(void* , size_t );                  // analog to begin() method, but starting index is taken as param
    err_t  (*setIter)(void* , iter_t* , size_t );           // operator =  for iterator
    err_t  (*incIter)(iter_t* );                            // operator ++ for iterator
    err_t  (*decIter)(iter_t* );                            // operator -- for iterator
    elem_t (*getElemIter)(void* , const iter_t* );          // operator *  for iterator
    err_t  (*setElemIter)(void*, const iter_t* , elem_t );  // combination of operators: *(iter*) =  
    int    (*cmpIter)(iter_t* , iter_t* );

    err_t  (*pushBack)(void* , const elem_t );
    err_t  (*popBack)(void* );
    err_t  (*pushFront)(void* , const elem_t);
    err_t  (*popFront)(void*);
    elem_t (*getElem)(void* , int);
    size_t (*getSize)(void*);
    err_t  (*insertAfter)(void* , size_t , elem_t);
    err_t  (*insertBefore)(void* , size_t , elem_t);
    err_t  (*print)(void* );
}ContainerMethods;


#endif