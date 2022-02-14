#ifndef VECTOR_PRIVATE_H
#define VECTOR_PRIVATE_H

typedef intptr_t iter_t;

struct Vector
{
    size_t capacity;
    size_t size;
    iter_t iter;
    size_t elemSize;
    void* elemArray;
};

int Vector_resize(Vector* vector);

#endif