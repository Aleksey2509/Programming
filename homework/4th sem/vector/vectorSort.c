#include "vector.h"

void Vector_QSort (VectorContainer* vectorCont, iter_t startIter, iter_t endIter, int (*comparator)(const void *, const void *));

void Vector_sort(VectorContainer* vectorCont, int (*comparator)(const void *, const void *))
{
    Vector_QSort(vectorCont, vectorCont->m->begin(vectorCont), vectorCont->m->end(vectorCont), comparator);
}

err_t Vector_swapElem(VectorContainer* vectorCont, iter_t* firstIter, iter_t* secondIter)
{
    if (!vectorCont->m->cmpIter(firstIter, secondIter))
    {
        return INVALID_ARG;
    }

    elem_t tmp = vectorCont->m->getElemIter(vectorCont, firstIter);

    vectorCont->m->setElemIter(vectorCont, firstIter, vectorCont->m->getElemIter(vectorCont, secondIter));
    vectorCont->m->setElemIter(vectorCont, secondIter, tmp);

    return 0;
}

void Vector_QSort (VectorContainer* vectorCont, iter_t startIter, iter_t endIter, int (*comparator)(const void *, const void *))
{
    vectorCont->m->incIter(&startIter);
    if (vectorCont->m->cmpIter(&startIter, &endIter) >= 0)
        return;
    else
        vectorCont->m->decIter(&startIter);

    iter_t i = startIter;
    iter_t j = endIter;
    iter_t pivotPos = (i + j) / 2;

    elem_t pivotElem = vectorCont->m->getElemIter(vectorCont, &pivotPos);

    do
    {   
        for (elem_t tmp = vectorCont->m->getElemIter(vectorCont, &i); (vectorCont->m->cmpIter(&i, &endIter) < 0) && (comparator( &tmp, &pivotElem ) < 0); )
        {
            vectorCont->m->incIter(&i);
            tmp = vectorCont->m->getElemIter(vectorCont, &i);
        }

        for (elem_t tmp = vectorCont->m->getElemIter(vectorCont, &j); (vectorCont->m->cmpIter(&j, &startIter) > 0) && (comparator( &tmp, &pivotElem ) > 0); )
        {
            vectorCont->m->decIter(&j);
            tmp = vectorCont->m->getElemIter(vectorCont, &j);
        }

        if (vectorCont->m->cmpIter(&i, &j) <= 0)
        {
            Vector_swapElem(vectorCont, &i, &j);
            vectorCont->m->decIter(&j);
            vectorCont->m->incIter(&i);
        }
    } while(i <= j);

    if (vectorCont->m->cmpIter(&i, &endIter) < 0)
    	Vector_QSort (vectorCont, i, endIter, comparator);
    if (vectorCont->m->cmpIter(&j, &startIter) > 0)
    {
        vectorCont->m->incIter(&j);
        Vector_QSort (vectorCont, startIter, j, comparator);
    }

}