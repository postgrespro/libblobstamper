
#include <string>
#include <stdio.h>

class StampGeneric;

class Blob
{
    private:
    public:
        Blob(char * data, int size);
        bool isEmpty ();
        void Dump();
        Blob ShiftBytes(size_t n);

        void * ShiftSingleStampBin(StampGeneric &stmp);
        std::string  ShiftSingleStampStr(StampGeneric &stmp);

        char*  data;  /*FIXME потом сделать private*/
        int    size;
        int    begin;
        int    end;
};

class StampGeneric
{
  protected:
     bool is_fixed_size;
     int  min_size;
     int  max_size;
  public:
    bool isFixedSize() {return is_fixed_size;}
    int  minSize() {return min_size;}
    int  maxSize() {return max_size;}

    virtual void *      Extract(Blob &blob) {printf ("1111111\n"); return NULL;}
    virtual std::string ExtractStr(Blob &blob)  {printf ("22222\n"); return "";}

};

class StampBinDouble: public StampGeneric
{
  public:
    StampBinDouble();
    void * Extract(Blob &blob) override;


};


class StampStrDouble: public StampBinDouble
{
  public:
//    StampStrDouble();
    StampStrDouble() : StampBinDouble() {}

  std::string ExtractStr(Blob &blob) override;

};


class StampStrPgPoint: public StampGeneric
{
  protected:
    StampStrDouble stamp_double;
  public:
    StampStrPgPoint();
    std::string ExtractStr(Blob &blob) override;
};

Blob wflShiftN(Blob &blob, size_t n);
std::string wflShiftDouble(Blob &blob);
std::string wflShiftPgPoint(Blob &blob);
std::string wflShiftPgPath(Blob &blob);

void hexdump(void *pAddressIn, long  lSize);
