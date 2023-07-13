/******************************************************************************
 *
 * Copyright 2021-2023 Nikolay Shaplov (Postgres Professional)
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

#include <cstring>

#include "blob.h"
#include "helpers.h"

#include "stamp.h"


Blob::Blob (char * data_in, size_t size_in)
{
    data = data_in;
    size = size_in;
    begin = 0;
    end = size -1; /* i.e. size=1 means begin=0 && end=0 */
}

bool
Blob::isEmpty ()
{
    if (! data) return true;
    return false;
}

void
Blob::Dump()
{
    size_t length = end - begin +1 ;
    hexdump(data + begin, length);
}


std::shared_ptr<Blob>
Blob::Chop(size_t chop_size)
{
    if (this->Size() < chop_size)
    {
        throw OutOfData();
    }

    std::shared_ptr<Blob> new_blob = std::make_shared<Blob>(this->data + begin, chop_size);
    begin += chop_size;

    return new_blob;
}

std::shared_ptr<Blob>
Blob::Chop(size_t min_size, size_t max_size)
{
    if (this->Size() < min_size)
    {
        throw OutOfData();
    }
    if (this->Size() >= max_size)
        return this->Chop(max_size);

    return this->Chop(this->Size());
}

std::vector<char>
Blob::AsByteVector()
{
    std::vector<char> res(data + begin, data + begin + size);
    return res;
}

size_t
Blob::Size()
{
  return end - begin + 1;
}

void
Blob::DataDup(char *& data_out, size_t& size_out)
{
  size_out = Size();
  data_out = (char *)malloc(size_out);
  //FIXME add out of memory check here!!!!
  memcpy(data_out, data + begin, size_out);
}

std::vector<char>
Blob::asVector()
{
  std::vector<char> res( (char *)data + begin, (char*)data + begin + Size());

//  memcpy(&res[0], data + begin, Size());
  return res;
}

/* Do not use in prod. For tests and examples only */
std::string
Blob::asString()
{
  std::string res((char *)data + begin, Size());
  return res;
}
