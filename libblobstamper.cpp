#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <list>

#include "libblobstamper.h"


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

/* Borrowed from http://www.stahlworks.com/dev/index.php?tool=csc01 */
/* Code is not nice for support, better rewrite it */

void hexdump(void *pAddressIn, long  lSize)
{
 char szBuf[100];
 long lIndent = 1;
 long lOutLen, lIndex, lIndex2, lOutLen2;
 long lRelPos;
 struct { char *pData; unsigned long lSize; } buf;
 unsigned char *pTmp,ucTmp;
 unsigned char *pAddress = (unsigned char *)pAddressIn;

   buf.pData   = (char *)pAddress;
   buf.lSize   = lSize;

   while (buf.lSize > 0)
   {
      pTmp     = (unsigned char *)buf.pData;
      lOutLen  = (int)buf.lSize;
      if (lOutLen > 16)
          lOutLen = 16;

      // create a 64-character formatted output line:
      sprintf(szBuf, " >                            "
                     "                      "
                     "    %08lX", pTmp-pAddress);
      lOutLen2 = lOutLen;

      for(lIndex = 1+lIndent, lIndex2 = 53-15+lIndent, lRelPos = 0;
          lOutLen2;
          lOutLen2--, lIndex += 2, lIndex2++
         )
      {
         ucTmp = *pTmp++;

         sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
         if(!isprint(ucTmp))  ucTmp = '.'; // nonprintable char
         szBuf[lIndex2] = ucTmp;

         if (!(++lRelPos & 3))     // extra blank after 4 bytes
         {  lIndex++; szBuf[lIndex+2] = ' '; }
      }

      if (!(lRelPos & 3)) lIndex--;

      szBuf[lIndex  ]   = '\t';
      szBuf[lIndex+1]   = '\t';

      printf("%s\n", szBuf);

      buf.pData   += lOutLen;
      buf.lSize   -= lOutLen;
   }
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
    if (begin + n > end)
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


StampBinDouble::StampBinDouble() : StampGeneric()
{
    min_size = sizeof(double);
    max_size = sizeof(double);
    is_fixed_size = true;
}

void *
StampBinDouble::Extract(Blob &blob)
{
    Blob blob2 = blob.ShiftBytes(min_size);
    if (blob2.isEmpty())
        return NULL;
    void *res = malloc(min_size);
    memcpy(res, blob2.data + blob2.begin, min_size);
    return res;
}

/* ---- */

std::string
StampStrDouble::ExtractStr(Blob &blob)
{
    std::string res = "";
    double *pd = (double *)this->Extract(blob);
    if (! pd)
        return res;

    int size_s = snprintf( nullptr, 0, "%.999g", *pd) + 1;
    if (size_s <= 0)
    {
        printf("ai-ai-ai\n");
        return "";
    }

    char * resc =(char *) malloc(size_s);
    if (! resc)
    {
        printf("oh-oh-oh\n");
        return "";
    }

    int ret = snprintf(resc,size_s,"%.999g", *pd);
    if (ret <= 0)
    {
        printf("oi-oi-oi\n");
        free(resc);
        return "";
    }
    res = resc;
    free(resc);
    free(pd);
    return res;
}
/* ---- */

StampStrPgPoint::StampStrPgPoint() : StampGeneric()
{
    min_size = stamp_double.minSize() * 2;
    max_size = stamp_double.maxSize() * 2;
    is_fixed_size = true;
}


std::string
StampStrPgPoint::ExtractStr(Blob &blob)
{
    std::string res = "";
    std::string x, y;

    x = blob.ShiftSingleStampStr(stamp_double);
    if (x.empty()) return "";

    y = blob.ShiftSingleStampStr(stamp_double);
    if (y.empty()) return "";

    res = (std::string) "(" + x +", " + y + ")";
    return res;
}


std::string
StampStrPgPolygon::ExtractStr(Blob &blob)
{
    std::string res = "";
  std::list<std::string> list = ExtractStrList(blob);


  for (std::string point : list) {
        if (!res.empty()) res = res + ", ";
        res = res + point;
  }

  if (res.empty())
    return res;
  res = "(" + res + ")";
  return res;
}
