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

#ifndef STAMP_JSON_H
#define STAMP_JSON_H

#include <string>
#include <limits>

#include "stamp.h"
#include "stamp_arithm.h"
#include "stamp_dict.h"
#include "stamp_enumerator.h"

class PoolPickerStamp: public virtual StampBaseStr
{
  protected:
    std::vector<std::shared_ptr<StampBaseStr>> pool;
    std::vector<std::weak_ptr<StampBaseStr>> weak_pool;

  public:
    PoolPickerStamp(std::vector<std::shared_ptr<StampBaseStr>> new_pool);
//    ~PoolPickerStamp() {fprintf(stderr, "DESTROY!\n");};

    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    virtual void add_weak(std::shared_ptr<StampBaseStr> stamp);
    virtual bool isRecursive();
    virtual int minSize() override;
    virtual int maxSize() override;
};

class StampJSONInt: public virtual StampArithm<long int>
{
};

class StampJSONFloat: public virtual StampArithm<double>
{
};

class StampJSONString: public virtual StampDictT<DictLCAlphaSmall>
{
  protected:
  public:
  std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    virtual int minSize() override {return 8;};
    virtual int maxSize() override {return 8;};

};

class StampJSONArray: public StampStrEnumerator
{
  private:
  public:
    StampJSONArray(std::shared_ptr<PoolPickerStamp> picker):
                   StampStrEnumerator(picker, ", ", "[", "]") {};
};

class StampJSONHashEl: public StampBaseStr
{
  private:
    std::shared_ptr<StampJSONString> stamp_name;
    std::shared_ptr<PoolPickerStamp> stamp_value;
  public:
    StampJSONHashEl(std::shared_ptr<PoolPickerStamp> picker):
                    stamp_value(picker), stamp_name(std::make_shared<StampJSONString>()) {};
    virtual int minSize() override {return stamp_name->minSize() + stamp_value->minSize();};
    virtual int maxSize() override {return -1;};
    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};

class StampJSONHash: public StampStrEnumerator
{
  private:
    std::shared_ptr<StampJSONHashEl> stamp_el;
  public:
    StampJSONHash(std::shared_ptr<PoolPickerStamp> picker):
                  StampStrEnumerator(stamp_el = std::make_shared<StampJSONHashEl>(picker), ", ", "{", "}") {};
};


class StampJSON: public PoolPickerStamp
{
  private:
    std::shared_ptr<StampJSONString> stamp_s;
    std::shared_ptr<StampJSONInt>    stamp_i;
    std::shared_ptr<StampJSONFloat>  stamp_f;
    std::shared_ptr<StampJSONArray>  stamp_a;
    std::shared_ptr<StampJSONHash>   stamp_h;

  public:
   StampJSON();
};

#endif /* STAMP_DICT_H */

