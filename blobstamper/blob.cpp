

#include <cstring>

#include "blob.h"
#include "helpers.h"


#include "stamp.h"

Blob::Blob (char * data_in, int size_in)
{
    data = data_in;
    size = size_in;
    begin = 0;
    end = size -1; /* i.e. size=1 means begin=0 && end=0 */
}

bool
Blob::isEmpty ()
{
    if (! data) return true;
    return false;
}

void
Blob::Dump()
{
    int length = end - begin +1 ;
    hexdump(data + begin, length);
}

Blob
Blob::ShiftBytes(size_t n)
{
    if (begin + n - 1 > end)
    {
        Blob empty(NULL, -1);
        return empty; /* not enough data */
    }

    Blob new_blob(data, size);

    new_blob.begin = begin;   /* FIXME this should go private once */
    new_blob.end = begin + n - 1;

    begin += n;

    return new_blob;
}

size_t
Blob::Size()
{
  return end - begin + 1;
}

void *
Blob::ShiftSingleStampBin(StampGeneric& stmp)
{
    return stmp.Extract(*this);
}

std::string
Blob::ShiftSingleStampStr(StampGeneric& stmp)
{
    return stmp.ExtractStr(*this);
}

void
Blob::DataDup(char *& data_out, size_t& size_out)
{
  size_out = Size();
  data_out = (char *)malloc(size_out);
  //FIXME add out of memory check here!!!!
  memcpy(data_out, data + begin, size_out);
}


std::list<std::string>
GalleySeries::Extract(Blob &blob)
{
  std::list<std::string> res;

  if (stamp.isFixedSize())
  {
    while (1)
    {
      std::string el = blob.ShiftSingleStampStr(stamp);
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



