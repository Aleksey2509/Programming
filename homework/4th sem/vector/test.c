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

void Vector_sort(VectorContainer* vectorCont, int (*comparator)(const void *, const void *));
err_t Vector_swapElem(VectorContainer* vectorCont, size_t i, size_t j);

int main()
{
    size_t cap = 20;
    VectorContainer* testSubject = Vector_create();
    int err = testSubject->m->init(testSubject, cap, NULL);
    if (err)
        return err;

    testSubject->m->pushBack(testSubject, 5);
    testSubject->m->print(testSubject);
    testSubject->m->pushBack(testSubject, 25);
    testSubject->m->pushBack(testSubject, 4);
    testSubject->m->pushBack(testSubject, 300);
    testSubject->m->pushBack(testSubject, 6);
    testSubject->m->pushBack(testSubject, 3);
    testSubject->m->pushBack(testSubject, 200);

    testSubject->m->print(testSubject);

    //Vector_swapElem(testSubject, 1, 4);

    Vector_sort(testSubject, comparator);

    testSubject->m->print(testSubject);
    testSubject->m->destroy(testSubject);
}