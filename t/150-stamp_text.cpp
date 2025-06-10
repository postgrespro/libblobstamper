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
  TEST_START(5);
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
    char data[] = "dad\0mam\0granddad\0grandmam\0brother\0sister";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampText<StampStringLatin1> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "d dad gra n dmam broth er siste", "StampText<StampStringLatin1>");
  }

  { /* 4..4 */
    char data[] = "abcdef" "abcdef" "ABCDEF" "012345";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampText<StampDictLCAlphaSmall> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "gleam godfather graffiti greened grouping gunshots gleam godfather graffiti greened grouping gunshots dismally dissented divested doorstep dread drunks convertors corpulent counterparts cranking crippled crusades", "StampText<StampDictLCAlphaSmall>");
  }

  { /* 5..5 */
    char data[] = "Некоторый текст написанный русскими буквами в кодировке utf-8";
    std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
    StampText<StampStringASCII> stamp;
    std::string s = stamp.ExtractStr(blob);
    is(s, "P9  Q  Q\x3Q \x1Q \x1P: P8 P<P 8 P 1Q\x3 P:P 2P0 P<P 8 P 2  P: P>P 4P8 Q  P>P 2P :P5  ut f-8", "StampText<StampStringASCII>");
  }

  TEST_END;
}
