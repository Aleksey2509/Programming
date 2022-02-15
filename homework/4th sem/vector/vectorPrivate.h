#ifndef VECTOR_PRIVATE_H
#define VECTOR_PRIVATE_H

struct Vector
{
    size_t capacity;
    size_t size;
    elem_t* elemArray;
};

err_t Vector_resize(Vector* vector);

#endif