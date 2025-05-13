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
#include <cstring>


#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/stamp_json.h"
#include "blobstamper/oracle.h"

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";
unsigned char bin_sample[]= {49, 22, 152, 226, 89, 119, 247, 115, 43, 42, 243, 71, 234, 231, 91, 35, 78, 108, 115, 39, 181, 248, 211, 52, 47, 94, 60, 237, 18, 39, 148, 62, 205, 214, 156, 184, 18, 201, 84, 183, 74, 134, 94, 72, 14, 116, 145, 109, 1, 230, 17, 95, 154, 100, 60, 15, 12, 102, 20, 115, 35, 183, 47, 176, 78, 176, 189, 113, 131, 93, 206, 62, 158, 166, 131, 95, 232, 217, 32, 171, 87, 31, 172, 160, 66, 160, 222, 134, 253, 1, 7, 191, 91, 125, 81, 148, 41, 46, 38, 171, 83, 215, 79, 34, 23, 215, 183, 118, 236, 191, 59, 160, 135, 58, 32, 199, 170, 183, 213, 53, 162, 138, 178, 118, 23, 202, 133, 8, 192, 183, 195, 199, 250, 29, 230, 34, 159, 10, 145, 74, 121, 85, 168, 204, 192, 25, 232, 88, 85, 76, 61, 168, 247, 128, 141, 176, 112, 113, 100, 201, 82, 183, 219, 236, 226, 171, 85, 97, 160, 1, 50, 250, 161, 51, 61, 220, 167, 227, 195, 17, 164, 211, 189, 130, 52, 167, 169, 42, 17, 29, 95, 15, 178, 165, 110, 87, 149, 214, 55, 12, 236, 138, 2, 245, 158, 84, 140, 24, 225, 169, 115, 16, 130, 253, 237, 182, 95, 109, 4, 28, 249, 4, 254, 166, 62, 107, 228, 113, 130, 127, 70, 79, 140, 41, 84, 218, 134, 146, 88, 65, 24, 174, 252, 253, 226, 214, 22, 92, 248, 14, 29, 60, 180, 94, 30, 186, 0};


int
main()
{

// If you uncomment this it will crash for reason I do not understand

/*    auto blob1 = std::make_shared<Blob>((char *)bin_sample, strlen((char *)bin_sample));
    auto stamp_j1 = std::make_shared<StampJSON>();
    std::string s1 = stamp_j1->ExtractStr(blob1);
    printf("%s\n", s1.c_str());
*/

    TEST_START(5);
    {
        auto blob = std::make_shared<Blob>((char *)bin_sample, strlen((char *)bin_sample));
        auto stamp_j = std::make_shared<StampJSON>();

        std::string s = stamp_j->ExtractStr(blob);
        std::string expected = "[6850509402014839822, 3438255817691106319, \"resulted\", -2.684757453484730872673776686700739040682020879678934003695336094670709694419006239953740106372555463816175000634871124431422513570790242370232656601974908085084433064181237341375458199532539671263821862377193715039215799333468172751725903574125176464661297323397544866180407166347114750393787405005704461598334438005517341732432719884411807209593715412099886215291366653445948031730949878692626953125e-151, \"lushes\", -5199661808640109653, \"denomination\", -3.06532686549173138956803086330182850360870361328125, \"sedating\", \"robots\", -3.3696970613711244822470283884492113423278459359891097388748050077510095184860700081976239055565263066452656899596451722800338757224380970001220703125e-42, \"preface\", 9.55422367779008699072920786882312943145717681740324270561686674939552239415695463827122863940711666092367434557202401330797838517640477979352142678948212051182053983211517333984375e-56, 6300155112918555921, \"dams\", 2.411323842459936645124797826433320804619144161893916130561265623523643502350374626190490408739886564139366504029821786107467040377895714047017891228899122281601579488797245463600235576598531485903332393648607370715931713232242018751523644727805408352870771308312443806176048554264200149458533769648133943422986030732712523236966445578809656378067940216124429747414289524093606877704785435824362397944294729373146388523502884316004124548536154374326842524560415029933595880452289294570895862703927051929788804023836835074268010108198185051300394774230223405817676130206688777308144339841912135245325959771275736867526109676372404347501558868228887456576678950716422841427035057837446885287135955877602100372314453125e-287, 9188031415654983422, -5901895462997796268, \"blazer\"]";
        is(s, expected, "json stamp works well");
    }
    /* Smallest sample giving sane output */
    {
        auto blob = std::make_shared<Blob>((char *)bin_sample, 10); /* ORACLE_SIZE = 2 + int = 8 */
        auto stamp_j = std::make_shared<StampJSON>();

        std::string s = stamp_j->ExtractStr(blob);
        std::string expected = "3038649880288027288";
        is(s, expected, "json stamp on shortest data works well");
    }
    /* Check throw OutOfMemory when data length is less then ORACLE_SIZE */
    {
        auto blob = std::make_shared<Blob>((char *)bin_sample, ORACLE_SIZE - 1);
        auto stamp_j = std::make_shared<StampJSON>();

        int cought = 0;
        try{
          std::string s = stamp_j->ExtractStr(blob);
        }
        catch (OutOfData)
        {
            cought = 1;
        }
        ok(cought, "Throws OutOfData if have data size less then ORACLE_SIZE");
    }
    /* Check throw OutOfMemory when data length is equal to ORACLE_SIZE */
    {
        auto blob = std::make_shared<Blob>((char *)bin_sample, ORACLE_SIZE);
        auto stamp_j = std::make_shared<StampJSON>();

        int cought = 0;
        try{
          std::string s = stamp_j->ExtractStr(blob);
        }
        catch (OutOfData)
        {
            cought = 1;
        }
        ok(cought, "Throws OutOfData if have data size equal to ORACLE_SIZE");
    }
    /* Check throw OutOfMemory when data length is bit bigger than ORACLE_SIZE */
    /* But not big enough to give any output */
    {
        auto blob = std::make_shared<Blob>((char *)bin_sample, ORACLE_SIZE + 1);
        auto stamp_j = std::make_shared<StampJSON>();

        int cought = 0;
        try{
          std::string s = stamp_j->ExtractStr(blob);
        }
        catch (OutOfData)
        {
            cought = 1;
        }
        ok(cought, "Throws OutOfData if have data size is a bit bigger then ORACLE_SIZE");
    }
    TEST_END;
}
