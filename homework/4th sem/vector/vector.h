#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../container/container.h"

typedef struct VectorData VectorData;
typedef struct Container
{
    const ContainerMethods* m;
    VectorData* vector;
}Container;

Container* Vector_create();
extern void printElem(elem_t* data);

enum Errors
{
    INVALID_ARG = -1000,
    MEM_ERR,
};


#endif