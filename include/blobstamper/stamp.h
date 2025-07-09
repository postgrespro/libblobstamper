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

#ifndef STAMP_H
#define STAMP_H

#include <string>
#include <list>
#include <vector>
#include <memory>

#include "helpers.h"
#include "blob.h"


class StampBase
{
  protected:
    std::shared_ptr<Blob> bitten_blob;
    bool                  is_recursive = 0;
    bool                  is_in_recursion = 0;
  public:
    virtual int minSize() = 0;
    virtual int maxSize() = 0;

    void Load(std::shared_ptr<Blob> blob);

    bool isFixedSize() { return minSize() == maxSize(); }
    bool isVariated()  { return !isFixedSize() && !isUnbounded(); }
    bool isUnbounded() { return maxSize() == -1; }
    virtual bool isRecursive() {return is_recursive;}
};

class StampRecursive: public virtual StampBase
{
  protected:
//    bool is_in_recursion = 0;
  public:
    StampRecursive() {is_recursive = 1;}
    virtual ssize_t minSizeNR() = 0;
};


class StampBaseStr: public virtual StampBase
{
  public:
    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) = 0;
    std::string UnloadStr() {return ExtractStr(bitten_blob);};
};


class StampBaseBin: public virtual StampBase
{
  public:
    virtual std::vector<char> ExtractBin(std::shared_ptr<Blob> blob) = 0;
    std::vector<char> UnloadBin() {return ExtractBin(bitten_blob);};
};


template<class T> class StampBasePV: public StampBaseBin
{
  public:
    virtual sized_ptr<T> ExtractPValue(std::shared_ptr<Blob> blob) = 0;/* Should be defined by derived classes*/
    sized_ptr<T> UnloadPValue() {return ExtractPValue(bitten_blob);};
    virtual std::vector<char> ExtractBin(std::shared_ptr<Blob> blob) override;
};

/* If we have pointer and size, we can represent it std::vector<char> for free */
template<class T> std::vector<char>
StampBasePV<T>::ExtractBin(std::shared_ptr<Blob> blob)
{
  sized_ptr<T> sp = this->ExtractPValue(blob);
  T* pval = sp;
  std::vector<char> v((char *) pval, (char *) pval + sp.size());
  return v;
}

template<class T> class StampBaseV: public StampBasePV<T>,
public virtual StampBase //FIXME I do not understand why do we need it here, but wihtout it, it does not build
{
  public:
    virtual T ExtractValue(std::shared_ptr<Blob> blob) = 0;/* Should be defined by derived classes*/
    T UnloadValue() {return ExtractValue(bitten_blob);};

    virtual std::vector<char> ExtractBin(std::shared_ptr<Blob> blob) override;
    virtual sized_ptr<T> ExtractPValue(std::shared_ptr<Blob> blob) override;
};

template<class T> sized_ptr<T>
StampBaseV<T>::ExtractPValue(std::shared_ptr<Blob> blob)
{
  T* p = (T*) malloc(sizeof(T));
  *p = ExtractValue(blob);
  sized_ptr<T> res(p,sizeof(T));
  return res;
}

/* If we have value, we can represent it as binary */
template<class T> std::vector<char>
StampBaseV<T>::ExtractBin(std::shared_ptr<Blob> blob)
{
  T value = this->ExtractValue(blob);
  std::vector<char> v((char *) &value, (char *) &value + sizeof(T));
  return v;
}

#endif  /* STAMP_H */
