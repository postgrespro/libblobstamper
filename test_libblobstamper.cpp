#include <string.h>

#include "libblobstamper.h"

char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

int
main(void)
{
    Blob bl(my_data, strlen(my_data));

    printf("Original blob:\n");

    bl.Dump();

    printf("\n Now shifting 3 bytes.\n");
    Blob bl2 = bl.ShiftBytes(3);

    printf("Bytes shifted:\n");
    bl2.Dump();

    printf("\n Remaining blob:\n");
    bl.Dump();

    printf("\n Now shifting too many bytes.\n");

    Blob bl3 = bl.ShiftBytes(99999);

    if (bl3.isEmpty())
        printf("Got empty result. All is right\n");
    else
        printf("got non-empty result. SOMETHING IS REALLY WRONG!!!!\n");


    StampBinDouble stmp_double;

    void* res = bl.ShiftSingleStampBin(stmp_double);

    hexdump(res,sizeof(double));
    free(res);

    StampStrDouble stmp_str_double;
    
    std::string str = stmp_str_double.ExtractStr(bl);

    printf("-------- %s\n", str.c_str());

    str = wflShiftPgPoint(bl);

    printf("++++++++ %s\n", str.c_str());

    return 0;
}