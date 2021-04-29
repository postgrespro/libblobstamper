#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>

#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "pg_op_wrappers.h"
}

using namespace TAP;


/* Tests for postgres geo type stamps */

double sample_data[] =  {1, 2, 3, 4.2e01, 5, 6, 7};
 
int
main()
{
    char * char_sample_data = (char *) sample_data;
    size_t sample_data_size = sizeof(double) * 7;

    TEST_START(3);
    {
        char *res1, *res2;
        poly_contain_prepare(char_sample_data, sample_data_size, &res1, &res2);

        ok(!strcmp(res1, "(1, 2)"),"first argument of poly_contain_pt op");
        ok(!strcmp(res2, "((3, 42), (5, 6))"),"second argument of poly_contain_pt op");

        if (res1) free(res1);
        if (res2) free(res2);
    }

    {
        char *res;
        poly_center_prepare(char_sample_data, sample_data_size, &res);

        ok(!strcmp(res, "((1, 2), (3, 42), (5, 6))"),"the only argument of poly_center op");

        if (res) free(res);
    }

    TEST_END;
}
