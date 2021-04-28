#include <string.h>

#include "blobstamper/blobstamper.h"

#include "blobstamper/helpers.h"

char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

int
main(void)
{
    Blob bl(my_data, strlen(my_data));

    StampStrPgPoint stmp_pg_point;

    std::string str = bl.ShiftSingleStampStr(stmp_pg_point);

    printf("======= %s\n", str.c_str());


    StampStrPgPolygon stmp_pg_poligon;

    str = bl.ShiftSingleStampStr(stmp_pg_poligon);

    printf("_________ %s\n", str.c_str());



    return 0;
}