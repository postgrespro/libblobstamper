#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#include <list>

#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"
#include "blobstamper/helpers.h"


using namespace TAP;


/* Tests for very basic stamp operations */


/* This stamps chops first two bytes and treat them as string */
/* Never do this in real live, as blob is binary and may have \0 in the middle of it*/
class StampForTest: public StampGeneric
{
  public:
    StampForTest();
    std::string ExtractStr(Blob &blob) override;
};

StampForTest::StampForTest() : StampGeneric()
{
    min_size = 2;  /* This stamp shifts two characters only */
    max_size = 2;
    is_fixed_size = true;
}

std::string
StampForTest::ExtractStr(Blob &blob)
{
    char * buf;
    size_t buf_size;

    Blob blob2 = blob.ShiftBytes(min_size);
    if (blob2.isEmpty())
        return "";

    /* Save shited data as string */
    /* NEVER do this in prod, as in real live blob is binary and may have 0 in the middle of it */
    blob2.DataDup(buf, buf_size);
    buf = (char *) realloc((void *)buf, buf_size + 1);
    buf[buf_size] = '\0';
    std::string res = buf;
    free(buf);

    return res;
}


char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

char short_sample[]="1234567";

int
main()
{
    char *ptr;
    size_t size;

    TEST_START(7);

    /* Test that ShiftSingleStampStr shifts ok with StampForTest stamp */
    { /* 1..3 */
        std::string expected1 = "12";
        char* expected2 = "34567";


        Blob blob(short_sample, strlen(short_sample));
        StampForTest stamp;
        std::string str = blob.ShiftSingleStampStr(stamp);
        ok(str == expected1, "ShiftSingleStampStr: shifts ok");

        blob.DataDup(ptr,size);
        ok(size == strlen(expected2), "ShiftSingleStampStr: Remaining blob data size ok");
        ok(! memcmp(expected2, ptr, size), "ShiftSingleStampStr: Remaining blob data ok");
        free(ptr);
    }

    /* Test that StamList really splits the whole blob to the specified stamps */
    { /* 4..7 */
        std::string expected1 = "12";
        std::string expected2 = "34";
        std::string expected3 = "56";

        StampForTest stamp;
        StampList    stamp_list(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::list<std::string> res = stamp_list.ExtractStrList(blob);

        std::string str;

        str = res.front();
        ok(str == expected1, "ExtractStrList: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        ok(str == expected2, "ExtractStrList: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        ok(str == expected3, "ExtractStrList: Third element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "ExtractStrList: The rest of the list is empty");
    }

    TEST_END;
}
