
#include <string>
#include <list>

#include <string.h>

#include "blob.h"
#include "stamp.h"


/*************************************************************************************/

void *
StampGeneric::Extract(Blob &blob)
{
    if (!is_fixed_size)
    {
       fprintf(stderr, "Something is really wrong. Default extract method does not support dynamic stamp size\n");
       return NULL;
    }
    Blob blob2 = blob.ShiftBytes(min_size);

    if (blob2.isEmpty())  /* original blob does not have enought data */
        return NULL;

    size_t res_size;
    char *res;
    blob2.DataDup(res,res_size);
    return (void *) res;
}


std::list<std::string> StampList::ExtractStrList(Blob &blob)
{
  std::list<std::string> res;

  if (target_stamp.isFixedSize())
  {
    while (1)
    {
      std::string el = blob.ShiftSingleStampStr(target_stamp);
      if (el.empty())
        break;
      res.push_back(el);
    }
  }
  else
  {
     printf("Not implemented yet!");
     exit(1);
  }

  return res;
}

