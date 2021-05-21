#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"

#include "test-chars-stamps.h"

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";

int
main()
{
    TEST_START(18);
    /* Test Galley Sereies with fixed size stampp*/
    { /* 1..4 */
        std::string expected1 = "12";
        std::string expected2 = "34";
        std::string expected3 = "56";

        StampTwoChars stamp;
        GalleySeries galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::list<std::string> res = galley.ExtractStr(blob);

        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, fixed size string stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, fixed size string  stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, fixed size string  stamp: Third element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, fixed size string stamp: The rest of the list is empty");
    }
    /* Test Galley Sereies with unlimited size stamp*/
    {  /* 5 .. 9*/
        /* This is not the best test, as we do not predict behavior by setting forged sample values,
          but at least here we check that it work the same way it worked before. May be this test should be improved later*/

        std::string expected1 = "(9a, bc, de, &f, gh, ij)";
        std::string expected2 = "(lm, no, pq, rs, tu, vw, xy)";
        std::string expected3 = "(zA, B%, CD, EF, GH, IJ, KL)";
        std::string expected4 = "(MN, OP, QR, ST, UV, WX, YZ)";

        Blob blob(longer_sample, strlen(longer_sample));
        StampTwoCharsList stamp_charlist;
        GalleySeries galley(stamp_charlist);

        std::list<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, unlimited size string stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, unlimited size string stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, unlimited size string stamp: Third element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected4, "GalleySeries, unlimited size string stamp: Fourth element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, unlimited size string stamp: The rest of the list is empty");
    }

    { /* 10..13 */

        unsigned short int expected1 = (unsigned char) '2' * 256 +(unsigned char) '1';
        unsigned short int expected2 = (unsigned char) '4' * 256 +(unsigned char) '3';
        unsigned short int expected3 = (unsigned char) '6' * 256 +(unsigned char) '5';

        StampBinInt16 stamp;
        GalleySeries galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::list<void * > res = galley.ExtractBin(blob);

        unsigned short int * data;

        data = (unsigned short int *) res.front();
        is(*data, expected1, "GalleySeries, fixed size binary stamp: First element of shifted list is ok");
        res.pop_front();
        free(data);

        data = (unsigned short int *) res.front();
        is(*data, expected2, "GalleySeries, fixed size binary stamp: Second element of shifted list is ok");
        res.pop_front();
        free(data);

        data = (unsigned short int *) res.front();
        is(*data, expected3, "GalleySeries, fixed size binary stamp: Third element of shifted list is ok");
        res.pop_front();
        free(data);

        ok(res.empty(), "GalleySeries, fixed size binary stamp: The rest of the list is empty");
    }

    /* Test Galley Sereies with variated size stamp*/
    {  /* 14 .. 18*/
        /* This is not the best test, as we do not predict behavior by setting forged sample values,
          but at least here we check that it work the same way it worked before. May be this test should be improved later*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "234";
        std::string expected2 = "7*8";
        std::string expected3 = "bcde";
        std::string expected4 = "ghij";

        Blob blob(sample, strlen(sample));
        StampSeveralChars stamp;
        GalleySeries galley(stamp);

        std::list<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, unlimited size string stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, unlimited size string stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, unlimited size string stamp: Third element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected4, "GalleySeries, unlimited size string stamp: Fourth element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, unlimited size string stamp: The rest of the list is empty");
    }




    TEST_END;
}
