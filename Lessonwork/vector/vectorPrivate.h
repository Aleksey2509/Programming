#ifndef VECTOR_PRIVATE_H
#define VECTOR_PRIVATE_H

struct Vector
{
    size_t capacity;
    size_t size;
    int* elemArray;
};

int Vector_resize(Vector* vector);

#endif