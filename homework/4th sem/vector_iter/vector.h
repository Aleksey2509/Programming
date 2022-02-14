#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>

typedef struct Vector Vector;

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

void Vector_sort(Vector* vector, int (*comparator)(const void *, const void *));

enum Errors
{
    INVALID_ARG = -1000,
    MEM_ERR,
};


#endif