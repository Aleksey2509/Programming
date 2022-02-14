#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vector Vector;
typedef intptr_t elem_t;

Vector* Vector_create();
int Vector_init(Vector*, size_t capacity, const int* data);
int Vector_destroy(Vector* vector);
int Vector_pushBack(Vector* vector, const int data);
int Vector_popBack(Vector* vector);
int Vector_pushFront(Vector* vector, const int data);
int Vector_popFront(Vector* vector);
int Vector_getElem(Vector* vector, int index);
int Vector_insertAfter(Vector* vector, int position, int elem);
int Vector_insertBefore(Vector* vector, int position, int elem);
int Vector_print(Vector* vector);
extern void printElem(elem_t* data);

void Vector_sort(Vector* vector, int (*comparator)(const void *, const void *));

enum Errors
{
    INVALID_ARG = -1000,
    MEM_ERR,
};


#endif