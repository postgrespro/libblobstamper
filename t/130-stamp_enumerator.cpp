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

#include "blobstamper/stamp_arithm.h"
#include "blobstamper/stamp_enumerator.h"


using namespace TAP;

unsigned char sample[]={1,63,64,255};

int
main()
{
    TEST_START(1);
    { /* 1..1 */
        Blob blob((char *) sample, sizeof(sample));
        StampArithm<unsigned char> base_stamp;
        StampStrEnumerator stamp(base_stamp, "; ", "<", ">");

        std::string s = stamp.ExtractStr(blob);

        is(s, "<1; 63; 64; 255>" , "The only test");

    }
    TEST_END;
}
