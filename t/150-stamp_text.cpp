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

#include "blobstamper/stamp_text.h"

using namespace TAP;

int
main()
{
  TEST_START(7);
  { /* 1..1 */
    char data[] = "папа\0мама\0бабушка\0дедушка\0братик\0сестричка";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampStringLatin1 stamp_str_l1;
    std::string s = stamp_str_l1.ExtractStr(blob);
    is(s, "папа мама бабушка дедушка братик сестричка", "StampStringLatin1");
  }

  { /* 2..2 */
    char data[] = "папа\0мама\0бабушка\0дедушка\0братик\0сестричка";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampStringASCII stamp_str_ascii;
    std::string s = stamp_str_ascii.ExtractStr(blob);
    is(s,
       "P?P0P?P0 P<P0P<P0 P1P0P1Q\x3Q\x8P:P0 P4P5P4Q\x3Q\x8P:P0 P1Q P0Q\x2P8P: Q\x1P5Q\x1Q\x2Q P8Q\x7P:P0",
        "StampTextASCII");
  }

  { /* 3..3 */
    unsigned char data[] = {
0x41, 0x00, 0x00, 0xB2, 0x03, 0x00, 0x14, 0x04, 0x00, 0x9C, 0x4E, 0x00, 0xC4, 0x30, 0x00, 0x36, 0xF4, 0x01, 0x23, 0x0E, 0x00, 0x0F, 0x00, 0x2C,
0x03, 0x26, 0x00, 0xA9, 0xF4, 0x01, 0x19, 0x4E, 0x00, 0x1E, 0xF6, 0x01, 0x95, 0x0D, 0x00, 0xA5, 0x07, 0x00, 0x7D, 0x0F, 0x00, 0xD0, 0x05, 0x00,
0x13, 0xF9, 0x01, 0xA9, 0x03, 0x00, 0x66, 0x5B, 0x00, 0x0A, 0x0E, 0x00, 0xE3, 0x53, 0x00, 0x30, 0x2B, 0x00, 0xB8, 0x30, 0x01, 0x7C, 0xF4, 0x01,
0xA2, 0xF3, 0x01, 0x35, 0x0C, 0x00, 0x0D, 0x4E, 0x00, 0xB0, 0xF9, 0x01, 0xE6, 0x0C, 0x00, 0x8E, 0xF4, 0x01, 0xA9, 0x06, 0x00, 0xC7, 0xF4, 0x01,
0x5C, 0x0B, 0x00, 0x4B, 0x4E, 0x00, 0xD2, 0x05, 0x00, 0x13, 0xF9, 0x01, 0x28, 0x0F, 0x00, 0xF4, 0xF3, 0x01, 0x95, 0x0B, 0x00, 0x80, 0x0D, 0x00,
0x3E, 0xF3, 0x01, 0x7B,
};
    std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *) data, (sizeof data)-1);
    StampStringUTF8 stamp_str_ascii;
    std::string s = stamp_str_ascii.ExtractStr(blob);
    is(s, "AβД亜ツ🐶ร󀀏☃💩丙😞ඕޥཽא🤓Ω学ช口⬰𓂸👼🎢వ不🦰೦💎ک📇ଡ଼之ג🤓༨🏴க඀🌾",
       "StampTextUTF8");
  }

  { /* 4..4 */
    char data[] = "dad\0mam\0granddad\0grandmam\0brother\0sister";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampText<StampStringLatin1> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "d dad gra n dmam broth er siste", "StampText<StampStringLatin1>");
  }

  { /* 5..5 */
    char data[] = "abcdef" "abcdef" "ABCDEF" "012345";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampText<StampDictLCAlphaSmall> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "gleam godfather graffiti greened grouping gunshots gleam godfather graffiti greened grouping gunshots dismally dissented divested doorstep dread drunks convertors corpulent counterparts cranking crippled crusades", "StampText<StampDictLCAlphaSmall>");
  }

  { /* 6..6 */
    char data[] = "Некоторый текст написанный русскими буквами в кодировке utf-8";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampText<StampStringASCII> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "P9  Q  Q\x3Q \x1Q \x1P: P8 P<P 8 P 1Q\x3 P:P 2P0 P<P 8 P 2  P: P>P 4P8 Q  P>P 2P :P5  ut f-8", "StampText<StampStringASCII>");
  }

  { /* 7..7 */
    unsigned char data[] = {
0x41, 0x00, 0x00, 0xB2, 0x03, 0x00, 0x14, 0x04, 0x00, 0x9C, 0x4E, 0x00, 0xC4, 0x30, 0x00, 0x36, 0xF4, 0x01, 0x23, 0x0E, 0x00, 0x0F, 0x00, 0x2C,
0x03, 0x26, 0x00, 0xA9, 0xF4, 0x01, 0x19, 0x4E, 0x00, 0x1E, 0xF6, 0x01, 0x95, 0x0D, 0x00, 0xA5, 0x07, 0x00, 0x7D, 0x0F, 0x00, 0xD0, 0x05, 0x00,
0x13, 0xF9, 0x01, 0xA9, 0x03, 0x00, 0x66, 0x5B, 0x00, 0x0A, 0x0E, 0x00, 0xE3, 0x53, 0x00, 0x30, 0x2B, 0x00, 0xB8, 0x30, 0x01, 0x7C, 0xF4, 0x01,
0xA2, 0xF3, 0x01, 0x35, 0x0C, 0x00, 0x0D, 0x4E, 0x00, 0xB0, 0xF9, 0x01, 0xE6, 0x0C, 0x00, 0x8E, 0xF4, 0x01, 0xA9, 0x06, 0x00, 0xC7, 0xF4, 0x01,
0x5C, 0x0B, 0x00, 0x4B, 0x4E, 0x00, 0xD2, 0x05, 0x00, 0x13, 0xF9, 0x01, 0x28, 0x0F, 0x00, 0xF4, 0xF3, 0x01, 0x95, 0x0B, 0x00, 0x80, 0x0D, 0x00,
0x3E, 0xF3, 0x01, 0x7B,
};
    std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *)data, (sizeof data)-1);
    StampText<StampStringUTF8> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "񀀃󀀄񀁎񠀰𰇴󰀎𲰀򐀦򐇴󠁎񐇶񐀍󐀇𰀅򐇹񠀃򠁛𰀎S򀀫󀄰𠇴񐇳󐀌N񠇹󠀌򐇴񰀆󀇴򰀋𠁎𰀅򀇹􀀏񐇳󠀍",
        "StampText<StampStringUTF8>");
  }

  TEST_END;
}
