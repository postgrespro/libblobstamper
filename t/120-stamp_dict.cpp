/******************************************************************************
 *
 * Copyright 2021 Nikolay Shaplov (Postgres Professional)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/dict.h"
#include "blobstamper/stamp_dict.h"

class DictTest : public DictBase
{
  public:
     DictTest();
};

DictTest::DictTest()
{
  data = {"one","two","three","four"};
}

using namespace TAP;

/* Test that dict works as expected*/
unsigned char sample[]={0,63,64,255};

int
main()
{
    TEST_START(4);
    { /* 1..4 */
       DictTest dict;
       StampDict stamp(dict);
       Blob blob((char *) sample, 4);
       std::string s;

       s =  blob.ShiftSingleStampStr(stamp);
       is(s, "one", "0 stamps as first element of dict");

       s =  blob.ShiftSingleStampStr(stamp);
       is(s, "one", "63 still stamps as first element of dict, when dict has only four elements");

       s =  blob.ShiftSingleStampStr(stamp);
       is(s, "two", "64 stamps as second element of dict");

       s =  blob.ShiftSingleStampStr(stamp);
       is(s, "four", "255 stamps as last (fourth) element of dict");

    }
    TEST_END;
}
