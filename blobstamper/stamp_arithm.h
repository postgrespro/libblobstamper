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

#ifndef STAMP_ATOMIC_H
#define STAMP_ATOMIC_H

#include <string>
#include "helpers.h"
#include "stamp.h"

template<class T> class StampArithm: public StampFixed, public StampBaseStr, public StampBaseV<T>
{
  public:
    StampArithm() { size = sizeof(T);};
    virtual std::string ExtractStr(Blob &blob) override;
    virtual T ExtractValue(Blob &blob) override;
};

template<class T> std::string
StampArithm<T>::ExtractStr(Blob &blob)
{
    std::string res;
    std::vector<char> v = this->ExtractBin(blob);

    if (v.size() == 0)
        return "";

    T *pT = (T *) &v[0];

    return to_string_precise(*pT);
}

template<class T> T
StampArithm<T>::ExtractValue(Blob &blob)
{
  std::vector<char> v = blob.ChopBlank(*this);  /* Chop out blank of maxSize */
  T *pT = (T *) &v[0]; /* And interpret it as value of arithmetic type */
  return *pT;
}


#endif  /* STAMP_ATOMIC_H */
