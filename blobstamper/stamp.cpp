
#include <string>
#include <list>

#include <string.h>

#include "blob.h"
#include "stamp.h"


/*************************************************************************************/

/* Generic Exrtact Bin function for fixed size stamp. In some cases we need just a chunk of raw blob data. */
/* Use this method in such a case */
void *
StampFixed::ExtractBin(Blob &blob)
{
    Blob blob2 = blob.ShiftBytes(size);

    if (blob2.isEmpty())  /* original blob does not have enought data */
        return NULL;

    size_t res_size;
    char *res;
    blob2.DataDup(res,res_size);
    return (void *) res;
}

