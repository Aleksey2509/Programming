#include "vector.h"

void Vector_QSort (Vector* vector, size_t startIndex, size_t endIndex, int (*comparator)(const void *, const void *));

void Vector_sort(Vector* vector, int (*comparator)(const void *, const void *))
{
    Vector_QSort(vector, 0, Vector_getSize(vector) - 1, comparator);
}

err_t Vector_swapElem(Vector* vector, size_t firstIndex, size_t secondIndex)
{
    if (firstIndex == secondIndex)
        return INVALID_ARG;
    
    iter_t firstIter = Vector_createIter(vector, firstIndex);
    iter_t secondIter = Vector_createIter(vector, secondIndex);

    elem_t tmp = Vector_getElemIter(vector, &firstIter);

    Vector_setElemIter(vector, &firstIter, Vector_getElemIter(vector, &secondIter));
    Vector_setElemIter(vector, &secondIter, tmp);

    return 0;
}

void Vector_QSort (Vector* vector, size_t startIndex, size_t endIndex, int (*comparator)(const void *, const void *))
{
    if ((endIndex - startIndex) < 1)
    return;

    size_t i = startIndex;
    size_t j = endIndex;
    size_t pivotPos = (startIndex + endIndex) / 2;

    elem_t pivotElem = Vector_getElem(vector, pivotPos);

    do
    {   
        for (elem_t tmp = Vector_getElem(vector, i); (i < endIndex) && (comparator( &tmp, &pivotElem ) < 0);)
        {
            i++;
            tmp = Vector_getElem(vector, i);
        }

        for (elem_t tmp = Vector_getElem(vector, j); (j > startIndex) && (comparator( &tmp, &pivotElem ) > 0);)
        {
            j--;
            tmp = Vector_getElem(vector, j);
        }

        if (i <= j)
        {
            Vector_swapElem(vector, i, j);
            j--;
            i++;
        }
    } while(i <= j);

    if (i < endIndex)
    	Vector_QSort (vector, i, endIndex, comparator);
    if (j > startIndex)
        Vector_QSort (vector, startIndex, j + 1 + startIndex, comparator);

}


//==============================================================================================================================
// version through iterators: could not implement; because have no clue how to correctly compare iterators with logical indexes
//==============================================================================================================================

void Vector_QSortIters (Vector* vector, size_t startIndex, size_t endIndex, int (*comparator)(const void *, const void *))
{
    if ((endIndex - startIndex) < 1)
    return;

    iter_t i = Vector_createIter(vector, startIndex);
    iter_t j = Vector_createIter(vector, endIndex);
    size_t pivotPos = (startIndex + endIndex) / 2;

    elem_t pivotElem = Vector_getElem(vector, pivotPos);
    

    do
    {   
        elem_t tmp;
        for (tmp = Vector_getElemIter(vector, &i); (i < endIndex) && (comparator( &tmp, &pivotElem ) < 0);)
        {
            Vector_incIter(&i);
            tmp = Vector_getElemIter(vector, &i);
        }

        for (tmp = Vector_getElemIter(vector, &j); (j > startIndex) && (comparator( &tmp, &pivotElem ) > 0);)
        {
            Vector_decIter(&j);
            tmp = Vector_getElemIter(vector, &j);
        }

        if (Vector_cmpIter(&i, &j) <= 0) // do not know how to correctly do comparing of logical indexes of iterators
        {
            Vector_swapElem(vector, i, j);
            Vector_decIter(&j);
            Vector_incIter(&i);
        }
    } while(Vector_cmpIter(&i, &j) <= 0);

    if (i < endIndex) // do not know how to correctly do comparing of logical indexes of iterators
    	Vector_QSort (vector, i, endIndex, comparator);
    if (j > startIndex) // do not know how to correctly do comparing of logical indexes of iterators
        Vector_QSort (vector, startIndex, j + 1 + startIndex, comparator);

}