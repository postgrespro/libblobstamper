#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>


#include "blobstamper/blobstamper.h"

#include "two-chars-stamp.h"

using namespace TAP;

/* Test that dict works as expected*/

char short_sample[]="1234567";

int
main()
{
    TEST_START(4);
    { /* 1..4 */
        std::string expected1 = "12";
        std::string expected2 = "34";
        std::string expected3 = "56";

        StampTwoChars stamp;
        GalleySeries galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::list<std::string> res = galley.Extract(blob);

        std::string str;

        str = res.front();
        ok(str == expected1, "GalleySeries: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        ok(str == expected2, "GalleySeries: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        ok(str == expected3, "GalleySeries: Third element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries: The rest of the list is empty");
    }

    TEST_END;
}
