#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>


typedef struct wflMemCtx
{
    //Nothing for now;
}wflMemCtx ;


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

typedef struct wflBlobDsc
{
  struct wflMemCtx * mctx;
  char * data;
  int    begin;
  int    end;
}wflBlobDsc;


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
wflMalloc(wflMemCtx * mctx, size_t size )
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

char *
wflShiftDouble(wflBlobDsc* blob)
{
    char buf[1000];
    int ret, length;
    double * d;
    char * res;
    wflBlobDsc * b2 = wflShiftN(blob, sizeof(double));
    if (! b2) return NULL;

    d = (double *)( (char*) b2->data + blob->begin);

    ret = snprintf(buf,1000,"%.999g",*d);
    // FIXME анализировать ret
    length = strlen(buf);
    res = (char*) wflMalloc(blob->mctx,length+1);
    memcpy(res,buf,length+1);
    wflFree(blob->mctx, b2);
    return res;
}

char *
wflShiftPgPoint(wflBlobDsc* blob)
{
    char buf[1000];
    int ret, length;
    char *res;
    char *a1, *a2;

    a1 = wflShiftDouble(blob);
    if (! a1) return NULL;

    a2 = wflShiftDouble(blob);
    if (! a2) return NULL;
    
    ret = snprintf(buf,1000,"(%s, %s)",a1, a2);
    // FIXME анализировать ret

    length = strlen(buf);
    res = (char*) wflMalloc(blob->mctx,length+1);
    memcpy(res,buf,length + 1);
    wflFree(blob->mctx, a1);
    wflFree(blob->mctx, a2);

    return res;
}


char *
wflShiftPgPath(wflBlobDsc* blob)
{
    std::string res = "";
    char *pc, *pres;

    while (pc = wflShiftPgPoint(blob))
    {
        if (!res.empty()) res = res + ", ";
        res = res + pc;
        wflFree(blob->mctx, pc);
    }
    res = "[" + res + "]";
    pres = (char*) wflMalloc(blob->mctx, res.size() + 1);
    memcpy(pres,res.c_str(), res.size() + 1);
    return pres;

}

char blob_data[]="aaalkjdhfs89345yu3ifhjew;lkhf4;lt;o34ithp;eriuwtgp;etup568p34tuewritwe;rtgj;ewoty;4w85tyeihwritgzzz";

int main()
{
    wflMemCtx * mctx;
    wflBlobDsc blob;
    wflBlobDsc * b2;
    char* str;

    mctx = wflCreateMemCtx();

    blob.mctx = mctx;
    blob.data = blob_data;
    blob.begin = 0;
    blob.end = strlen(blob_data)-1;


    wflBlobDump(&blob);
printf("-----\n");
    b2 = wflShiftN(&blob,3);

    str = wflShiftDouble(&blob);

printf("\n aaa = %s\n-----\n",str);

    str = wflShiftPgPoint(&blob);

printf("\n aaa = %s\n-----\n",str);

    str = wflShiftPgPath(&blob);

printf("\n zzz = %s\n-----\n",str);

    wflBlobDump(&blob);
    wflBlobDump(b2);

    wflDestroyMemCtx(mctx);
}