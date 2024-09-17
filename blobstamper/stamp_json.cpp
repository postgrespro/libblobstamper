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


#include <string>
#include <vector>
#include <memory>

#include "stamp_json.h"
#include "oracle.h"

PoolPickerStamp::PoolPickerStamp(std::vector<std::shared_ptr<StampBaseStr>> new_pool)
        : pool{new_pool}
{
  for (auto stamp : pool)
  {
     std::weak_ptr<StampBaseStr> wp = stamp;
     weak_pool.push_back(wp);
  }
}

bool
PoolPickerStamp::isRecursive()
{
  if (is_recursive || is_in_recursion)
    return true;
  is_in_recursion = true;
  for (auto stamp : weak_pool)
  {
    if (stamp.lock()->isRecursive())
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
  static ORACLE_STAMP stamp_oracle;
  ORACLE_TYPE oracle = stamp_oracle.ExtractValue(blob);

  std::vector<std::weak_ptr<StampBaseStr>> target_pool;
  std::vector<std::weak_ptr<StampBaseStr>> unbounded_pool;

  for (auto stamp_w : weak_pool)
  {
    auto stamp = stamp_w.lock();
    if (stamp->minSize() <= blob->Size())
    {
      target_pool.push_back(stamp_w);
      if (stamp->maxSize() == -1 || stamp->maxSize() >= blob->Size())
      {
        unbounded_pool.push_back(stamp_w);
      }
    }
  }
  if (unbounded_pool.size()>0)
    target_pool = unbounded_pool;

  size_t index = OracleProportion(oracle, 0, target_pool.size() - 1);
  return target_pool[index].lock()->ExtractStr(blob);
}

int
PoolPickerStamp::minSize()
{
  int res = INT_MAX / 2;
  /* Do not check is_recursive here: even if stamp is known to be recursive we
   * still should iterate all his non-recursive children to find real minimal
   * size */
  if (is_in_recursion)
    return res;
  is_in_recursion = true; /* Do not use isRecursive() inside as it uses same flag*/
  for(auto stamp : weak_pool)
  {
    int candidat = stamp.lock()->minSize();
    if (res > candidat)
         res = candidat;
  }
  is_in_recursion = false;
  if (res == INT_MAX / 2)
      return INT_MAX / 2;
  res += ORACLE_SIZE;
  return res;
}

int
PoolPickerStamp::maxSize()
{
  int res = 0;
  if (is_recursive || is_in_recursion)
    return -1;
  is_in_recursion = true; /* Do not use isRecursive() inside as it uses same flag*/
  for (auto stamp : weak_pool)
  {
    int candidat = stamp.lock()->maxSize();
    if (candidat == -1)
    {
      is_in_recursion = false;
      return  -1;
    }
    if (res < candidat)
         res = candidat;
  }
  is_in_recursion = false;
  res += ORACLE_SIZE;
  return res;
}

void
PoolPickerStamp::add_weak(std::shared_ptr<StampBaseStr> stamp)
{
  weak_pool.push_back(stamp);
}

std::string
StampJSONString::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::string res = "\"" + StampDictT<DictLCAlphaSmall>::ExtractStr(blob) +"\"";
  return res;
}


std::string
StampJSONHashEl::ExtractStr(std::shared_ptr<Blob> blob)
{
   std::string n = stamp_name->ExtractStr(blob);
   std::string v = stamp_value->ExtractStr(blob);
   return n + ": " + v;
}

void null_deleter(StampJSON *) {}

StampJSON::StampJSON()
   : PoolPickerStamp({})
{
  stamp_i = std::make_shared<StampJSONInt>();
  stamp_f = std::make_shared<StampJSONFloat>();
  stamp_s = std::make_shared<StampJSONString>();

  // FIXME Так не надо делеать!!!! null_deleter -- зло.
  stamp_a = std::make_shared<StampJSONArray>(std::shared_ptr<StampJSON>(this, null_deleter));
  stamp_h = std::make_shared<StampJSONHash>(std::shared_ptr<StampJSON>(this, null_deleter));
  add_weak(stamp_i);
  add_weak(stamp_f);
  add_weak(stamp_s);

  add_weak(stamp_a);
  add_weak(stamp_h);
}

