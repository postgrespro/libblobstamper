#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/dict.h"

class DictTest : public DictBase
{
  public:
     DictTest();
};

DictTest::DictTest()
{
  data = {"one","two","three"};
}

using namespace TAP;

/* Test that dict works as expected*/

int
main()
{
    TEST_START(2);

    { /* 1..2 */
       DictTest dict;
       ok(dict.size() == 3,"Dict size");
       ok(dict.get(1) == "two","Dict content");
    }

    TEST_END;
}
