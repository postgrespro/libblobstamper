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

#ifndef STAMP_DICT_H
#define STAMP_DICT_H

#include <limits.h>

#include "blob.h"
#include "stamp.h"
#include "stamp_arithm.h"
#include "dict.h"
#include <memory>

class StampDict: public StampBaseStr
{
  protected:
    StampArithm<unsigned char>  stamp8;
    StampArithm<unsigned short int> stamp16;
    StampArithm<unsigned int> stamp32;
    StampArithm<unsigned long long> stamp64;
    int stamp_size;
    std::shared_ptr<DictBase> dict;
    unsigned long long stamp_max_value;

    int ChooseStampSize(std::shared_ptr<DictBase> dict);

  public:
    StampDict(std::shared_ptr<DictBase> dict_arg) : dict(dict_arg),  stamp_size(ChooseStampSize(dict_arg)) {};
    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    int  minSize() override {return stamp_size;}
    int  maxSize() override {return stamp_size;}
};

class StampDictLCAlphaSmall: public StampDict
{
  public:
    StampDictLCAlphaSmall(): StampDict(std::make_shared<DictLCAlphaSmall>()) {};
};


template<class T> class StampDictT: public StampDict
{
  public:
    StampDictT<T>(): StampDict(std::make_shared<T>()) {};
};


#endif /* STAMP_DICT_H */
