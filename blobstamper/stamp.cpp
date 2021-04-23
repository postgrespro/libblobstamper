
#include <string>
#include <list>

#include "blob.h"
#include "stamp.h"

/*************************************************************************************/

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

