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

#include"stamp_text.h"

std::string
StampTextPulp::ExtractStr(std::shared_ptr<Blob> blob)
{

    std::vector<char> data = blob->ChopBlank(*this);

    std::vector<char>::iterator the_iterator;

    the_iterator = data.begin();
    std:: string res;
    while (the_iterator != data.end())
    {
      if (*the_iterator == '\0') { *the_iterator = ' '; }
      res.push_back(*the_iterator++);
    }

    return res;
}

std::string StampTextPulpWords::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::vector<std::string> data = ExtractStrVector(blob);
  std::string res = "";

  for(std::string s : data)
  {
    if (!res.empty())
    {
      res+=" ";
    }
    res+= s;
  }
  return res;
}

std::string StampTextDictWords::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::vector<std::string> data = ExtractStrVector(blob);
  std::string res = "";

  for(std::string s : data)
  {
    if (!res.empty())
    {
      res+=" ";
    }
    res+= s;
  }
  return res;
}

