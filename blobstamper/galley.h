
#ifndef GALLEY_H
#define GALLEY_H


#include <limits>
#include <vector>
#include <functional> // for std::reference_wrapper

#define ORACLE_TYPE unsigned short int
#define ORACLE_STAMP StampArithm<ORACLE_TYPE>
#define ORACLE_SIZE sizeof(ORACLE_TYPE)
#define ORACLE_MAX std::numeric_limits<ORACLE_TYPE>::max()

class GalleyBase
{
  public:
    virtual int minSize() = 0;
    virtual int maxSize() = 0;
    bool isFixedSize() {return minSize() == maxSize();}
    bool isUnbounded() {return maxSize() == -1;}

};

class GalleyVector : public GalleyBase
{
  protected:
    StampBase &stamp;
  public:
    GalleyVector(StampBase & stamp_arg) : stamp(stamp_arg) {};
    std::vector<Blob> extract_internal(Blob &blob);
    std::vector<std::string> ExtractStr(Blob &blob);
    std::vector<std::vector<char>> ExtractBin(Blob &blob);

    int minSize() override;
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
};

template<class T> class GalleyVectorV: public GalleyVector
{
    StampBaseV<T>& v_stamp;
  public:
    GalleyVectorV(StampBaseV<T> & stamp_arg): GalleyVector(stamp_arg), v_stamp(stamp_arg)  {};
    std::vector<T> ExtractValues(Blob &blob);
};


template<class T> std::vector<T>
GalleyVectorV<T>::ExtractValues(Blob &blob)
{
  std::vector<Blob> blobs = extract_internal(blob);
  std::vector<T> res(blobs.size());

  for(int i=0; i<blobs.size(); i++)
  {
    res[i] = v_stamp.ExtractValue(blobs[i]);
  }
  return res;
}


class GalleySet : public GalleyBase
{
  protected:
    std::vector<std::reference_wrapper<StampBase>> stamps;
  public:
    GalleySet(std::vector<std::reference_wrapper<StampBase>> arg) : stamps(arg) {};
    std::vector<Blob> extract_internal(Blob &blob);
    std::vector<std::string> ExtractStr(Blob &blob);
    std::vector<std::vector<char>> ExtractBin(Blob &blob);

    int minSize() override;
    int maxSize() override;
};


#endif /* GALLEY_H */
