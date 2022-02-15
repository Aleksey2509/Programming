#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vector Vector;
typedef intptr_t elem_t;
typedef size_t err_t;
typedef size_t iter_t;

Vector* Vector_create();
err_t Vector_init(Vector* vector, size_t capacity, elem_t* data);
err_t Vector_destroy(Vector* vector);

iter_t Vector_createIter(Vector* vector, size_t index);                         // analog to begin() method, but starting index is taken as param
err_t Vector_setIter(Vector* vector, iter_t* iterator, size_t index);           // operator =  for iterator
err_t Vector_incIter(iter_t* iterator);                                         // operator ++ for iterator
err_t Vector_decIter(iter_t* iterator);                                         // operator -- for iterator
elem_t Vector_getElemIter(Vector* vector, const iter_t* iterator);              // operator *  for iterator
err_t Vector_setElemIter(Vector* vector, const iter_t* iterator, elem_t data);  // combination of operators: *(iter*) =  
int Vector_cmpIter(iter_t* firstIter, iter_t* secondIter);                      // returns -1 if element to which firstIter points has 
                                                                                // lesser logical index, 1 if bigger and 0 if they point to
                                                                                // same element

size_t Vector_getSize(Vector* vector);
elem_t Vector_getElem(Vector* vector, int index);

err_t Vector_pushBack(Vector* vector, elem_t data);
err_t Vector_popBack(Vector* vector);
err_t Vector_pushFront(Vector* vector, elem_t data);
err_t Vector_popFront(Vector* vector);

err_t Vector_insertAfter(Vector* vector, size_t index, elem_t elem);
err_t Vector_insertBefore(Vector* vector, size_t index, elem_t elem);

err_t Vector_print(Vector* vector);
extern void printElem(elem_t* data);

enum Errors
{
    INVALID_ARG = -1000,
    MEM_ERR,
};


#endif