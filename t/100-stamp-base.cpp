#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#include <list>

#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"
#include "blobstamper/helpers.h"

#include "two-chars-stamp.h"

using namespace TAP;


/* Tests for very basic stamp operations */

char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

char short_sample[]="1234567";

int
main()
{
    char *ptr;
    size_t size;

    TEST_START(4);

    /* Test that ShiftSingleStampStr shifts ok with StampTwoChars stamp */
    { /* 1..3 */
        std::string expected1 = "12";
        char* expected2 = "34567";


        Blob blob(short_sample, strlen(short_sample));
        StampTwoChars stamp;
        std::string str = blob.ShiftSingleStampStr(stamp);
        ok(str == expected1, "ShiftSingleStampStr: shifts ok");

        blob.DataDup(ptr,size);
        ok(size == strlen(expected2), "ShiftSingleStampStr: Remaining blob data size ok");
        ok(! memcmp(expected2, ptr, size), "ShiftSingleStampStr: Remaining blob data ok");
        free(ptr);
    }

    /* Chekc that data is shifted till blob data is empty*/
    {   /* 8 */
        char sample_two_bytes[]="12";
        std::string expected1 = "12";
        Blob blob(sample_two_bytes, strlen(sample_two_bytes));
        StampTwoChars stamp;
        std::string str = blob.ShiftSingleStampStr(stamp);
        ok(str == expected1, "ShiftSingleStampStr: shifts last two bytes ok");
    }

    TEST_END;
}
