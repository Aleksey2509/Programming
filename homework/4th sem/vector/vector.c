#include "vector.h"
#include "vectorPrivate.h"

VectorContainer* Vector_create()
{
    VectorContainer* vectorContainer = malloc(sizeof(VectorContainer));
    if (vectorContainer == NULL)
        return NULL;

    vectorContainer->vector = malloc(sizeof(VectorData));
    if (vectorContainer->vector == NULL)
    {
        free(vectorContainer);
        return NULL;
    }

    vectorContainer->vector->elemArray = NULL;
    vectorContainer->vector->size = 0;
    vectorContainer->vector->capacity = 0;
    vectorContainer->m = &VectorMethods;

    return vectorContainer;
}

err_t Vector_init(void* vectorCont, size_t capacity, const elem_t* data)
{
    if ((vectorCont == NULL) || (capacity == 0))
        return INVALID_ARG;
    ((VectorContainer *)vectorCont)->m = &VectorMethods;
    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

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

err_t Vector_destroy(void* vectorCont)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

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

iter_t Vector_createIter(void* vectorCont, size_t index)
{

    if ((vectorCont == NULL) || (index < 0) || (index > ((VectorContainer* )vectorCont)->vector->size - 1))
        return INVALID_ARG;

    return index;
}

err_t Vector_setIter(void* vectorCont, iter_t* iterator, size_t index)
{
    vectorCont = (VectorContainer *)vectorCont;

    if ((vectorCont == NULL) || (index < 0) || (index > ((VectorContainer* )vectorCont)->vector->size - 1))
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

elem_t Vector_getElemIter(void* vectorCont, const iter_t* iterator)
{
    int index = *iterator;
    return ((VectorContainer* )vectorCont)->vector->elemArray[index];
}

err_t Vector_setElemIter(void* vectorCont, const iter_t* iterator, elem_t data)
{
    if ((vectorCont == NULL) || (iterator == NULL))
        return INVALID_ARG;

    ((VectorContainer* )vectorCont)->vector->elemArray[*iterator] = data;
    return 0;
}

int Vector_cmpIter(iter_t* firstIter, iter_t* secondIter)
{
    return (*firstIter) == (*secondIter);
}

//================================================================================================

size_t Vector_getSize(void* vectorCont)
{
    return ((VectorContainer* )vectorCont)->vector->size;
}

elem_t Vector_getElem(void* vectorCont, int index)
{
    if ((vectorCont == NULL) || (index < 0) || (index > (((VectorContainer* )vectorCont)->vector->size - 1)))
        return INVALID_ARG;

    return ((VectorContainer* )vectorCont)->vector->elemArray[index];
}

err_t Vector_pushBack(void* vectorCont, elem_t data)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vectorCont);
        if (err)
            return err;
    }

    vector->elemArray[vector->size] = data;
    vector->size++;

    return 0;
}

err_t Vector_popBack(void* vectorCont)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    vector->size--;
    return (vector->elemArray[vector->size]);
}

err_t Vector_pushFront(void* vectorCont, elem_t data)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vectorCont);
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

err_t Vector_popFront(void* vectorCont)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    int data = vector->elemArray[0];
    for (int i = 0; i < vector->size - 1; i++)
    {
        vector->elemArray[i] = vector->elemArray[i + 1];
    }

    return data;
}

err_t Vector_insertAfter(void* vectorCont, size_t index, elem_t elem)
{
    if ((vectorCont == NULL) || (index < 0) || (index > ((VectorContainer *)vectorCont)->vector->size - 1))
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vectorCont);
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

err_t Vector_insertBefore(void* vectorCont, size_t index, elem_t elem)
{
    vectorCont = (VectorContainer *)vectorCont;

    if ((vectorCont == NULL) || (index < 0) || (index > (((VectorContainer *)vectorCont)->vector->capacity - 1)))
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    if (vector->size == vector->capacity - 1)
    {
        int err = Vector_resize(vectorCont);
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

err_t Vector_print(void* vectorCont)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    printf("\nsize = %zu\n", vector->size);
    for (size_t i = 0; i < vector->size; i++)
    {
        printf("data[%zu] = ", i);
        printElem(vector->elemArray + i);
    }

    return 0;
}

err_t Vector_resize(void* vectorCont)
{
    if (vectorCont == NULL)
        return INVALID_ARG;

    VectorData* vector = ((VectorContainer *)vectorCont)->vector;

    void* tmp = realloc(vector->elemArray, vector->capacity * 2);
    if (tmp == NULL)
        return MEM_ERR;
    vector->elemArray = tmp;

    return 0;
}