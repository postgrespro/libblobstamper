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

#include "blobstamper/blob.h"
#include "blobstamper/stamp_arithm.h"
#include "blobstamper/stamp_lottery.h"


using namespace TAP;

unsigned char sample[]={1,63,64,255,1};
unsigned char sample2[]={1,63,64,255,1,63,64,255,1};

int
main()
{
    TEST_START(10);

    /* Tests for common Stamp Lottery*/
    { /* 1..5 */
	std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) sample, sizeof(sample));
        StampArithm<unsigned short> stamp_s;
        StampArithm<unsigned char> stamp_c;
        StampArithm<unsigned int> stamp_i;
        StampLottery<StampBaseStr> stamp({stamp_s});

        is(stamp.maxSize(), 2+1, "Test Set 1, max size is as expected");
        is(stamp.minSize(), 2+1, "Test Set 1, min size is as expected");

        stamp.Append(stamp_c);
        stamp.Append(stamp_i);

        is(stamp.maxSize(), 4+1, "Test Set 1.1, max size is as expected");
        is(stamp.minSize(), 1+1, "Test Set 1.1, min size is as expected");

        std::string s = stamp.ExtractStr(blob);

        is(s, "16447", "Test Set 1, return value is as expected");

    }

    { /* 6 */
        sample[0] = 255; /* Should choose last stamp*/
	std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) sample, sizeof(sample));
        StampArithm<unsigned short> stamp_s;
        StampArithm<unsigned char> stamp_c;
        StampArithm<unsigned int> stamp_i;
        StampLottery<StampBaseStr> stamp({stamp_s,stamp_c,stamp_i});

        std::string s = stamp.ExtractStr(blob);

        is(s, "33505343", "Test Set 2, return value is as expected");
    }

    { /* 7 */
        sample[0] = 128; /* Should choose stamp in the middle */
	std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) sample, sizeof(sample));
        StampArithm<unsigned short> stamp_s;
        StampArithm<unsigned char> stamp_c;
        StampArithm<unsigned int> stamp_i;
        StampLottery<StampBaseStr> stamp({stamp_s,stamp_c,stamp_i});

        std::string s = stamp.ExtractStr(blob);

        is(s, "63", "Test Set 3, return value is as expected");
    }

    { /* 8 */
        sample[0] = 1; /* Should choose first available stamp*/
	std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) sample, 2);  /* Sic! */
        StampArithm<unsigned short> stamp_s;
        StampArithm<unsigned char> stamp_c;
        StampArithm<unsigned long> stamp_l;
        StampLottery<StampBaseStr> stamp({stamp_s,stamp_l,stamp_c});

        std::string s = stamp.ExtractStr(blob);
        /* Only char stamp will be chosen as only it has enough data*/
        is(s, "63", "Test Set 4, return value is as expected");
    }

    /* Tests for common Stamp Lottery that is used for recursion*/


    { /* 9 */
        sample2[0] = 1; /* Should choose first available stamp*/
	std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) sample2, sizeof(sample2));
        StampArithm<unsigned short> stamp_s;
        StampArithm<unsigned char> stamp_c;
        StampArithm<unsigned long> stamp_l;
        StampLottery4Recursion<StampBaseStr> stamp({stamp_s,stamp_c,stamp_l});

        std::string s = stamp.ExtractStr(blob);
        /* Only unsigned long stamp will be chosen as other stamps does not consume enough data*/
        is(s, "143904352459767871", "Test Set 5, return value is as expected");
    }

    { /* 10 */
        sample2[0] = 255; /* Should choose last available stamp*/
	std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) sample2, sizeof(sample2) -1 ); /*One byte short for unsigned long*/
        StampArithm<unsigned short> stamp_s;
        StampArithm<unsigned char> stamp_c;
        StampArithm<unsigned long> stamp_l;
        StampLottery4Recursion<StampBaseStr> stamp({stamp_s,stamp_c,stamp_l});

        std::string s = stamp.ExtractStr(blob);
        /* l-stamp is to long. s & c stamps are too short. When no stamps left, it will ignore "too short" limit, and will chose last available stamp: c */
        is(s, "63", "Test Set 6, return value is as expected");
    }



    TEST_END;
}
