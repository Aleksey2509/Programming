#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

int comparator (const void* a, const void* b)
{
    int c = *(int *)a;
    int d = *(int *)b;
    return (c > d) - (c < d);
}

void printElem(elem_t* data)
{
    printf("%d\n", *(int*)data);
}

int main()
{
    size_t cap = 20;
    Vector* testSubject = Vector_create();
    int err = Vector_init(testSubject, cap, NULL);
    if (err)
        return err;

    Vector_pushBack(testSubject, 5);
    Vector_pushBack(testSubject, 2);
    Vector_pushBack(testSubject, 4);

    Vector_print(testSubject);

    Vector_sort(testSubject, &comparator);

    Vector_print(testSubject);
    Vector_destroy(testSubject);
}