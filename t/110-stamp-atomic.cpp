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
short int sample_data_int16[] = {1,  -2, -3, 4};
int       sample_data_int32[] = {10,-20,-30,40};
long long sample_data_int64[] = {100,-200,-300,400};
double    sample_data_double[] =  {1.4142, 2, 3.1415, 4.2e01, 5, 6, 7};

int
main()
{
    size_t sample_data_int16_size = sizeof(short int) * 4;
    size_t sample_data_int32_size = sizeof(int) * 4;
    size_t sample_data_int64_size = sizeof(long long) * 4;
    size_t sample_data_double_size = sizeof(double) * 7;

    TEST_START(13);

    /* Check that Bin and Str Char stamps works well */
    { /* 1, 2  */
        Blob blob(sample_data_char, strlen(sample_data_char));
        StampArithm<char> stamp;
        char * c = (char *) blob.ShiftSingleStampBin(stamp);
        is(*c, 'S' , "Bin Char stamp works well");
        free(c);

//        StampStrUInt8 stamp2;
        StampArithm<char> stamp2;
        std::string s = blob.ShiftSingleStampStr(stamp2);
        is(s, "111" , "Str UInt8 stamp works well");  // 'o'==111
    }

    /* Check that Bin and Srt Int16 stamps works well */
    { /* 3, 4, 5 */
        Blob blob((char *)sample_data_int16, sample_data_int16_size);
        StampArithm<short int> stamp;
        short int * i = (short int *) blob.ShiftSingleStampBin(stamp);
        is(*i,  1 , "Bin Int16 stamp works well");
        free(i);

        StampArithm<unsigned short int> stamp2;
        std::string s = blob.ShiftSingleStampStr(stamp2);
        is(s, "65534" , "Str UInt16 stamp works well");  // (unsigned short int)-2 == 65534

        StampArithm<signed short int> stamp3;
        s = blob.ShiftSingleStampStr(stamp3);
        is(s, "-3" , "Str SInt16 stamp works well");
    }

    /* Check that Bin and Srt Int32 stamps works well */
    { /* 6, 7, 8 */
        Blob blob((char *)sample_data_int32, sample_data_int32_size);
        StampArithm<int> stamp;
        int * i = (int *) blob.ShiftSingleStampBin(stamp);
        is(*i,  10 , "Bin Int32 stamp works well");
        free(i);

        StampArithm<unsigned int> stamp2;
        std::string s = blob.ShiftSingleStampStr(stamp2);
        is(s, "4294967276" , "Str UInt32 stamp works well");  // (unsigned short int)-20 == 4294967276

        StampArithm<signed int> stamp3;
        s = blob.ShiftSingleStampStr(stamp3);
        is(s, "-30" , "Str SInt32 stamp works well");
    }


    /* Check that Bin and Srt Int64 stamps works well */
    { /* 9, 10, 11 */
        Blob blob((char *)sample_data_int64, sample_data_int64_size);
        StampArithm<long long> stamp;
        long long * i = (long long *) blob.ShiftSingleStampBin(stamp);
        is(*i,  100 , "Bin Int64 stamp works well");
        free(i);

        StampArithm<unsigned long long> stamp2;
        std::string s = blob.ShiftSingleStampStr(stamp2);
        is(s, "18446744073709551416" , "Str UInt64 stamp works well");  // (unsigned short int)-200 == 18446744073709551416

        StampArithm<signed long long> stamp3;
        s = blob.ShiftSingleStampStr(stamp3);
        is(s, "-300" , "Str SInt64 stamp works well");
    }



    /* Check that Bin Double stamp works well */
    { /* 12 */
        Blob blob((char *)sample_data_double, sample_data_double_size);
        StampArithm<double> stamp;
        double *d = (double *) blob.ShiftSingleStampBin(stamp);
        is(*d, 1.4142, "Bin Double stamp works well");
        free(d);
    }

    /* Check that Str Double stamp works well */
    { /* 13 */
        Blob blob((char *)sample_data_double, sample_data_double_size);
        StampArithm<double> stamp;
        std::string res = blob.ShiftSingleStampStr(stamp);
        res = blob.ShiftSingleStampStr(stamp);
        is(res, "2", "Str Double stamp works well");
    }
    TEST_END;
}
