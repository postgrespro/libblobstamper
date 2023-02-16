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

#include "blob.h"
#include "stamp.h"
#include "stamp_arithm.h"
#include "stamp_dict.h"

int
StampDict::ChooseStampSize(std::shared_ptr<DictBase> dict)
{
    if (dict->size() <= UCHAR_MAX+1)
    {
        stamp_max_value = UCHAR_MAX;
        return 1;
    }
    if (dict->size() <= USHRT_MAX+1)
    {
        stamp_max_value = USHRT_MAX;
        return 2;
    }
    if (dict->size() <= UINT_MAX+1)
    {
        stamp_max_value = UINT_MAX;
        return 4;
    }
    stamp_max_value = ULONG_MAX;
    return 8;
}

std::string
StampDict::ExtractStr(std::shared_ptr<Blob> blob)
{
    unsigned long long index_oracle;

    /* Shifting index oracle according to size of dictionary index variable*/
    switch (stamp_size)
    {
       case 1:
       {
          index_oracle = stamp8.ExtractValue(blob);
          break;
       }
       case 2:
       {
          index_oracle = stamp16.ExtractValue(blob);
          break;
       }
       case 4:
       {
          index_oracle = stamp32.ExtractValue(blob);
          break;
       }
       case 8:
       {
          index_oracle = stamp64.ExtractValue(blob);
          break;
       }
       default:
         printf("StampDict::ExtractStr: Something is really wrong\n"); // FIXME better to throw something here :-)
         exit(1);
    }
    long long actual_index = ((double) index_oracle) / stamp_max_value * dict->size();
    if ( actual_index == dict->size())  actual_index--; /* If we hit the boundary step inside a bit*/
    return dict->get(actual_index);
}
