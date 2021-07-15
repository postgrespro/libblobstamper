
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

    virtual std::vector<char> ExtractBin(Blob &blob)  {printf ("Not implemented"); exit(1);}
    virtual std::string ExtractStr(Blob &blob)  {printf ("Not implemented"); exit(1);}
};


class StampFixed : public StampBase
{
  protected:
     int  size;
  public:
    virtual int  minSize() {return size;}
    virtual int  maxSize() {return size;}

    std::vector<char> ExtractBin(Blob &blob) override;
};

class StampVariated : public StampBase
{
  protected:
     int  min_size;
     int  max_size;
  public:
    virtual int  minSize() {return min_size;}
    virtual int  maxSize() {return max_size;}
};

class StampUnbounded : public StampBase
{
  protected:
     int  min_size;
  public:
    virtual int  minSize() {return min_size;}
    virtual int  maxSize() {return -1;}
};

#endif  /* STAMP_H */
