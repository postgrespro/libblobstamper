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

#include <string>
#include <vector>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/stamp_json.h"

// tmp
#include <fstream>

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";
unsigned char bin_sample[]= {49, 22, 152, 226, 89, 119, 247, 115, 43, 42, 243, 71, 234, 231, 91, 35, 78, 108, 115, 39, 181, 248, 211, 52, 47, 94, 60, 237, 18, 39, 148, 62, 205, 214, 156, 184, 18, 201, 84, 183, 74, 134, 94, 72, 14, 116, 145, 109, 1, 230, 17, 95, 154, 100, 60, 15, 12, 102, 20, 115, 35, 183, 47, 176, 78, 176, 189, 113, 131, 93, 206, 62, 158, 166, 131, 95, 232, 217, 32, 171, 87, 31, 172, 160, 66, 160, 222, 134, 253, 1, 7, 191, 91, 125, 81, 148, 41, 46, 38, 171, 83, 215, 79, 34, 23, 215, 183, 118, 236, 191, 59, 160, 135, 58, 32, 199, 170, 183, 213, 53, 162, 138, 178, 118, 23, 202, 133, 8, 192, 183, 195, 199, 250, 29, 230, 34, 159, 10, 145, 74, 121, 85, 168, 204, 192, 25, 232, 88, 85, 76, 61, 168, 247, 128, 141, 176, 112, 113, 100, 201, 82, 183, 219, 236, 226, 171, 85, 97, 160, 1, 50, 250, 161, 51, 61, 220, 167, 227, 195, 17, 164, 211, 189, 130, 52, 167, 169, 42, 17, 29, 95, 15, 178, 165, 110, 87, 149, 214, 55, 12, 236, 138, 2, 245, 158, 84, 140, 24, 225, 169, 115, 16, 130, 253, 237, 182, 95, 109, 4, 28, 249, 4, 254, 166, 62, 107, 228, 113, 130, 127, 70, 79, 140, 41, 84, 218, 134, 146, 88, 65, 24, 174, 252, 253, 226, 214, 22, 92, 248, 14, 29, 60, 180, 94, 30, 186, 0};


int
main()
{
    std::fstream f{"/dev/random"};
//    std::fstream f{"buf"};
    if (!f) 
      std::cerr << "Unable to open file";

    std::vector<char> buffer (128,0);
    f.read(&buffer[0], buffer.size());

//    auto blob = std::make_shared<Blob>((char *)bin_sample, strlen((char *)bin_sample));
    auto blob = std::make_shared<Blob>(&buffer[0], buffer.size());


    auto stamp_j = std::make_shared<StampJSON>();

    std::string s = stamp_j->ExtractStr(blob);

    printf("%s\n", s.c_str());

/*

    TEST_START(6);
    {


    }
    TEST_END;*/
}
