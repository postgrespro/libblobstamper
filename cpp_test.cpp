#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>

#include "libblobstamper.h"


char blob_data[]="aaalkjdhfs89345yu3ifhjew;lkhf4;lt;o34ithp;eriuwtgp;etup568p34tuewritwe;rtgj;ewoty;4w85tyeihwritgzzz";

int main()
{
    char *res1, *res2;

    poly_contain_prepare(blob_data,strlen(blob_data),&res1, &res2);

  
    printf("---- %s\n",res1);
    printf("==== %s\n",res2);


    if (res1) free(res1);
    if (res2) free(res2);

}