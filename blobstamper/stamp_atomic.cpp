
#include <string.h>

#include "blob.h"
#include "stamp.h"
#include "stamp_atomic.h"

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