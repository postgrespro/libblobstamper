#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "libblobstamper.h"


Blob::Blob (char * data_in, int size_in)
{
    data = data_in;
    size = size_in;
    begin = 0;
    end = size;
}

bool
Blob::isEmpty ()
{
    if (! data) return true;
    return false;
}

void
wflBlobDump(Blob blob)
{
    int length = blob.end - blob.begin + 1 ;
    char * str =(char *) malloc(length + 1); // second +1 is for \0
    // FIXME проверка null
    str[0]='\0';

    strncat(str, blob.data + blob.begin, length);

    printf("%s\n",str);
    free(str);
}

Blob
wflShiftN(Blob &blob, size_t n)
{
    if (blob.begin + n > blob.end)
    {
        Blob empty(NULL, -1);
        return empty; /*not enough data*/
    }

    Blob new_blob(blob.data, blob.size);

    new_blob.begin = blob.begin;
    new_blob.end = blob.begin + n - 1;

    blob.begin += n;

    return new_blob;
}

std::string
wflShiftDouble(Blob &blob)
{
    int ret;
    double * d;
    char * resc;
    std::string res;


    Blob b2 = wflShiftN(blob, sizeof(double));
    if (b2.isEmpty()) return "";

    d = (double *)( (char*)b2.data + b2.begin);

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
wflShiftPgPoint(Blob &blob)
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
wflShiftPgPath(Blob &blob)
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
    *res1 = NULL;
    *res2 = NULL;

    std::string r1, r2;
    Blob blob(in, in_size);

    r1 = wflShiftPgPoint(blob);

    if (r1.empty())
    {
      fprintf(stderr,"Problema1\n");
      return 1;
    }

    r2 = wflShiftPgPath(blob);

    if (r2.empty())
    {
      fprintf(stderr,"Problema2\n");
      return 1;
    }

    *res1 = (char *) malloc(strlen(r1.c_str()) + 1);
    memcpy(*res1, r1.c_str(), strlen(r1.c_str())  + 1);

    *res2 = (char *) malloc(strlen(r2.c_str())+1);
    memcpy(*res2, r2.c_str(), strlen(r2.c_str())  + 1);

    return 0;
}

int
poly_center_prepare(char* in, int in_size, char ** res2)
{
    *res2 = NULL;

    std::string  r2;
    Blob blob(in, in_size);

    r2 = wflShiftPgPath(blob);

    if (r2.empty())
    {
      fprintf(stderr,"Problema2\n");
      return 1;
    }

    *res2 = (char *) malloc(strlen(r2.c_str())+1);
    memcpy(*res2, r2.c_str(), strlen(r2.c_str())  + 1);

    return 0;
}

}