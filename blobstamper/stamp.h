
#ifndef STAMP_H
#define STAMP_H

#include <string>
#include <list>
#include <vector>

#include "helpers.h"


class StampBase
{
  public:
    virtual int  minSize() = 0;
    virtual int  maxSize() = 0;

    bool isFixedSize() {return minSize() == maxSize();}
    bool isVariated()  {return ! isFixedSize() && ! isUnbounded();}
    bool isUnbounded() {return maxSize() == -1;}

    virtual std::vector<char> ExtractBin(Blob &blob)  {throw NotImplemented();};
    virtual std::string ExtractStr(Blob &blob)  {throw NotImplemented();};
};


template<class T> class StampBasePV: public virtual StampBase
{
  public:
    virtual sized_ptr<T> ExtractPValue(Blob &blob) = 0;/* Shoud be defined by derived classes*/
    virtual std::vector<char> ExtractBin(Blob &blob) override;
};

/* If we have pointer and size, we can represent it std::vector<char> for free */
template<class T> std::vector<char>
StampBasePV<T>::ExtractBin(Blob &blob)
{
  sized_ptr<T> sp = this->ExtractPValue(blob);
  T* pval = sp;
  std::vector<char> v((char *) pval, (char *) pval + sp.size());
  return v;
}

template<class T> class StampBaseV: public StampBasePV<T>
{
  public:
    virtual T ExtractValue(Blob &blob) = 0;/* Shoud be defined by derived classes*/
    virtual std::vector<char> ExtractBin(Blob &blob) override;
    virtual sized_ptr<T> ExtractPValue(Blob &blob) override;
};

template<class T> sized_ptr<T>
StampBaseV<T>::ExtractPValue(Blob &blob)
{
  T* p = (T*) malloc(sizeof(T));
  *p = ExtractValue(blob);
  sized_ptr<T> res(p,sizeof(T));
  return res;
}

/* If we have value, we can represent it as binary */
template<class T> std::vector<char>
StampBaseV<T>::ExtractBin(Blob &blob)
{
  T value = this->ExtractValue(blob);
  std::vector<char> v((char *) &value, (char *) &value + sizeof(T));
  return v;
}

class StampFixed : public virtual StampBase
{
  protected:
     int  size;
  public:
    virtual int  minSize() override {return size;}
    virtual int  maxSize() override {return size;}
};

class StampVariated : public virtual StampBase
{
  protected:
     int  min_size;
     int  max_size;
  public:
    virtual int  minSize() override {return min_size;}
    virtual int  maxSize() override {return max_size;}
};

class StampUnbounded : public virtual StampBase
{
  protected:
     int  min_size;
  public:
    virtual int  minSize() override {return min_size;}
    virtual int  maxSize() override {return -1;}
};

#endif  /* STAMP_H */
