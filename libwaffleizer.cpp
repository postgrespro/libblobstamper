#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "libwaffleizer.h"

wflMemCtx static_ctx;

wflMemCtx *
wflCreateMemCtx()
{
  return &static_ctx;
}


void 
wflDestroyMemCtx(wflMemCtx * mctx)
{
}


void
wflBlobDump(wflBlobDsc* blob)
{
    int length = blob->end - blob->begin + 1 ; 
    char * str =(char *) malloc(length + 1); // second +1 is for \0
    // FIXME проверка null
    str[0]='\0';

    strncat(str, blob->data + blob->begin, length); 

    printf("%s\n",str);
    free(str);
}

void*
wflMalloc(wflMemCtx * mctx, size_t size)
{
   /*just that simple for now*/
  return malloc( size );
}

void
wflFree(wflMemCtx * mctx, void* ptr)
{
   /*just that simple for now*/
  free(ptr);
}


wflBlobDsc*
wflShiftN(wflBlobDsc* blob, size_t n)
{
    wflBlobDsc*  new_blob;
    // FIXME null check here;
    if (blob->begin + n > blob->end)
        return NULL; /*not enough data*/

    new_blob = (wflBlobDsc*) wflMalloc(blob->mctx, sizeof(wflBlobDsc));

    new_blob->data  = blob->data;
    new_blob->begin = blob->begin;
    new_blob->end = blob->begin + n - 1;
    new_blob->mctx = blob->mctx;

    blob->begin += n;

    return new_blob;
}

std::string
wflShiftDouble(wflBlobDsc* blob)
{
    int ret;
    double * d;
    char * resc;
    std::string res;


    wflBlobDsc * b2 = wflShiftN(blob, sizeof(double));
    if (! b2) return "";

    d = (double *)( (char*)b2->data + b2->begin);
    wflFree(blob->mctx, b2);

    int size_s = snprintf( nullptr, 0, "%.999g", *d) + 1;
    if (size_s <= 0)
    {
        printf("ai-ai-ai\n");
        return "";
    }

    resc =(char *) malloc(size_s);
    if (! resc)
    {
        printf("oh-oh-oh\n");
        return "";
    }

    ret = snprintf(resc,size_s,"%.999g", *d);
    if (ret <= 0)
    {
        printf("oi-oi-oi\n");
        free(resc);
        return "";
    }
    res = resc;
    free(resc);
    return res;
}

std::string
wflShiftPgPoint(wflBlobDsc* blob)
{
    std::string res = "";
    std::string x, y;

    x = wflShiftDouble(blob);
    if (x.empty()) return "";

    y = wflShiftDouble(blob);
    if (y.empty()) return "";

    res = (std::string) "(" + x +", " + y + ")";
    return res;
}


std::string
wflShiftPgPath(wflBlobDsc* blob)
{
    std::string res = "";
    std::string point;

    while (1)
    {
        point = wflShiftPgPoint(blob);
        if (point.empty())
            break;
        if (!res.empty()) res = res + ", ";
        res = res + point;
    }
    if (res.empty())
        return res;
    res = "(" + res + ")";
    return res;
}

extern "C" {
int
poly_contain_prepare(char* in, int in_size, char ** res1, char ** res2)
{
    char str[]="124567";

    wflMemCtx * mctx;
    wflBlobDsc blob;
    wflBlobDsc * b2;

    std::string r1, r2;

    mctx = wflCreateMemCtx();

    blob.mctx = mctx;
    blob.data = in;
    blob.begin = 0;
    blob.end = in_size;


    r1 = wflShiftPgPoint(&blob);

    if (r1.empty())
    {
      fprintf(stderr,"Problema\n");
      return 1;
    }

    r2 = wflShiftPgPath(& blob);

    if (r2.empty())
    {
      fprintf(stderr,"Problema\n");
      return 1;
    }

    *res1 = (char *) malloc(strlen(r1.c_str()) + 1);
    memcpy(*res1, r1.c_str(), strlen(r1.c_str())  + 1);

    *res2 = (char *) malloc(strlen(r2.c_str())+1);
    memcpy(*res2, r2.c_str(), strlen(r2.c_str())  + 1);

    return 0;
}

}