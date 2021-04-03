#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "libwaffleizer.h"


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