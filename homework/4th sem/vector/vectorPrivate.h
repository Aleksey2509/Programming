#ifndef VECTOR_PRIVATE_H
#define VECTOR_PRIVATE_H
#include "vector.h"
#include "../container/container.h"


struct VectorData
{
    size_t capacity;
    size_t size;
    elem_t* elemArray;
};

err_t Vector_init(void* vector, size_t capacity, const elem_t* data);
err_t Vector_destroy(void* vector);

iter_t Vector_begin(void* vector);
iter_t Vector_end(void* vector);
iter_t Vector_createIter(void* vector, size_t index);                         // analog to begin() method, but starting index is taken as param
err_t Vector_setIter(void* vector, iter_t* iterator, size_t index);           // operator =  for iterator
err_t Vector_incIter(iter_t* iterator);                                         // operator ++ for iterator
err_t Vector_decIter(iter_t* iterator);                                         // operator -- for iterator
elem_t Vector_getElemIter(void* vector, const iter_t* iterator);              // operator *  for iterator
err_t Vector_setElemIter(void* vector, const iter_t* iterator, elem_t data);  // combination of operators: *(iter*) =  
int Vector_cmpIter(iter_t* firstIter, iter_t* secondIter);                      // returns -1 if element to which firstIter points has 
                                                                                // lesser logical index, 1 if bigger and 0 if they point to
                                                                                // same elemnaent
size_t Vector_getSize(void* vector);
elem_t Vector_getElem(void* vector, int index);

err_t Vector_pushBack(void* vector, elem_t data);
err_t Vector_popBack(void* vector);
err_t Vector_pushFront(void* vector, elem_t data);
err_t Vector_popFront(void* vector);

err_t Vector_insertAfter(void* vector, size_t index, elem_t elem);
err_t Vector_insertBefore(void* vector, size_t index, elem_t elem);

err_t Vector_print(void* vector);

err_t Vector_resize(void* vector);

const ContainerMethods VectorMethods = 
{
    .init = Vector_init,
    .destroy = Vector_destroy,
    .begin = Vector_begin,
    .end = Vector_end,
    .createIter = Vector_createIter,            // analog to begin() method, but starting index is taken as param
    .setIter = Vector_setIter,                  // operator =  for iterator
    .incIter = Vector_incIter,                  // operator ++ for iterator
    .decIter = Vector_decIter,                  // operator -- for iterator
    .getElemIter = Vector_getElemIter,          // operator *  for iterator
    .setElemIter = Vector_setElemIter,          // combination of operators: *(iter*) =  
    .cmpIter = Vector_cmpIter,
    .getElem = Vector_getElem,
    .getSize = Vector_getSize,
    .pushBack = Vector_pushBack,
    .popBack = Vector_popBack,
    .pushFront = Vector_pushBack,
    .popFront = Vector_popBack,
    .insertAfter = Vector_insertAfter,
    .insertBefore = Vector_insertBefore,
    .print = Vector_print
};

#endif