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

#ifndef GALLEY_H
#define GALLEY_H


#include <limits>
#include <vector>
#include <functional> // for std::reference_wrapper

#include "stamp.h"
#include "blob.h"


#define ORACLE_TYPE unsigned short int
#define ORACLE_STAMP StampArithm<ORACLE_TYPE>
#define ORACLE_SIZE sizeof(ORACLE_TYPE)
#define ORACLE_MAX std::numeric_limits<ORACLE_TYPE>::max()

class GalleyBase: public virtual StampBase
/* Galley is a kind of stamp, somwhere deep inside. */
/* You can inherit it, and make a stamp out of it*/
{

};

class GalleyVectorBase : public GalleyBase
{
  protected:
    std::shared_ptr<StampBase> stamp;
  public:
    GalleyVectorBase(std::shared_ptr<StampBase> stamp_arg) : stamp(stamp_arg) {};
    std::vector<std::shared_ptr<Blob>> extract_internal(std::shared_ptr<Blob> blob);
    int minSize() override;
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
};


class GalleyVectorStr: public GalleyVectorBase
{
  public:
    GalleyVectorStr(std::shared_ptr<StampBaseStr> stamp_arg): GalleyVectorBase(stamp_arg) {};
    std::vector<std::string> ExtractStrVector(std::shared_ptr<Blob> blob);
};

template<class T> class GalleyVectorStrStampBase:  public GalleyVectorStr, public StampBaseStr
{
  public:
    GalleyVectorStrStampBase(): GalleyVectorStr(std::make_shared<T>()) {};

};


class GalleyVectorBin: public GalleyVectorBase
{
    std::shared_ptr<StampBaseBin> b_stamp;
  public:
    GalleyVectorBin(std::shared_ptr<StampBaseBin> stamp_arg): GalleyVectorBase(stamp_arg), b_stamp(stamp_arg)  {};
    std::vector<std::vector<char>> ExtractBinVector(std::shared_ptr<Blob> blob);
};


template<class T> class GalleyVectorV: public GalleyVectorBase
{
    std::shared_ptr<StampBaseV<T>> v_stamp;
  public:
    GalleyVectorV(std::shared_ptr<StampBaseV<T>> stamp_arg): GalleyVectorBase(stamp_arg), v_stamp(stamp_arg)  {};
    std::vector<T> ExtractValuesVector(std::shared_ptr<Blob> blob);
};


template<class T> std::vector<T>
GalleyVectorV<T>::ExtractValuesVector(std::shared_ptr<Blob> blob)
{
  std::vector<std::shared_ptr<Blob>> blobs = extract_internal(blob);
  std::vector<T> res(blobs.size());

  for(int i=0; i<blobs.size(); i++)
  {
    res[i] = v_stamp->ExtractValue(blobs[i]);
  }
  return res;
}


class GalleySetBase : public GalleyBase
{
  protected:
    std::vector<std::reference_wrapper<StampBase>> stamps;
  public:
    GalleySetBase(std::vector<std::reference_wrapper<StampBase>> arg) : stamps(arg) {};
    std::vector<std::shared_ptr<Blob>> extract_internal(std::shared_ptr<Blob> blob);
    void LoadAll(std::shared_ptr<Blob> blob);

    int minSize() override;
    int maxSize() override;
};

class GalleySetBin : public GalleySetBase
{
    std::vector<std::reference_wrapper<StampBaseBin>> b_stamps;
  public:
    GalleySetBin(std::vector<std::reference_wrapper<StampBaseBin>> arg) : GalleySetBase(cast_arg(arg)), b_stamps(arg) {};
    std::vector<std::vector<char>> ExtractBinSet(std::shared_ptr<Blob> blob);

    std::vector<std::reference_wrapper<StampBase>> cast_arg(std::vector<std::reference_wrapper<StampBaseBin>> in)
    {
      std::vector<std::reference_wrapper<StampBase>> res;
      for(StampBaseBin & s : in)
      {
        res.push_back(s);
      }
      return res;
    };
};


class GalleySetStr : public GalleySetBase
{
    std::vector<std::reference_wrapper<StampBaseStr>> s_stamps;
  public:
    GalleySetStr(std::vector<std::reference_wrapper<StampBaseStr>> arg) : GalleySetBase(cast_arg(arg)), s_stamps(arg) {}; 
    std::vector<std::string> ExtractStrSet(std::shared_ptr<Blob> blob);

    std::vector<std::reference_wrapper<StampBase>> cast_arg(std::vector<std::reference_wrapper<StampBaseStr>> in)
    {
      std::vector<std::reference_wrapper<StampBase>> res;
      for(StampBaseStr & s : in)
      {
        res.push_back(s);
      }
      return res;
    };
};


#endif /* GALLEY_H */
