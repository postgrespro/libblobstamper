
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
    std::vector<char> res = blob.ChopBlank(*this);
    return res;
}

