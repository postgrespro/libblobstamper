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
#include <list>

#include <string.h>

#include "blob.h"
#include "stamp.h"


void
StampBase::Load(Blob &blob)
{

  if (minSize() > blob.Size())
  {
    throw OutOfData();
  }

  size_t res_size;
  if (isUnbounded())
  {
    res_size = blob.Size();
  } else
  {
    res_size = maxSize();
    if (res_size > blob.Size())
        res_size = blob.Size();
  }
  Blob *pb = new Blob(blob.ShiftBytes(res_size));
  bitten_blob = std::unique_ptr<Blob>(pb);
}
