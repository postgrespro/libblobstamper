#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"
#include "blobstamper/helpers.h"


using namespace TAP;

char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

char short_sample[]="123456";

int
main()
{
    char *ptr;
    size_t size;

    TEST_START(7);

    { /* 1..2 */
        /* Check that DataDup gives us the same data we've just added to blob */
        Blob blob(my_data, strlen(my_data));
        blob.DataDup(ptr,size);
        ok(! memcmp(my_data, ptr, size), "Blob gets and returns blob data ok");
        ok(size == strlen(my_data), "Blob gets and returns blob data with expected size");
        free(ptr);
    }
    { /* 3..6 */
        /* Check that Shift bytes behave as expected*/
        char expected1[]="456";
        char expected2[]="123";

        Blob blob1(short_sample,strlen(short_sample));
        Blob blob2 = blob1.ShiftBytes(3);

        blob1.DataDup(ptr,size);
        ok(size == strlen(expected1), "Blob shifted data size ok");
        ok(! memcmp(expected1, ptr, size), "Blob shifted data ok");
        free(ptr);

        blob2.DataDup(ptr,size);
        ok(size == strlen(expected2), "Blob remained data size ok");
        ok(! memcmp(expected2, ptr, size), "Blob remained data ok");
        free(ptr);
    }
    { /* 7 */
        /* Check that shifting too many bytes return empty result */
        Blob blob(my_data, strlen(my_data));
        try
        {
          Blob blob_res = blob.ShiftBytes(99999);
          ok(false, "Shift too many bytes");
        }
        catch (OutOfData)
        {
          ok(true, "Shift too many bytes");
        }
        catch (...) //Any othe exeption
        {
          ok(false, "Shift too many bytes");
        }
    }



    TEST_END;
}
