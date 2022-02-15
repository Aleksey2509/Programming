#include "vector.h"

void Vector_QSort (VectorContainer* vectorCont, size_t startIndex, size_t endIndex, int (*comparator)(const void *, const void *));

void Vector_sort(VectorContainer* vectorCont, int (*comparator)(const void *, const void *))
{
    Vector_QSort(vectorCont, 0, vectorCont->m->getSize(vectorCont) - 1, comparator);
}

err_t Vector_swapElem(VectorContainer* vectorCont, size_t firstIndex, size_t secondIndex)
{
    if (firstIndex == secondIndex)
        return INVALID_ARG;
    
    iter_t firstIter = vectorCont->m->createIter(vectorCont, firstIndex);
    iter_t secondIter = vectorCont->m->createIter(vectorCont, secondIndex);

    elem_t tmp = vectorCont->m->getElemIter(vectorCont, &firstIter);

    vectorCont->m->setElemIter(vectorCont, &firstIter, vectorCont->m->getElemIter(vectorCont, &secondIter));
    vectorCont->m->setElemIter(vectorCont, &secondIter, tmp);

    return 0;
}

void Vector_QSort (VectorContainer* vectorCont, size_t startIndex, size_t endIndex, int (*comparator)(const void *, const void *))
{
    if ((endIndex - startIndex) < 1)
    return;

    size_t i = startIndex;
    size_t j = endIndex;
    size_t pivotPos = (startIndex + endIndex) / 2;

    elem_t pivotElem = vectorCont->m->getElem(vectorCont, pivotPos);

    do
    {   
        for (elem_t tmp = vectorCont->m->getElem(vectorCont, i); (i < endIndex) && (comparator( &tmp, &pivotElem ) < 0);)
        {
            i++;
            tmp = vectorCont->m->getElem(vectorCont, i);
        }

        for (elem_t tmp = vectorCont->m->getElem(vectorCont, j); (j > startIndex) && (comparator( &tmp, &pivotElem ) > 0);)
        {
            j--;
            tmp = vectorCont->m->getElem(vectorCont, j);
        }

        if (i <= j)
        {
            Vector_swapElem(vectorCont, i, j);
            j--;
            i++;
        }
    } while(i <= j);

    if (i < endIndex)
    	Vector_QSort (vectorCont, i, endIndex, comparator);
    if (j > startIndex)
        Vector_QSort (vectorCont, startIndex, j + 1 + startIndex, comparator);

}


//==============================================================================================================================
// version using iterators: could not implement; because have no clue how to correctly compare iterators 
// by logical indexes of their corresponding elements
//==============================================================================================================================

// void Vector_QSortIters (VectorContainer* vector, size_t startIndex, size_t endIndex, int (*comparator)(const void *, const void *))
// {
//     if ((endIndex - startIndex) < 1)
//     return;

//     iter_t i = Vector_createIter(vector, startIndex);
//     iter_t j = Vector_createIter(vector, endIndex);
//     size_t pivotPos = (startIndex + endIndex) / 2;

//     elem_t pivotElem = Vector_getElem(vector, pivotPos);
    

//     do
//     {   
//         elem_t tmp;
//         for (tmp = Vector_getElemIter(vector, &i); (i < endIndex) && (comparator( &tmp, &pivotElem ) < 0);)
//         {
//             Vector_incIter(&i);
//             tmp = Vector_getElemIter(vector, &i);
//         }

//         for (tmp = Vector_getElemIter(vector, &j); (j > startIndex) && (comparator( &tmp, &pivotElem ) > 0);)
//         {
//             Vector_decIter(&j);
//             tmp = Vector_getElemIter(vector, &j);
//         }

//         if (Vector_cmpIter(&i, &j) <= 0) // do not know how to correctly do comparing of logical indexes of iterators
//         {
//             Vector_swapElem(vector, i, j);
//             Vector_decIter(&j);
//             Vector_incIter(&i);
//         }
//     } while(Vector_cmpIter(&i, &j) <= 0);

//     if (i < endIndex) // do not know how to correctly do comparing of logical indexes of iterators
//     	Vector_QSort (vector, i, endIndex, comparator);
//     if (j > startIndex) // do not know how to correctly do comparing of logical indexes of iterators
//         Vector_QSort (vector, startIndex, j + 1 + startIndex, comparator);

// }