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

int Vector_init(Vector* vector, size_t capacity, const int* data)
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

int Vector_destroy(Vector* vector)
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

int Vector_pushBack(Vector* vector, const int data)
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

int Vector_popBack(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    vector->size--;
    return (vector->elemArray[vector->size]);
}

int Vector_pushFront(Vector* vector, const int data)
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

int Vector_popFront(Vector* vector)
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

int Vector_getElem(Vector* vector, int index)
{
    if ((vector == NULL) || (index < 0) || (index > (vector->capacity - 1)))
        return INVALID_ARG;
        
    int data = vector->elemArray[index];

    for (int i = index; i < (vector->size - 1); i++)
    {
        vector->elemArray[i] = vector->elemArray[i + 1];
    }

    return data;
}

int Vector_insertAfter(Vector* vector, int position, int elem)
{
    if ((vector == NULL) || (position < 0) || (position > (vector->capacity - 1)))
        return INVALID_ARG;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vector);
        if (err)
            return err;
    }

    for (int i = vector->size - 1; i > position + 1; i--)
    {
        vector->elemArray[i] = vector->elemArray[i - 1];
    }
    vector->elemArray[position + 1] = elem;

    return 0;
}

int Vector_insertBefore(Vector* vector, int position, int elem)
{
    if ((vector == NULL) || (position < 0) || (position > (vector->capacity - 1)))
        return INVALID_ARG;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vector);
        if (err)
            return err;
    }

    for (int i = vector->size - 1; i > position; i--)
    {
        vector->elemArray[i] = vector->elemArray[i - 1];
    }
    vector->elemArray[position] = elem;

    return 0;
}

int Vector_print(Vector* vector)
{
    if (vector == NULL)
        return INVALID_ARG;

    printf("\nsize = %zu\n", vector->size);
    for (int i = 0; i < vector->size; i++)
    {
        printf("elemArr[%d] = %d\n", i, vector->elemArray[i]);
    }

    return 0;
}

int Vector_resize(Vector* vector)
{
    void* tmp = realloc(vector->elemArray, vector->capacity * 2);
    if (tmp == NULL)
        return MEM_ERR;
    vector->elemArray = tmp;

    return 0;
}