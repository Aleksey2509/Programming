#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct container_methods
{
    int (*next)(int);
    int (*create)();
    int (*init)(void*, size_t, const int* );
    int (*destroy)(void* );
    int (*pushBack)(void* , const int );
    int (*popBack)(void* );
    int (*pushFront)(void* , const int);
    int (*popFront)(void*);
    int (*getElem)(void* , int);
    int (*insertAfter)(void* , int , int);
    int (*insertBefore)(void* , int , int);
};