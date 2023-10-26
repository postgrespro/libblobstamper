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


#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/oracle.h"

using namespace TAP;

int
main()
{
    TEST_START(6);
    {
       is(OracleProportion(0,0,255), 0);
       is(OracleProportion(255,0,255), 0);
       is(OracleProportion(256,0,255), 1);
       is(OracleProportion(65535,0,255), 255);
       is(OracleProportion(65535-255,0,255), 255);
       is(OracleProportion(65535-256,0,255), 254);

    }
    TEST_END;
}
