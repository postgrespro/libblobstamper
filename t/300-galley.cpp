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
#include <vector>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"

#include "test-chars-stamps.h"

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";

int
main()
    {
    TEST_START(68);
    /* Test Galley Vector with fixed size stamp*/
    { /* 1..4 */
        std::string expected1 = "12";
        std::string expected2 = "34";
        std::string expected3 = "56";

        StampTwoChars stamp;
        GalleyVectorStr galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::vector<std::string> res = galley.ExtractStrVector(blob);

        is(res[0], expected1, "GalleyVector, fixed size string stamp: First element of shifted list is ok");
        is(res[1], expected2, "GalleyVector, fixed size string  stamp: Second element of shifted list is ok");
        is(res[2], expected3, "GalleyVector, fixed size string  stamp: Third element of shifted list is ok");
        is(res.size(), 3, "GalleyVector, fixed size string stamp: res has 3 items");
    }
    /* Test Galley Sereies with unlimited size stamp*/
    {  /* 5 .. 9*/
        /* This is not the best test, as we do not predict behavior by setting forged sample values,
          but at least here we check that it work the same way it worked before. May be this test should be improved later*/

        std::string expected1 = "(9a, bc, de, &f, gh, ij)";
        std::string expected2 = "(lm, no, pq, rs, tu, vw, xy)";
        std::string expected3 = "(zA, B%, CD, EF, GH, IJ, KL)";
        std::string expected4 = "(MN, OP, QR, ST, UV, WX, YZ)";

        Blob blob(longer_sample, strlen(longer_sample));
        StampTwoCharsList stamp_charlist;
        GalleyVectorStr galley(stamp_charlist);

        std::vector<std::string> res = galley.ExtractStrVector(blob);
        std::string str;

        is(res[0], expected1, "GalleyVector, unlimited size string stamp: First element of shifted list is ok");
        is(res[1], expected2, "GalleyVector, unlimited size string stamp: Second element of shifted list is ok");
        is(res[2], expected3, "GalleyVector, unlimited size string stamp: Third element of shifted list is ok");
        is(res[3], expected4, "GalleyVector, unlimited size string stamp: Fourth element of shifted list is ok");
        is(res.size(), 4, "GalleyVector, unlimited size string stamp: The rest of the list is empty");
    }

    { /* 10..13 */

        unsigned short int expected1 = (unsigned char) '2' * 256 +(unsigned char) '1';
        unsigned short int expected2 = (unsigned char) '4' * 256 +(unsigned char) '3';
        unsigned short int expected3 = (unsigned char) '6' * 256 +(unsigned char) '5';

        StampArithm<unsigned short int> stamp;
        GalleyVectorBin galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::vector<std::vector<char>> res = galley.ExtractBinVector(blob);

        std::vector<char> v;
        unsigned short int * data;

        v = res[0];
        data = (unsigned short int *) &v[0];
        is(*data, expected1, "GalleyVector, fixed size binary stamp: First element of shifted list is ok");

        v = res[1];
        data = (unsigned short int *) &v[0];
        is(*data, expected2, "GalleyVector, fixed size binary stamp: Second element of shifted list is ok");

        v = res[2];
        data = (unsigned short int *) &v[0];
        is(*data, expected3, "GalleyVector, fixed size binary stamp: Third element of shifted list is ok");

        is(res.size(),3, "GalleyVector, fixed size binary stamp: result has 3 elements");
    }

    { /* 14 */

        signed int sample[] = {1, -2, -30, 40, -55, 6};
        StampArithm<signed int> stamp;
        GalleyVectorV<signed int> galley(stamp);
        Blob blob((char*)sample, sizeof(sample));
        std::vector<signed int> res = galley.ExtractValuesVector(blob);
        ok(!memcmp((void*) &sample, (void *) &res[0], sizeof(sample)), "GalleyVectorV returns ok");
    }

    /* Test Galley Vector with variated size stamp*/
    {  /* 15 .. 19*/
        /* This is not the best test, as we do not predict behavior by setting forged sample values,
          but at least here we check that it work the same way it worked before. May be this test should be improved later*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "234";
        std::string expected2 = "7*8";
        std::string expected3 = "bcde";
        std::string expected4 = "ghij";

        Blob blob(sample, strlen(sample));
        StampSeveralChars stamp;
        GalleyVectorStr galley(stamp);

        std::vector<std::string> res = galley.ExtractStrVector(blob);
        std::string str;

        is(res[0], expected1, "GalleyVector, unlimited size string stamp: First element of shifted list is ok");
        is(res[1], expected2, "GalleyVector, unlimited size string stamp: Second element of shifted list is ok");
        is(res[2], expected3, "GalleyVector, unlimited size string stamp: Third element of shifted list is ok");
        is(res[3], expected4, "GalleyVector, unlimited size string stamp: Fourth element of shifted list is ok");

        is(res.size(), 4, "GalleyVector, unlimited size string stamp: The list has only 4 members");

    }

    /* Test GalleySet with fixed size stamps*/
    {  /* 20 .. 23*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "z1";
        std::string expected2 = "23";


        Blob blob(sample, strlen(sample));

        StampTwoChars stamp;
        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(stamp);
        stamps.push_back(stamp);

        GalleySetStr galley(stamps);

        std::vector<std::string> res = galley.ExtractStrSet(blob);
        std::string str;
        str = res[0];
        is(str, expected1, "GalleySet, fixed size string stamps: First element of vector is ok");

        str = res[1];
        is(str, expected2, "GalleySet, fixed size string stamps: Second element of vector is ok");

        is(res.size(), 2, "GalleySet, fixed size string stamps: The vector has only two elements ");

        is(blob.Size(), strlen(sample) - res[0].length() - res[1].length(), "GalleySet: shifts no extra bytes for fixed stamps");

        is(galley.minSize(), stamp.minSize()*2, "GalleySet, fixed size string stamps: galley min size is ok");
        is(galley.maxSize(), stamp.maxSize()*2, "GalleySet, fixed size string stamps: galley max size is ok");
    }

    /* Test Galley Set with variated size stamps*/
    {  /* 26 .. 31*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "456";
        std::string expected2 = "7*8";

        Blob blob(sample, strlen(sample));

        StampSeveralChars stamp;
        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(stamp);
        stamps.push_back(stamp);

        GalleySetStr galley(stamps);

        std::vector<std::string> res = galley.ExtractStrSet(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleySet, variated size string stamp: First element of vector is ok");

        str = res[1];
        is(str, expected2, "GalleySet, variated size string stamp: Second element of vector is ok");

        is(res.size(), 2, "GalleySet, variated size string stamp: The vector has only two elements ");

        is(blob.Size(), strlen(sample) - res[0].length() - res[1].length() - ORACLE_SIZE*2, "GalleySet: shifts one oracle for each variated stamps");

        is(galley.minSize(), stamp.minSize()*2 + ORACLE_SIZE * 2, "GalleySet, variated size string stamps: galley min size is ok");
        is(galley.maxSize(), stamp.maxSize()*2 + ORACLE_SIZE * 2, "GalleySet, variated size string stamps: galley max size is ok");
    }

    /* Test Galley Set with unbounded size stamps*/
    {  /* 32 .. 37*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "(45, 67, *8, 9a, bc)";
        std::string expected2 = "(de, &f, gh, ij, kl)";

        Blob blob(sample, strlen(sample));

        StampTwoCharsList stamp;
        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(stamp);
        stamps.push_back(stamp);

        GalleySetStr galley(stamps);

        std::vector<std::string> res = galley.ExtractStrSet(blob);
        std::string str;

        is(res[0], expected1, "GalleySet, unbounded size string stamp: First element of vector is ok");

        is(res[1], expected2, "GalleySet, unbounded size string stamp: Second element of vector is ok");

        is(res.size(), 2, "GalleySet, unbounded size string stamp: The vector has only two elements ");

        is(blob.Size(), 0 , "GalleySet: will use all data for unbounded stamp");

        is(galley.minSize(), stamp.minSize()*2 + ORACLE_SIZE * 2, "GalleySet, unbounded size string stamps: galley min size is ok");
        is(galley.maxSize(), -1, "GalleySet, unbounded size string stamps: galley max size is ok");

    }

    /* Test Galley Set with mixed stam types */
    {  /* 38 .. 47*/

        char sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";

        std::string expected1 = "(9a, bc, de, &f, gh, ij, kl, mn, op, qr, st)";  // first u_stamp
        std::string expected2 = "uvw"; // first v_stamp
        std::string expected3 = "xyzA"; // second v_stamp
        std::string expected4 = "B%";   // first f_stamp
        std::string expected5 = "(CD, EF, GH, IJ, KL, MN, OP, QR, ST, UV, WX)"; //second u_stamp
        std::string expected6 = "Z!"; //second f_stamp

        Blob blob(sample, strlen(sample));


        StampTwoChars f_stamp;
        StampSeveralChars v_stamp;
        StampTwoCharsList u_stamp;

        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(u_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleySetStr galley(stamps);

        std::vector<std::string> res = galley.ExtractStrSet(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleySet, mixed type stamps: First unbounded stamp is ok");

        str = res[1];
        is(str, expected2, "GalleySet, mixed type stamps: Fist varieded stamp is ok");

        str = res[2];
        is(str, expected3, "GalleySet, mixed type stamps: Second variated stamp is ok");

        str = res[3];
        is(str, expected4, "GalleySet, mixed type stamps: First fixed stamp is ok");

        str = res[4];
        is(str, expected5, "GalleySet, mixed type stamps: Second unbounded stamp is ok");

        str = res[5];
        is(str, expected6, "GalleySet, mixed type stamps: Second fixed stamp is ok");

        is(res.size(), 6, "GalleySet, mixed type stamps: The vector has only six elements ");

        is(blob.Size(), 0 , "GalleySet: will use all data if we have at least one unbounded stamp");

        is(galley.minSize(), (f_stamp.minSize()+v_stamp.minSize()+u_stamp.minSize())*2 + ORACLE_SIZE * (2+2+1), "GalleySet, mixed types stamps: galley min size is ok");
        is(galley.maxSize(), -1, "GalleySet, mixed types stamps: galley max size is ok");
    }

    /* Test Galley Set, single unbounded stamp will use all data with no oracle to predict it*/
    {  /* 48 .. 54*/

        char sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";

        std::string expected1 = "z1";  // first f_stamp
        std::string expected2 = "(23, 45, 67, *8, 9a, bc, de, &f, gh, ij, kl, mn, op, qr, st, uv, wx, yz, AB, %C, DE, FG, HI, JK, LM, NO, PQ, RS, TU, VW, XY)"; // u_stamp
        std::string expected3 = "Z!"; // second f_stamp

        Blob blob(sample, strlen(sample));


        StampTwoChars f_stamp;
        StampTwoCharsList u_stamp;

        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleySetStr galley(stamps);

        std::vector<std::string> res = galley.ExtractStrSet(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleySet, mixed type stamps 2: First unbounded stamp is ok");

        str = res[1];
        is(str, expected2, "GalleySet, mixed type stamps 2: Fist varieded stamp is ok");

        str = res[2];
        is(str, expected3, "GalleySet, mixed type stamps 2: Second variated stamp is ok");

        is(res.size(), 3, "GalleySet, mixed type stamps 2: The vector has only three elements ");

        is(blob.Size(), 0 , "GalleySet 2: will use all data if we have at least one unbounded stamp");

        is(galley.minSize(), f_stamp.minSize()*2 + u_stamp.minSize() , "GalleySet 2, mixed types stamps: galley min size is ok");
        is(galley.maxSize(), -1, "GalleySet, mixed types stamps 2: galley max size is ok");
    }

    /* Test Galley Set, when there are only minimal amound of data available */
    {  /* 55 .. 64*/

        char sample[] = "oo" "oo" "oo" "oo" "oo" "z1" "23" "45" "67" "89" "ab";

        std::string expected1 = "(z1)";  // first u_stamp
        std::string expected2 = "23";    // first v_stamp
        std::string expected3 = "45";    // second v_stamp
        std::string expected4 = "67";    // first f_stamp
        std::string expected5 = "(89)";  //second u_stamp
        std::string expected6 = "ab";    //second f_stamp

        Blob blob(sample, strlen(sample));

        StampTwoChars f_stamp;
        StampSeveralChars v_stamp;
        StampTwoCharsList u_stamp;

        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(u_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleySetStr galley(stamps);

        std::vector<std::string> res = galley.ExtractStrSet(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleySet, mixed type stamps 3: First unbounded stamp is ok");

        str = res[1];
        is(str, expected2, "GalleySet, mixed type stamps 3: Fist varieded stamp is ok");

        str = res[2];
        is(str, expected3, "GalleySet, mixed type stamps 3: Second variated stamp is ok");

        str = res[3];
        is(str, expected4, "GalleySet, mixed type stamps 3: First fixed stamp is ok");

        str = res[4];
        is(str, expected5, "GalleySet, mixed type stamps 3: Second unbounded stamp is ok");

        str = res[5];
        is(str, expected6, "GalleySet, mixed type stamps 3: Second fixed stamp is ok");

        is(res.size(), 6, "GalleySet, mixed type stamps 3: The vector has only six elements ");

        is(blob.Size(), 0 , "GalleySet 3: will use all data if we have at least one unbounded stamp");

        is(galley.minSize(), (f_stamp.minSize()+v_stamp.minSize()+u_stamp.minSize())*2 + ORACLE_SIZE * (2+2+1), "GalleySet, mixed types stamps 3: galley min size is ok");
        is(galley.maxSize(), -1, "GalleySet, mixed types stamps 3: galley max size is ok");
    }

    /* Test Galley Set, when there are not enought data */
    {  /* 65 .. 68*/

        char sample[] = "oo" "oo" "oo" "oo" "oo" "z1" "23" "45" "67" "89" "a";

        Blob blob(sample, strlen(sample));

        StampTwoChars f_stamp;
        StampSeveralChars v_stamp;
        StampTwoCharsList u_stamp;

        std::vector<std::reference_wrapper<StampBaseStr>> stamps;
        stamps.push_back(u_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleySetStr galley(stamps);
        try{
          std::vector<std::string> res = galley.ExtractStrSet(blob);
          ok(false, "GalleySet, not enough data");
        }
        catch (OutOfData)
        {
          ok(true, "GalleySet, not enough data");
        }
        catch (...) //Any other exeption
        {
          ok(false, "GalleySet, not enough data");
        }

        is(blob.Size(), strlen(sample) , "GalleySet 4: will use keep all data when applying galley fails");

        is(galley.minSize(), (f_stamp.minSize()+v_stamp.minSize()+u_stamp.minSize())*2 + ORACLE_SIZE * (2+2+1), "GalleySet, mixed types stamps 4: galley min size is ok");
        is(galley.maxSize(), -1, "GalleySet, mixed types stamps 4: galley max size is ok");
    }
    TEST_END;
}
