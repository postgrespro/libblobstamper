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
#include <sstream>

#include"stamp_text.h"
#include "utils.h"

std::string
StampTextPulp::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();
  if (data.empty())
  {
    return {};
  }
  std::ostringstream ss;

  for (auto sym : data)
  {
    if (sym == '\0') { sym = ' '; }
    ss << sym;
  }

  return ss.str();
}

std::string StampTextPulpWords::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::vector<std::string> data = ExtractStrVector(blob);
  return Utils::join(data, " ");
}

std::string StampTextDictWords::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::vector<std::string> data = ExtractStrVector(blob);
  return Utils::join(data, " ");
}

