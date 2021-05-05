#include "blob.h"
#include "stamp.h"
#include "stamp_atomic.h"


StampBinChar::StampBinChar() : StampGeneric()
{
    min_size = sizeof(char);
    max_size = sizeof(char);
    is_fixed_size = true;
}


std::string
StampStrUInt8::ExtractStr(Blob &blob)
{
    std::string res;
    char *pc = (char *)this->Extract(blob);
    if (! pc)
        return "";
    res = std::to_string((int) *pc);
    free(pc);
    return res;
}
/* ---- */

StampBinInt16::StampBinInt16() : StampGeneric()
{
    min_size = sizeof(short int);
    max_size = sizeof(short int);
    is_fixed_size = true;
}

std::string
StampStrUInt16::ExtractStr(Blob &blob)
{
    std::string res;
    unsigned short int *pi = (unsigned short int *)this->Extract(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

std::string
StampStrSInt16::ExtractStr(Blob &blob)
{
    std::string res;
    signed short int *pi = (signed short int *)this->Extract(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

/* ---- */

StampBinInt32::StampBinInt32() : StampGeneric()
{
    min_size = sizeof(int);
    max_size = sizeof(int);
    is_fixed_size = true;
}

std::string
StampStrUInt32::ExtractStr(Blob &blob)
{
    std::string res;
    unsigned int *pi = (unsigned int *)this->Extract(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

std::string
StampStrSInt32::ExtractStr(Blob &blob)
{
    std::string res;
    signed int *pi = (signed int *)this->Extract(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

/* ---- */

StampBinInt64::StampBinInt64() : StampGeneric()
{
    min_size = sizeof(long long);
    max_size = sizeof(long long);
    is_fixed_size = true;
}

std::string
StampStrUInt64::ExtractStr(Blob &blob)
{
    std::string res;
    unsigned long long *pi = (unsigned long long *)this->Extract(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

std::string
StampStrSInt64::ExtractStr(Blob &blob)
{
    std::string res;
    signed long long *pi = (signed long long *)this->Extract(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

/* ---- */

StampBinDouble::StampBinDouble() : StampGeneric()
{
    min_size = sizeof(double);
    max_size = sizeof(double);
    is_fixed_size = true;
}

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
