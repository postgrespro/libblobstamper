
#ifndef STAMP_H
#define STAMP_H

#include <string>
#include <list>
#include <vector>


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


template<class T> class StampBaseT: public virtual StampBase
{
  public:
    virtual T ExtractValue(Blob &blob) = 0;/* Shoud be defined by derived classes*/
};

class StampFixed : public virtual StampBase
{
  protected:
     int  size;
  public:
    virtual int  minSize() override {return size;}
    virtual int  maxSize() override {return size;}

    std::vector<char> ExtractBin(Blob &blob) override;
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
