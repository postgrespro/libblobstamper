#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
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
    TEST_START(63);
    /* Test Galley Sereies with fixed size stampp*/
    { /* 1..4 */
        std::string expected1 = "12";
        std::string expected2 = "34";
        std::string expected3 = "56";

        StampTwoChars stamp;
        GalleySeries galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::list<std::string> res = galley.ExtractStr(blob);

        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, fixed size string stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, fixed size string  stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, fixed size string  stamp: Third element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, fixed size string stamp: The rest of the list is empty");
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
        GalleySeries galley(stamp_charlist);

        std::list<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, unlimited size string stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, unlimited size string stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, unlimited size string stamp: Third element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected4, "GalleySeries, unlimited size string stamp: Fourth element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, unlimited size string stamp: The rest of the list is empty");
    }

    { /* 10..13 */

        unsigned short int expected1 = (unsigned char) '2' * 256 +(unsigned char) '1';
        unsigned short int expected2 = (unsigned char) '4' * 256 +(unsigned char) '3';
        unsigned short int expected3 = (unsigned char) '6' * 256 +(unsigned char) '5';

        StampBinInt16 stamp;
        GalleySeries galley(stamp);
        Blob blob(short_sample, strlen(short_sample));
        std::list<void * > res = galley.ExtractBin(blob);

        unsigned short int * data;

        data = (unsigned short int *) res.front();
        is(*data, expected1, "GalleySeries, fixed size binary stamp: First element of shifted list is ok");
        res.pop_front();
        free(data);

        data = (unsigned short int *) res.front();
        is(*data, expected2, "GalleySeries, fixed size binary stamp: Second element of shifted list is ok");
        res.pop_front();
        free(data);

        data = (unsigned short int *) res.front();
        is(*data, expected3, "GalleySeries, fixed size binary stamp: Third element of shifted list is ok");
        res.pop_front();
        free(data);

        ok(res.empty(), "GalleySeries, fixed size binary stamp: The rest of the list is empty");
    }

    /* Test Galley Sereies with variated size stamp*/
    {  /* 14 .. 18*/
        /* This is not the best test, as we do not predict behavior by setting forged sample values,
          but at least here we check that it work the same way it worked before. May be this test should be improved later*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "234";
        std::string expected2 = "7*8";
        std::string expected3 = "bcde";
        std::string expected4 = "ghij";

        Blob blob(sample, strlen(sample));
        StampSeveralChars stamp;
        GalleySeries galley(stamp);

        std::list<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res.front();
        is(str, expected1, "GalleySeries, unlimited size string stamp: First element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected2, "GalleySeries, unlimited size string stamp: Second element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected3, "GalleySeries, unlimited size string stamp: Third element of shifted list is ok");
        res.pop_front();

        str = res.front();
        is(str, expected4, "GalleySeries, unlimited size string stamp: Fourth element of shifted list is ok");
        res.pop_front();

        ok(res.empty(), "GalleySeries, unlimited size string stamp: The rest of the list is empty");

    }

    /* Test Galley Vector with fixed size stamps*/
    {  /* 19 .. 22*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "z1";
        std::string expected2 = "23";


        Blob blob(sample, strlen(sample));

        StampTwoChars stamp;
        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(stamp);
        stamps.push_back(stamp);

        GalleyVector galley(stamps);

        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;
        str = res[0];
        is(str, expected1, "GalleyVector, fixed size string stamps: First element of vector is ok");

        str = res[1];
        is(str, expected2, "GalleyVector, fixed size string stamps: Second element of vector is ok");

        is(res.size(), 2, "GalleyVector, fixed size string stamps: The vector has only two elements ");

        is(blob.Size(), strlen(sample) - res[0].length() - res[1].length(), "GalleyVector: shifts no extra bytes for fixed stamps");

        is(galley.minSize(), stamp.minSize()*2, "GalleyVector, fixed size string stamps: galley min size is ok");
        is(galley.maxSize(), stamp.maxSize()*2, "GalleyVector, fixed size string stamps: galley max size is ok");
    }

    /* Test Galley Vector with variated size stamps*/
    {  /* 25 .. 30*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "456";
        std::string expected2 = "7*8";

        Blob blob(sample, strlen(sample));

        StampSeveralChars stamp;
        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(stamp);
        stamps.push_back(stamp);

        GalleyVector galley(stamps);

        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleyVector, variated size string stamp: First element of vector is ok");

        str = res[1];
        is(str, expected2, "GalleyVector, variated size string stamp: Second element of vector is ok");

        is(res.size(), 2, "GalleyVector, variated size string stamp: The vector has only two elements ");

        is(blob.Size(), strlen(sample) - res[0].length() - res[1].length() - ORACLE_SIZE*2, "GalleyVector: shifts one oracle for each variated stamps");

        is(galley.minSize(), stamp.minSize()*2 + ORACLE_SIZE * 2, "GalleyVector, variated size string stamps: galley min size is ok");
        is(galley.maxSize(), stamp.maxSize()*2 + ORACLE_SIZE * 2, "GalleyVector, variated size string stamps: galley max size is ok");
    }

    /* Test Galley Vector with unbounded size stamps*/
    {  /* 31 .. 36*/

        char sample[]="z1234567*89abcde&fghijklm";

        std::string expected1 = "(45, 67, *8, 9a, bc)";
        std::string expected2 = "(de, &f, gh, ij, kl)";

        Blob blob(sample, strlen(sample));

        StampTwoCharsList stamp;
        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(stamp);
        stamps.push_back(stamp);

        GalleyVector galley(stamps);

        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleyVector, unbounded size string stamp: First element of vector is ok");

        str = res[1];
        is(str, expected2, "GalleyVector, unbounded size string stamp: Second element of vector is ok");

        is(res.size(), 2, "GalleyVector, unbounded size string stamp: The vector has only two elements ");

        is(blob.Size(), 0 , "GalleyVector: will use all data for unbounded stamp");

        is(galley.minSize(), stamp.minSize()*2 + ORACLE_SIZE * 2, "GalleyVector, unbounded size string stamps: galley min size is ok");
        is(galley.maxSize(), -1, "GalleyVector, unbounded size string stamps: galley max size is ok");

    }

    /* Test Galley Vector with mixed stam types */
    {  /* 37 .. 46*/

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

        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(u_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleyVector galley(stamps);

        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleyVector, mixed type stamps: First unbounded stamp is ok");

        str = res[1];
        is(str, expected2, "GalleyVector, mixed type stamps: Fist varieded stamp is ok");

        str = res[2];
        is(str, expected3, "GalleyVector, mixed type stamps: Second variated stamp is ok");

        str = res[3];
        is(str, expected4, "GalleyVector, mixed type stamps: First fixed stamp is ok");

        str = res[4];
        is(str, expected5, "GalleyVector, mixed type stamps: Second unbounded stamp is ok");

        str = res[5];
        is(str, expected6, "GalleyVector, mixed type stamps: Second fixed stamp is ok");

        is(res.size(), 6, "GalleyVector, mixed type stamps: The vector has only six elements ");

        is(blob.Size(), 0 , "GalleyVector: will use all data if we have at least one unbounded stamp");

        is(galley.minSize(), (f_stamp.minSize()+v_stamp.minSize()+u_stamp.minSize())*2 + ORACLE_SIZE * (2+2+1), "GalleyVector, mixed types stamps: galley min size is ok");
        is(galley.maxSize(), -1, "GalleyVector, mixed types stamps: galley max size is ok");
    }

    /* Test Galley Vector, single unbounded stamp will use all data with no oracle to predict it*/
    {  /* 47 .. 53*/

        char sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";

        std::string expected1 = "z1";  // first f_stamp
        std::string expected2 = "(23, 45, 67, *8, 9a, bc, de, &f, gh, ij, kl, mn, op, qr, st, uv, wx, yz, AB, %C, DE, FG, HI, JK, LM, NO, PQ, RS, TU, VW, XY)"; // u_stamp
        std::string expected3 = "Z!"; // second f_stamp

        Blob blob(sample, strlen(sample));


        StampTwoChars f_stamp;
        StampTwoCharsList u_stamp;

        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleyVector galley(stamps);

        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleyVector, mixed type stamps 2: First unbounded stamp is ok");

        str = res[1];
        is(str, expected2, "GalleyVector, mixed type stamps 2: Fist varieded stamp is ok");

        str = res[2];
        is(str, expected3, "GalleyVector, mixed type stamps 2: Second variated stamp is ok");

        is(res.size(), 3, "GalleyVector, mixed type stamps 2: The vector has only three elements ");

        is(blob.Size(), 0 , "GalleyVector 2: will use all data if we have at least one unbounded stamp");

        is(galley.minSize(), f_stamp.minSize()*2 + u_stamp.minSize() , "GalleyVector 2, mixed types stamps: galley min size is ok");
        is(galley.maxSize(), -1, "GalleyVector, mixed types stamps 2: galley max size is ok");
    }

    /* Test Galley Vector, when there are only minimal amound of data available */
    {  /* 54 .. 63*/

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

        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(u_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleyVector galley(stamps);

        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;

        str = res[0];
        is(str, expected1, "GalleyVector, mixed type stamps 3: First unbounded stamp is ok");

        str = res[1];
        is(str, expected2, "GalleyVector, mixed type stamps 3: Fist varieded stamp is ok");

        str = res[2];
        is(str, expected3, "GalleyVector, mixed type stamps 3: Second variated stamp is ok");

        str = res[3];
        is(str, expected4, "GalleyVector, mixed type stamps 3: First fixed stamp is ok");

        str = res[4];
        is(str, expected5, "GalleyVector, mixed type stamps 3: Second unbounded stamp is ok");

        str = res[5];
        is(str, expected6, "GalleyVector, mixed type stamps 3: Second fixed stamp is ok");

        is(res.size(), 6, "GalleyVector, mixed type stamps 3: The vector has only six elements ");

        is(blob.Size(), 0 , "GalleyVector 3: will use all data if we have at least one unbounded stamp");

        is(galley.minSize(), (f_stamp.minSize()+v_stamp.minSize()+u_stamp.minSize())*2 + ORACLE_SIZE * (2+2+1), "GalleyVector, mixed types stamps 3: galley min size is ok");
        is(galley.maxSize(), -1, "GalleyVector, mixed types stamps 3: galley max size is ok");
    }

    /* Test Galley Vector, when there are not enought data */
    {  /* 64 .. 67*/

        char sample[] = "oo" "oo" "oo" "oo" "oo" "z1" "23" "45" "67" "89" "a";

        Blob blob(sample, strlen(sample));

        StampTwoChars f_stamp;
        StampSeveralChars v_stamp;
        StampTwoCharsList u_stamp;

        std::vector<std::reference_wrapper<StampBase>> stamps;
        stamps.push_back(u_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(v_stamp);
        stamps.push_back(f_stamp);
        stamps.push_back(u_stamp);
        stamps.push_back(f_stamp);

        GalleyVector galley(stamps);
        std::vector<std::string> res = galley.ExtractStr(blob);
        std::string str;

        is(res.size(), 0, "GalleyVector, mixed type stamps 4: Fails when not enught data");

        is(blob.Size(), strlen(sample) , "GalleyVector 4: will use keep all data when applying galley fails");

        is(galley.minSize(), (f_stamp.minSize()+v_stamp.minSize()+u_stamp.minSize())*2 + ORACLE_SIZE * (2+2+1), "GalleyVector, mixed types stamps 4: galley min size is ok");
        is(galley.maxSize(), -1, "GalleyVector, mixed types stamps 4: galley max size is ok");
    }
    TEST_END;
}
