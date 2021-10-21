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
#include <list>

#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"
#include "blobstamper/helpers.h"

#include "test-chars-stamps.h"

using namespace TAP;


/* Tests for very basic stamp operations */

char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

char short_sample[]="1234567";

int
main()
{
    char *ptr;
    size_t size;

    TEST_START(10);

    /* Test that ShiftSingleStampStr shifts ok with StampTwoChars stamp */
    { /* 1..3 */
        std::string expected1 = "12";
        char expected2[] = "34567";


        Blob blob(short_sample, strlen(short_sample));
        StampTwoChars stamp;
        std::string str = stamp.ExtractStr(blob);
        is(str, expected1, "ShiftSingleStampStr: shifts ok");

        blob.DataDup(ptr,size);
        ok(size == strlen(expected2), "ShiftSingleStampStr: Remaining blob data size ok");
        ok(! memcmp(expected2, ptr, size), "ShiftSingleStampStr: Remaining blob data ok");
        free(ptr);
    }

    /* Check that data is shifted till blob data is empty*/
    {   /* 4 */
        char sample_two_bytes[]="12";
        std::string expected1 = "12";
        Blob blob(sample_two_bytes, strlen(sample_two_bytes));
        StampTwoChars stamp;
        std::string str = stamp.ExtractStr(blob);
        is(str, expected1, "ShiftSingleStampStr: shifts first two bytes ok");
    }
    /* Checks for variated size stamps*/

    {  /* 5,6 */
       char sample[]="1234567890";
       Blob blob(sample, strlen(sample));
       StampSeveralChars stamp; /* accepts from 2 to 8 bytes*/

       /* If used alone, is shifts as much bytes as it can. When blob has a lot, it shifts maxSize bytes */
        std::string str = stamp.ExtractStr(blob);

        is(str, "12345678", "variated size stamp shifts as much data as it can");

        str = stamp.ExtractStr(blob);
        is(str, "90", "but will be satisfied with less, unless it is not less than minSize()");
    }

    {  /* 7,8 */
       char sample[]="123456789";
       Blob blob(sample, strlen(sample));
       StampSeveralChars stamp; /* accepts from 2 to 8 bytes*/

       /* If used alone, is shifts as much bytes as it can. When blob has a lot, it shifts maxSize bytes */
        std::string str = stamp.ExtractStr(blob);

        is(str, "12345678", "variated size stamp shifts as much data as it can (take two)");

        try {
          std::string str = stamp.ExtractStr(blob);
          ok(false, "Variated stamp, not enough data");
        }
        catch (OutOfData)
        {
          ok(true, "Variated stamp, not enough data");
        }
        catch (...) //Any other exeption
        {
          ok(false, "Variated stamp, not enough data");
        }
    }

    {  /* 9 */
        char sample[]="1";
        Blob blob(sample, strlen(sample));
        StampTwoChars stamp;
        try {
          std::string str = stamp.ExtractStr(blob);
          ok(false, "Fixed stamp, not enough data");
        }
        catch (OutOfData)
        {
          ok(true, "Fixed stamp, not enough data");
        }
        catch (...) //Any other exeption
        {
          ok(false, "Fixed stamp, not enough data");
        }
    }

    {  /* 10 */
        char sample[]="1";
        Blob blob(sample, strlen(sample));
        StampTwoCharsList stamp;
        try {
          std::string str = stamp.ExtractStr(blob);
          ok(false, "Unbounded stamp, not enough data");
        }
        catch (OutOfData)
        {
          ok(true, "Unbounded stamp, not enough data");
        }
        catch (...) //Any other exeption
        {
          ok(false, "Unbounded stamp, not enough data");
        }
    }
    TEST_END;
}
