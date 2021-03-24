#include <string.h>
#include <stdio.h>
#include <stdlib.h>



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
    char * str = malloc(length +1 ); // second +1 is for \0
    // FIXME проверка null
    str[0]='\0';



    strncat(str, blob->data + blob->begin, length); // second +1 is for \0

    printf("%s\n",str);
    free(str);
}

void* 
wflMalloc( size_t size )
{
   /*just that simple for now*/
  return malloc( size );
}


wflBlobDsc*
wflShiftN(wflBlobDsc* blob, size_t n)
{
    wflBlobDsc*  new_blob;
    // FIXME null check here;
    if (blob->begin + n > blob->end)
        return NULL; /*not enough data*/

    new_blob = wflMalloc(sizeof(wflBlobDsc));

    new_blob->data  = blob->data;
    new_blob->begin = blob->begin;
    new_blob->end = blob->begin + n - 1;

    blob->begin += n;

    return new_blob;
}



char blob_data[]="aaalkjdhfs89345yu3ifhjew;lkhf4;lt;o34ithp;eriuwtgp;etup568p34tuewritwe;rtgj;ewoty;4w85tyeihwritgzzz";

int main()
{
    wflMemCtx * mctx;
    wflBlobDsc blob;
    wflBlobDsc * b2;

    mctx = wflCreateMemCtx();

    blob.mctx = mctx;
    blob.data = blob_data;
    blob.begin = 0;
    blob.end = strlen(blob_data)-1;



    wflBlobDump(&blob);
printf("-----\n");
    b2 = wflShiftN(&blob,9993);


    wflBlobDump(&blob);
    wflBlobDump(b2);

    wflDestroyMemCtx(mctx);
}