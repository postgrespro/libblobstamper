#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <list>

#include "libblobstamper.h"


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
