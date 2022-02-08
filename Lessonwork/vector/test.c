#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector.h"

int main()
{
    size_t cap = 20;
    Vector* testSubject = Vector_create();
    int err = Vector_init(testSubject, cap, NULL);
    Vector_pushBack(testSubject, 5);
    Vector_pushBack(testSubject, 4);
    Vector_pushBack(testSubject, 2);
    Vector_print(testSubject);
    Vector_destroy(testSubject);
}