#ifndef STAMP_ATOMIC_H
#define STAMP_ATOMIC_H

#include <string>

class StampBinDouble: public StampGeneric
{
  public:
    StampBinDouble();
    void * Extract(Blob &blob) override;
};


class StampStrDouble: public StampBinDouble
{
  public:
    StampStrDouble() : StampBinDouble() {}
    std::string ExtractStr(Blob &blob) override;
};



#endif  /* STAMP_ATOMIC_H */
