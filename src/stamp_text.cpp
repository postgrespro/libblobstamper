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

#include "blobstamper/stamp_text.h"

std::string
StampStringLatin1::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();

    std::vector<char>::iterator iterator;

    iterator = data.begin();
    std::string res;
    while (iterator != data.end())
    {
      if (*iterator == '\0') { *iterator = ' '; }
      res.push_back(*iterator++);
    }

    return res;
}

std::string
StampStringASCII::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();

    std::vector<char>::iterator iterator;

    iterator = data.begin();
    std::string res;
    while (iterator != data.end())
    {
      *iterator &= 0x7F; // Remove first bit. This will made it ASCII (or more precisely 7bit encoding including character 0x01..0x1F)
      if (*iterator == '\0') { *iterator = ' '; }
      res.push_back(*iterator++);
    }
    return res;
}

