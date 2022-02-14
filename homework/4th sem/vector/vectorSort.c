#include "vector.h"
#include "vectorPrivate.h"

void quickSort (elem_t* base, size_t nElem, size_t elemSize, int (*comparator)(const void *, const void *))
{
    if (nElem < 2)
    return;

    int i = 0;
    int j = nElem - 1;
    int pivotPos = (i + j) / 2;

    int* pivot = base + pivotPos;

    do
    {   
        //printf("i = %d, j = %d\n", i, j);
        while( (i < nElem - 1) && (comparator( base + i * elemSize, pivot ) < 0) )
            i++;
        while( (j > 0) && (comparator( base + j * elemSize, pivot ) > 0) )
            j--;
        if (i <= j)
        {
            //printf("SWAAAPING %.*s, %.*s\n", ((line*)base)[i].lineLen, ((line*)base)[i].line, ((line*)base)[j].lineLen, ((line*)base)[j].line);
            int tmp = base[i];
            base[i] = base[j];
            base[j] = tmp;
            j--;
            i++;
        }
        //printf("at the end of cycle: i = %d, j = %d\n\n", i, j);
    }while(i <= j);

    if (i < nElem)
    	quickSort (base + i * elemSize, nElem - i, elemSize, comparator);
    if (j > 0)
        quickSort (base, j + 1, elemSize, comparator);

}

void Vector_sort(Vector* vector, int (*comparator)(const void *, const void *))
{
    quickSort(vector->elemArray, vector->size, sizeof(int), comparator);
}