#include "blob.h"
#include "stamp.h"
#include "stamp_atomic.h"


StampBinChar::StampBinChar() : StampFixed()
{
    size = sizeof(char);
}


std::string
StampStrUInt8::ExtractStr(Blob &blob)
{
    std::string res;
    char *pc = (char *)this->ExtractBin(blob);
    if (! pc)
        return "";
    res = std::to_string((int) *pc);
    free(pc);
    return res;
}
/* ---- */

StampBinInt16::StampBinInt16() : StampFixed()
{
    size = sizeof(short int);
}

std::string
StampStrUInt16::ExtractStr(Blob &blob)
{
    std::string res;
    unsigned short int *pi = (unsigned short int *)this->ExtractBin(blob);
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
    signed short int *pi = (signed short int *)this->ExtractBin(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

/* ---- */

StampBinInt32::StampBinInt32() : StampFixed()
{
    size = sizeof(int);
}

std::string
StampStrUInt32::ExtractStr(Blob &blob)
{
    std::string res;
    unsigned int *pi = (unsigned int *)this->ExtractBin(blob);
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
    signed int *pi = (signed int *)this->ExtractBin(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

/* ---- */

StampBinInt64::StampBinInt64() : StampFixed()
{
    size = sizeof(long long);
}

std::string
StampStrUInt64::ExtractStr(Blob &blob)
{
    std::string res;
    unsigned long long *pi = (unsigned long long *)this->ExtractBin(blob);
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
    signed long long *pi = (signed long long *)this->ExtractBin(blob);
    if (! pi)
        return "";
    res = std::to_string(*pi);
    free(pi);
    return res;
}

/* ---- */

StampBinDouble::StampBinDouble() : StampFixed()
{
    size = sizeof(double);
}

std::string
StampStrDouble::ExtractStr(Blob &blob)
{
    std::string res = "";
    double *pd = (double *)this->ExtractBin(blob);
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
