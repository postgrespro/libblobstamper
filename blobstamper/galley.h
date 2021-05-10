
#ifndef GALLEY_H
#define GALLEY_H


#include <limits.h>

#define ORACLE_STAMP StampBinInt16
#define ORACLE_TYPE unsigned short int
#define ORACLE_SIZE sizeof(ORACLE_TYPE)
#define ORACLE_MAX USHRT_MAX

class GalleyBase
{
  public:
    virtual int minSize() {return 1;};
    virtual int maxSize() {return 1;};
    virtual bool isFixedSize() {return 0;}

};

class GalleySeries : public GalleyBase
{
  protected:
    StampGeneric &stamp;
  public:
    GalleySeries(StampGeneric & stamp_arg) : stamp(stamp_arg) {};
    std::list<std::string> Extract(Blob &blob);
    int minSize() override;
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
    bool isFixedSize() override {return 0;} /* And not fixed size */

};

#endif /* GALLEY_H */
