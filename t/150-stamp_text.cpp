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
    TEST_START(3);
    { /* 1..1 */
		  char data[] = "папа\0мама\0бабушка\0дедушка\0братик\0сестричка";
		  std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
		  StampTextPulp stamp;
		  std::string s = stamp.ExtractStr(blob);
      is(s, "папа мама бабушка дедушка братик сестричка", "StampTextSimple");

    }
    { /* 2..2 */
		  char data[] = "dad\0mam\0granddad\0grandmam\0brother\0sister";
		  std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
		  StampTextPulpWords stamp;
		  std::string s = stamp.ExtractStr(blob);
      is(s, "d dad gra n dmam broth er siste", "GalleyTextSimple");

    }
    { /* 3..3 */
		  char data[] = "abcdef" "abcdef" "ABCDEF" "012345";
		  std::shared_ptr<Blob> blob = std::make_shared<Blob>(data, (sizeof data)-1);
		  StampTextDictWords stamp;
		  std::string s = stamp.ExtractStr(blob);
      is(s, "gleam godfather graffiti greened grouping gunshots gleam godfather graffiti greened grouping gunshots dismally dissented divested doorstep dread drunks convertors corpulent counterparts cranking crippled crusades", "GalleyLCAlphaSmall");

    }
    TEST_END;
}
