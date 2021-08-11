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


/* Tests for atomic type stamps */

char      sample_data_char[] = "Some string";
short int sample_data_int16[] =  {1,     -2,     -3,     4, 555, 66,  777, 8};
int       sample_data_int32[] =  {10,   -20,   - 30,    40, 500,  6,   77, 888};
long long sample_data_int64[] =  {100, -200,   -300,   400,   5,  6, 7000, 808};
double    sample_data_double[] = {1.4142, 2, 3.1415, 4.2e01,  5,  6, 7};

int
main()
{
    size_t sample_data_int16_size = sizeof(sample_data_int16);
    size_t sample_data_int32_size = sizeof(sample_data_int32);
    size_t sample_data_int64_size = sizeof(sample_data_int64);
    size_t sample_data_double_size = sizeof(double) * 7;

    TEST_START(29);

    /* Check that Bin and Str Char stamps works well */
    { /* 1..4 */
        Blob blob(sample_data_char, strlen(sample_data_char));
        StampArithm<char> stamp;

        std::vector<char> v = blob.ShiftSingleStampBin(stamp);
        char * pc = (char *) &v[0];
        is(*pc, 'S' , "Bin Char stamp works well");

        std::string s = blob.ShiftSingleStampStr(stamp);
        is(s, "111" , "Str UInt8 stamp works well");  // 'o'==111

        char c = stamp.ExtractValue(blob);
        is(c, 'm' , "extract char as value works well");

        sized_ptr<char> sp =stamp.ExtractPValue(blob);
        char * p = sp;
        is(*p, 'e', "Extract poiner to value works well");
    }

    /* Check that Bin and Srt Int16 stamps works well */
    { /* 5..11 */
        Blob blob((char *)sample_data_int16, sample_data_int16_size);
        StampArithm<short int> stamp;
        std::vector<char> v = blob.ShiftSingleStampBin(stamp);
        short int * pi = (short int *) &v[0];
        is(*pi,  1 , "Bin Int16 stamp works well");

        StampArithm<unsigned short int> stamp_unsigned;
        std::string s = blob.ShiftSingleStampStr(stamp_unsigned);
        is(s, "65534" , "Str UInt16 stamp works well");  // (unsigned short int)-2 == 65534

        StampArithm<signed short int> stamp_signed;
        s = blob.ShiftSingleStampStr(stamp_signed);
        is(s, "-3" , "Str SInt16 stamp works well");

        unsigned short int ui = stamp_unsigned.ExtractValue(blob);
        is(ui, 4, "Extract unsigned int16 as value");

        signed short int si = stamp_signed.ExtractValue(blob);
        is(si, 555, "Extract signed int16 as value");

        sized_ptr<unsigned short int> usp =stamp_unsigned.ExtractPValue(blob);
        unsigned short int * up = usp;
        is(*up, 66, "Extract poiner to value for unsigned int16 works well");

        sized_ptr<signed short int> ssp =stamp_signed.ExtractPValue(blob);
        signed short int * sp = ssp;
        is(*sp, 777, "Extract poiner to value for signed int16 works well");
    }

    /* Check that Bin and Srt Int32 stamps works well */
    { /*  12..18 */
        Blob blob((char *)sample_data_int32, sample_data_int32_size);
        StampArithm<int> stamp;

        std::vector<char> v = blob.ShiftSingleStampBin(stamp);

        int * i = (int *) &v[0];
        is(*i,  10 , "Bin Int32 stamp works well");

        StampArithm<unsigned int> stamp_unsigned;
        std::string s = blob.ShiftSingleStampStr(stamp_unsigned);
        is(s, "4294967276" , "Str UInt32 stamp works well");  // (unsigned short int)-20 == 4294967276

        StampArithm<signed int> stamp_signed;
        s = blob.ShiftSingleStampStr(stamp_signed);
        is(s, "-30" , "Str SInt32 stamp works well");

        unsigned int ui = stamp_unsigned.ExtractValue(blob);
        is(ui, 40, "Extract unsigned int32 as value");

        signed int si = stamp_signed.ExtractValue(blob);
        is(si, 500, "Extract signed int32 as value");

        sized_ptr<unsigned int> usp =stamp_unsigned.ExtractPValue(blob);
        unsigned int * up = usp;
        is(*up, 6, "Extract poiner to value for unsigned int32 works well");

        sized_ptr<signed int> ssp =stamp_signed.ExtractPValue(blob);
        signed int * sp = ssp;
        is(*sp, 77, "Extract poiner to value for signed int32 works well");
    }


    /* Check that Bin and Srt Int64 stamps works well */
    { /* 19..25 */
        Blob blob((char *)sample_data_int64, sample_data_int64_size);
        StampArithm<long long> stamp;

        std::vector<char> v = blob.ShiftSingleStampBin(stamp);

        long long * i = (long long *) &v[0];
        is(*i,  100 , "Bin Int64 stamp works well");

        StampArithm<unsigned long long> stamp_unsigned;
        std::string s = blob.ShiftSingleStampStr(stamp_unsigned);
        is(s, "18446744073709551416" , "Str UInt64 stamp works well");  // (unsigned short int)-200 == 18446744073709551416

        StampArithm<signed long long> stamp_signed;
        s = blob.ShiftSingleStampStr(stamp_signed);
        is(s, "-300" , "Str SInt64 stamp works well");

        unsigned long long ui = stamp_unsigned.ExtractValue(blob);
        is(ui, 400, "Extract unsigned int32 as value");

        signed long long si = stamp_signed.ExtractValue(blob);
        is(si, 5, "Extract signed int32 as value");

        sized_ptr<unsigned long long> usp =stamp_unsigned.ExtractPValue(blob);
        unsigned  long long * up = usp;
        is(*up, 6, "Extract poiner to value for unsigned int64 works well");

        sized_ptr<signed  long long> ssp =stamp_signed.ExtractPValue(blob);
        signed  long long * sp = ssp;
        is(*sp, 7000, "Extract poiner to value for signed int64 works well");

    }

    /* Test Double stamp */
    { /* 26..29 */
        Blob blob((char *)sample_data_double, sample_data_double_size);
        StampArithm<double> stamp;
        std::vector<char> v = blob.ShiftSingleStampBin(stamp);
        double *pd = (double *) &v[0];
        is(*pd, 1.4142, "Bin Double stamp works well");

        std::string res = blob.ShiftSingleStampStr(stamp);
        is(res, "2", "Str Double stamp works well");

        double d = stamp.ExtractValue(blob);
        is(d, 3.1415, "Extract double as value");

        sized_ptr<double> sp =stamp.ExtractPValue(blob);
        double * p = sp;
        is(*p, 42, "Extract poiner to value for double works well");

    }
    TEST_END;
}
