#ifndef STAMP_ATOMIC_H
#define STAMP_ATOMIC_H

#include <string>


class StampBinChar: public StampGeneric
{
  public:
    StampBinChar();
};

class StampStrUInt8: public StampBinChar
{
  public:
    std::string ExtractStr(Blob &blob) override;
};

/* --- */

class StampBinInt16: public StampGeneric
{
  public:
    StampBinInt16();
};

class StampStrUInt16: public StampBinInt16
{
  public:
    std::string ExtractStr(Blob &blob) override;
};

class StampStrSInt16: public StampBinInt16
{
  public:
    std::string ExtractStr(Blob &blob) override;
};


/* --- */

class StampBinInt32: public StampGeneric
{
  public:
    StampBinInt32();
};

class StampStrUInt32: public StampBinInt32
{
  public:
    std::string ExtractStr(Blob &blob) override;
};

class StampStrSInt32: public StampBinInt32
{
  public:
    std::string ExtractStr(Blob &blob) override;
};

/* --- */

class StampBinInt64: public StampGeneric
{
  public:
    StampBinInt64();
};

class StampStrUInt64: public StampBinInt64
{
  public:
    std::string ExtractStr(Blob &blob) override;
};

class StampStrSInt64: public StampBinInt64
{
  public:
    std::string ExtractStr(Blob &blob) override;
};

/* --- */


class StampBinDouble: public StampGeneric
{
  public:
    StampBinDouble();
};


class StampStrDouble: public StampBinDouble
{
  public:
    std::string ExtractStr(Blob &blob) override;
};



#endif  /* STAMP_ATOMIC_H */
