#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>

#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"
#include "blobstamper/helpers.h"


using namespace TAP;


/* Tests for atomic type stamps */



double sample_data[] =  {1.4142, 2, 3.1415, 4.2e01, 5, 6, 7};
 
char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

char short_sample[]="1234567";

int
main()
{
    char * char_sample_data = (char *) sample_data;
    size_t sample_data_size = sizeof(double) * 7;

    TEST_START(2);

    /* Check that Bin Double stamp works well */
    { /* 1 */
        double * d;
        Blob blob(char_sample_data, sample_data_size);
        StampBinDouble stamp;
        d = (double *) blob.ShiftSingleStampBin(stamp);
        ok(*d == 1.4142, "Bin Double stamp works well");
        free(d);
    }

    /* Check that Str Double stamp works well */
    { /* 2 */
        Blob blob(char_sample_data, sample_data_size);
        StampStrDouble stamp;
        std::string res = blob.ShiftSingleStampStr(stamp);
        res = blob.ShiftSingleStampStr(stamp);
        ok(res == "2", "Str Double stamp works well");
    }
    TEST_END;
}
