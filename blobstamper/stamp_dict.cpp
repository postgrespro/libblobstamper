
#include "blob.h"
#include "stamp.h"
#include "stamp_atomic.h"
#include "stamp_dict.h"

StampGeneric&
StampDict::GuessStamp(DictBase & dict)
{
    if (dict.size()<= UCHAR_MAX+1)
    {
        stamp_max_value = UCHAR_MAX;
        return stamp8;
    }
    if (dict.size()<= USHRT_MAX+1)
    {
        stamp_max_value = USHRT_MAX;
        return stamp16;
    }
    if (dict.size()<= UINT_MAX+1)
    {
        stamp_max_value = UINT_MAX;
        return stamp32;
    }
    stamp_max_value = ULONG_MAX;
    return stamp64;
}

std::string
StampDict::ExtractStr(Blob &blob)
{
    unsigned long long index_oracle;

    /* Shifting index oracle according to size of dictionary index variable*/
    switch (stamp.minSize())
    {
       case 1:
       {
          unsigned char * i =  (unsigned char *) blob.ShiftSingleStampBin(stamp);
          index_oracle = * i;
          free(i);
          break;
       }
       case 2:
       {
          unsigned short int * i = (unsigned short int *) blob.ShiftSingleStampBin(stamp);
          index_oracle = * i;
          free(i);
          break;
       }
       case 4:
       {
          unsigned int * i = ( unsigned int *) blob.ShiftSingleStampBin(stamp);
          index_oracle = * i;
          free(i);
          break;
       }

       case 8:
       {
          unsigned long long * i = ( unsigned long long *) blob.ShiftSingleStampBin(stamp);
          index_oracle = * i;
          free(i);
          break;
       }
       default:
         printf("StampDict::ExtractStr: Something is really wrong\n");
         exit(1);
    }
    long long actual_index = ((double) index_oracle) / stamp_max_value * dict.size();
    if ( actual_index == dict.size())  actual_index--; /* If we hit the boundary step inside a bit*/
    return dict.get(actual_index);
}
