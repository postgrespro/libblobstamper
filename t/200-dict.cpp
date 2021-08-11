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
       is(dict.size(), 3,"Dict size");
       is(dict.get(1), "two","Dict content");
    }

    TEST_END;
}
