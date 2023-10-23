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
#include <vector>
#include <cassert>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"
#include "blobstamper/dict.h"

#include "test-chars-stamps.h"

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";
unsigned char bin_sample[]= {49, 22, 152, 226, 89, 119, 247, 115, 43, 42, 243, 71, 234, 231, 91, 35, 78, 108, 115, 39, 181, 248, 211, 52, 47, 94, 60, 237, 18, 39, 148, 62, 205, 214, 156, 184, 18, 201, 84, 183, 74, 134, 94, 72, 14, 116, 145, 109, 1, 230, 17, 95, 154, 100, 60, 15, 12, 102, 20, 115, 35, 183, 47, 176, 78, 176, 189, 113, 131, 93, 206, 62, 158, 166, 131, 95, 232, 217, 32, 171, 87, 31, 172, 160, 66, 160, 222, 134, 253, 1, 7, 191, 91, 125, 81, 148, 41, 46, 38, 171, 83, 215, 79, 34, 23, 215, 183, 118, 236, 191, 59, 160, 135, 58, 32, 199, 170, 183, 213, 53, 162, 138, 178, 118, 23, 202, 133, 8, 192, 183, 195, 199, 250, 29, 230, 34, 159, 10, 145, 74, 121, 85, 168, 204, 192, 25, 232, 88, 85, 76, 61, 168, 247, 128, 141, 176, 112, 113, 100, 201, 82, 183, 219, 236, 226, 171, 85, 97, 160, 1, 50, 250, 161, 51, 61, 220, 167, 227, 195, 17, 164, 211, 189, 130, 52, 167, 169, 42, 17, 29, 95, 15, 178, 165, 110, 87, 149, 214, 55, 12, 236, 138, 2, 245, 158, 84, 140, 24, 225, 169, 115, 16, 130, 253, 237, 182, 95, 109, 4, 28, 249, 4, 254, 166, 62, 107, 228, 113, 130, 127, 70, 79, 140, 41, 84, 218, 134, 146, 88, 65, 24, 174, 252, 253, 226, 214, 22, 92, 248, 14, 29, 60, 180, 94, 30, 186, 0};



//class SimpeRecursionNode;
//class TestRNode1;
//class TestRNode2;
//class TestRNode3;

size_t
Proportion(ORACLE_TYPE oracle, size_t min, size_t max)
{
  /* Sorry explanation is in Russian. PR translation if you can */
  /* Считаем пропорацию, с округлением вниз.
   * Диапазон увиличиваем на единицу, чтобы хвост диапазона пресказания при
   * округлении вниз как раз попадал в последний элемент целевого диапозона.
   * Разброс предсказания увеличиваем на единицу, так чтобы ровно на конец
   * хвоста диапазона предсказания не попасть никогда и тогда он не округлиться 
   * в max + 1*/
  size_t delta = max - min + 1;
  size_t res = floor(((float) oracle) / ((float) ORACLE_MAX + 1) * delta );
  return min + res;
}

class PoolPickerStamp : public virtual StampBaseStr
{
  protected:
    std::vector<std::shared_ptr<StampBaseStr>> pool;
    std::vector<std::weak_ptr<StampBaseStr>> weak_pool;

  public:
    PoolPickerStamp(std::vector<std::shared_ptr<StampBaseStr>> new_pool);
    ~PoolPickerStamp() {fprintf(stderr, "DESTROY!\n");};

    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    virtual void add_weak(std::shared_ptr<StampBaseStr> stamp);
    virtual bool isRecursive();
    virtual int minSize() override;
    virtual int maxSize() override;
};


PoolPickerStamp::PoolPickerStamp(std::vector<std::shared_ptr<StampBaseStr>> new_pool) 
        : pool{new_pool}
{
  for(auto stamp : pool)
  {
     std::weak_ptr<StampBaseStr> wp = stamp;
     weak_pool.push_back(wp);
  }
}

bool
PoolPickerStamp::isRecursive()
{
  if(is_recursive || is_in_recursion)
    return true;
  is_in_recursion = true;
  for(auto stamp : pool)
  {
    if (stamp->isRecursive())
    {
      is_recursive = true;  // Once recursive -- recursive forever.
      is_in_recursion = false;
      return true;
    }
  }
  is_in_recursion = false;
  return false;
}

std::string
PoolPickerStamp::ExtractStr(std::shared_ptr<Blob> blob)
{
fprintf(stderr, "*");
  static ORACLE_STAMP stamp_oracle;
  ORACLE_TYPE oracle = stamp_oracle.ExtractValue(blob);

  std::vector<std::weak_ptr<StampBaseStr>> target_pool;
  for(auto stamp : weak_pool)
  {
    if (stamp.lock()->minSize() <= blob->Size())
    {
       target_pool.push_back(stamp);
    }
  }

  size_t index = Proportion(oracle, 0, target_pool.size() - 1);
  return target_pool[index].lock()->ExtractStr(blob);
}

int
PoolPickerStamp::minSize()
{
  int res = INT_MAX;
  /* Do not check is_recursive here: even if stamp is known to be recursive we
   * still should iterate all his non-recursive children to find real minimal 
   * size */
  if (is_in_recursion)
    return res;
  is_in_recursion = 1; /* Do not use isRecursive() inside as it uses same flag*/
  for(auto stamp : pool)
  {
    int candidat = stamp->minSize();
    if (res > candidat)
         res = candidat;
  }
  is_in_recursion = 0;
  res += ORACLE_SIZE;
  return res;
}

int
PoolPickerStamp::maxSize()
{
  int res = 0;
  if (is_recursive || is_in_recursion)
    return -1;
  is_in_recursion = 1; /* Do not use isRecursive() inside as it uses same flag*/
  for(auto stamp : pool)
  {
    int candidat = stamp->maxSize();
    if (candidat == -1)
      return  -1;
    if (res < candidat)
         res = candidat;
  }
  is_in_recursion = 0;
  res += ORACLE_SIZE;
  return res;
}

void
PoolPickerStamp::add_weak(std::shared_ptr<StampBaseStr> stamp)
{
  weak_pool.push_back(stamp);
}

class StampJSONInt : public virtual StampArithm<long int>
{
};

class StampJSONFloat : public virtual StampArithm<double>
{
};

class StampJSONString : public virtual StampDictT<DictLCAlphaSmall>
{
  protected:
  public:
  std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};


std::string
StampJSONString::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::string res = "\"" + StampDictT<DictLCAlphaSmall>::ExtractStr(blob) +"\"";
  return res;
}


class StampJSONArray: public StampStrEnumerator
{
  private:
  public:
    StampJSONArray(std::shared_ptr<PoolPickerStamp> picker)
            :StampStrEnumerator(picker, ", ", "[", "]") {};
};



int
main()
{
 //   auto dict =std::make_shared<DictLCAlphaSmall>();
 //   auto stamp_d = std::make_shared<StampDict>(dict);
//    auto stamp_d = std::make_shared<StampDictT<DictLCAlphaSmall>>();
//    auto stamp_i = std::make_shared<StampArithm<long int>>();
//    auto stamp_f = std::make_shared<StampArithm<float>>();
//
    auto stamp_d = std::make_shared<StampJSONString>();
    auto stamp_i = std::make_shared<StampJSONInt>();
    auto stamp_f = std::make_shared<StampJSONFloat>();


//    PoolPickerStamp stamp({stamp_i, stamp_f, stamp_d});
    std::shared_ptr<PoolPickerStamp> picker(new PoolPickerStamp({stamp_i, stamp_f, stamp_d}));
 //   picker->add_weak(picker);
    auto stamp_a = std::make_shared<StampJSONArray>(picker);
    picker->add_weak(stamp_a);

    
    std::shared_ptr<Blob> blob = std::make_shared<Blob>((char *)bin_sample, strlen((char *)bin_sample));


    fprintf(stderr,"%i %i \n",stamp_a->minSize(), stamp_a->maxSize());
//    for(int i =0; i<25; i++)
    {
      std::string s = stamp_a->ExtractStr(blob);

      fprintf(stderr,"%i %s\n",picker->isRecursive(), s.c_str());
    }



    TEST_START(6);
    {
       is(Proportion(0,0,255), 0);
       is(Proportion(255,0,255), 0);
       is(Proportion(256,0,255), 1);
       is(Proportion(65535,0,255), 255);
       is(Proportion(65535-255,0,255), 255);
       is(Proportion(65535-256,0,255), 254);

    }
    TEST_END;
}
