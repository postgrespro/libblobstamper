
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

class GalleySeries : public GalleyBase
{
  protected:
    StampBase &stamp;
  public:
    GalleySeries(StampBase & stamp_arg) : stamp(stamp_arg) {};
    std::list<Blob> extract_internal(Blob &blob);
    std::list<std::string> ExtractStr(Blob &blob);
    std::list<std::vector<char>> ExtractBin(Blob &blob);

    int minSize() override;
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
};

class GalleyVector : public GalleyBase
{
  protected:
    std::vector<std::reference_wrapper<StampBase>> stamps;
  public:
    GalleyVector(std::vector<std::reference_wrapper<StampBase>> arg) : stamps(arg) {};
    std::vector<Blob> extract_internal(Blob &blob);
    std::vector<std::string> ExtractStr(Blob &blob);
    std::vector<std::vector<char>> ExtractBin(Blob &blob);

    int minSize() override;
    int maxSize() override;
};


#endif /* GALLEY_H */
