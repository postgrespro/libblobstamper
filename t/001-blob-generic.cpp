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

#include "blobstamper/blobstamper.h"
#include "blobstamper/helpers.h"


using namespace TAP;

char my_data[]="1234567890ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyzАБВГДитд.___";

char short_sample[]="123456";

int
main()
{
    char *ptr;
    size_t size;

    TEST_START(7);

    { /* 1..2 */
        /* Check that DataDup gives us the same data we've just added to blob */
        Blob blob(my_data, strlen(my_data));
        blob.DataDup(ptr,size);
        ok(! memcmp(my_data, ptr, size), "Blob gets and returns blob data ok");
        ok(size == strlen(my_data), "Blob gets and returns blob data with expected size");
        free(ptr);
    }
    { /* 3..6 */
        /* Check that Shift bytes behave as expected*/
        char expected1[]="456";
        char expected2[]="123";

	std::shared_ptr<Blob> blob1 = std::make_shared<Blob>(short_sample,strlen(short_sample));
	std::shared_ptr<Blob> blob2 = blob1->Chop(3);

        blob1->DataDup(ptr,size);
        ok(size == strlen(expected1), "Blob shifted data size ok");
        ok(! memcmp(expected1, ptr, size), "Blob shifted data ok");
        free(ptr);

        blob2->DataDup(ptr,size);
        ok(size == strlen(expected2), "Blob remained data size ok");
        ok(! memcmp(expected2, ptr, size), "Blob remained data ok");
        free(ptr);
    }
    { /* 7 */
        /* Check that shifting too many bytes return empty result */
	std::shared_ptr<Blob> blob = std::make_shared<Blob>(my_data, strlen(my_data));
        try
        {
	  std::shared_ptr<Blob> blob_res = blob->Chop(99999);
          ok(false, "Shift too many bytes");
        }
        catch (OutOfData)
        {
          ok(true, "Shift too many bytes");
        }
        catch (...) //Any othe exeption
        {
          ok(false, "Shift too many bytes");
        }
    }



    TEST_END;
}
