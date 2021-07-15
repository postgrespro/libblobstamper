
#include <string>
#include <list>

#include <string.h>

#include "blob.h"
#include "stamp.h"


/*************************************************************************************/

/* Generic Exrtact Bin function for fixed size stamp. In some cases we need just a chunk of raw blob data. */
/* Use this method in those cases */
std::vector<char>
StampFixed::ExtractBin(Blob &blob)
{
    Blob blob2 = blob.ShiftBytes(size);

    if (blob2.isEmpty())  /* original blob does not have enought data */
    {
        std::vector<char> empty(0);
        return empty;
    }
    return blob2.asVector();
}

