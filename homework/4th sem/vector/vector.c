#include "vector.h"
#include "vectorPrivate.h"

Vector* Vector_create()
{
    Vector* vector = malloc(sizeof(Vector));
    if (vector == NULL)
        return NULL;

    vector->elemArray = NULL;
    vector->capacity = 0;
    vector->size = 0;

    return vector;
}

err_t Vector_init(Vector* vector, size_t capacity, elem_t* data)
{
    if ((vector == NULL) || (capacity == 0))
        return INVALID_ARG;

    vector->elemArray = calloc(capacity, sizeof(int));
    if (vector->elemArray == NULL)
        return MEM_ERR;

    vector->capacity = capacity;
    if (data == NULL)
    {
        vector->size = 0;
        return 0;
    }

    memcpy(vector->elemArray, data, capacity);

    return 0;
}

err_t Vector_destroy(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    vector->capacity = 0;
    vector->size = 0;
    free (vector);
    free (vector->elemArray);
    vector->elemArray = NULL;

    return 0;
}

//================================================================================================
//  iterators: replaced what would be done with operators using functions
//================================================================================================

iter_t Vector_createIter(Vector* vector, size_t index)
{
    if ((vector == NULL) || (index < 0) || (index > vector->size - 1))
        return INVALID_ARG;

    return index;
}

err_t Vector_setIter(Vector* vector, iter_t* iterator, size_t index)
{
    if ((vector == NULL) || (index < 0) || (index > vector->size - 1))
        return INVALID_ARG;

    *iterator = index;

    return 0;
}

err_t Vector_incIter(iter_t* iterator)
{
    if (iterator == NULL)
        return INVALID_ARG;

    (*iterator)++;

    return 0;
}

err_t Vector_decIter(iter_t* iterator)
{
    if ((iterator == NULL) || (*iterator == 0))
        return INVALID_ARG;

    (*iterator)--;

    return 0;
}

elem_t Vector_getElemIter(Vector* vector, const iter_t* iterator)
{
    int index = *iterator;
    return vector->elemArray[index];
}

err_t Vector_setElemIter(Vector* vector, const iter_t* iterator, elem_t data)
{
    if ((vector == NULL) || (iterator == NULL))
        return INVALID_ARG;

    vector->elemArray[*iterator] = data;
    return 0;
}

int Vector_cmpIter(iter_t* firstIter, iter_t* secondIter)
{
    return (*firstIter) == (*secondIter);
}

//================================================================================================

size_t Vector_getSize(Vector* vector)
{
    return vector->size;
}

elem_t Vector_getElem(Vector* vector, int index)
{
    if ((vector == NULL) || (index < 0) || (index > (vector->size - 1)))
        return INVALID_ARG;

    return vector->elemArray[index];
}

err_t Vector_pushBack(Vector* vector, elem_t data)
{
    if (vector == NULL)
        return INVALID_ARG;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vector);
        if (err)
            return err;
    }

    vector->elemArray[vector->size] = data;
    vector->size++;

    return 0;
}

err_t Vector_popBack(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    vector->size--;
    return (vector->elemArray[vector->size]);
}

err_t Vector_pushFront(Vector* vector, elem_t data)
{
    if (vector == NULL)
        return INVALID_ARG;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vector);
        if (err)
            return err;
    }

    for (int i = vector->size; i > 0; i--)
    {
        vector->elemArray[i] = vector->elemArray[i - 1];
    }

    vector->elemArray[0] = data;
    vector->size++;

    return 0;
}

err_t Vector_popFront(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    int data = vector->elemArray[0];
    for (int i = 0; i < vector->size - 1; i++)
    {
        vector->elemArray[i] = vector->elemArray[i + 1];
    }

    return data;
}

err_t Vector_insertAfter(Vector* vector, size_t index, elem_t elem)
{
    if ((vector == NULL) || (index < 0) || (index > (vector->size - 1)))
        return INVALID_ARG;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vector);
        if (err)
            return err;
    }

    for (int i = vector->size - 1; i > index + 1; i--)
    {
        vector->elemArray[i] = vector->elemArray[i - 1];
    }
    vector->elemArray[index + 1] = elem;

    return 0;
}

err_t Vector_insertBefore(Vector* vector, size_t index, elem_t elem)
{
    if ((vector == NULL) || (index < 0) || (index > (vector->capacity - 1)))
        return INVALID_ARG;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vector);
        if (err)
            return err;
    }

    for (int i = vector->size - 1; i > index; i--)
    {
        vector->elemArray[i] = vector->elemArray[i - 1];
    }
    vector->elemArray[index] = elem;

    return 0;
}

err_t Vector_print(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    printf("\nsize = %zu\n", vector->size);
    for (int i = 0; i < vector->size; i++)
    {
        printElem(vector->elemArray + i);
    }

    return 0;
}

err_t Vector_resize(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    void* tmp = realloc(vector->elemArray, vector->capacity * 2);
    if (tmp == NULL)
        return MEM_ERR;
    vector->elemArray = tmp;

    return 0;
}