#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>


#include "blobstamper/blobstamper.h"

#include "two-chars-stamp.h"

using namespace TAP;

class StampTwoCharsList: public StampGeneric
{
  protected:
    StampTwoChars el_stamp;
    GalleySeries galley;
  public:
    std::string ExtractStr(Blob &blob) override;
    StampTwoCharsList(): el_stamp {}, galley {el_stamp} {};
    virtual bool isFixedSize() override {return false;};
    virtual int maxSize() override {return -1;};
    virtual int minSize() override {return el_stamp.minSize();};
};

std::string
StampTwoCharsList::ExtractStr(Blob &blob)
{
    std::string res = "";
    std::list<std::string> list = galley.Extract(blob);

    for (std::string point : list)
    {
        if (!res.empty()) res = res + ", ";
        res = res + point;
    }

    if (res.empty())  return "";

    res = "(" + res + ")";
    return res;
}


char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";

int
main()
{
    TEST_START(9);
    /* Test Galley Sereies with fixed size stampp*/
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
        is(str, expected1, "GalleySeries, fixed size stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, fixed size stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, fixed size stamp: Third element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, fixed size stamp: The rest of the list is empty");
    }
    /* Test Galley Sereies with unlimited size stampp*/
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

        std::list<std::string> res = galley.Extract(blob);
        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, unlimited size stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, unlimited size stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, unlimited size stamp: Third element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected4, "GalleySeries, unlimited size stamp: Fourth element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, unlimited size stamp: The rest of the list is empty");
    }

    TEST_END;
}
