
#ifndef STAMP_DICT_H
#define STAMP_DICT_H

#include <limits.h>

#include "dict.h"

class StampDict: public StampGeneric
{
  protected:
    StampBinChar  stamp8;
    StampBinInt16 stamp16;
    StampBinInt32 stamp32;
    StampBinInt64 stamp64;
    StampGeneric& stamp;
    DictBase& dict;
    unsigned long long stamp_max_value;

    StampGeneric& GuessStamp(DictBase & dict);

  public:
    StampDict(DictBase & dict_arg) : dict{dict_arg},  stamp{GuessStamp(dict_arg)} {};
    std::string ExtractStr(Blob &blob) override;
    bool isFixedSize() override {return true;}
    int  minSize() override {return stamp.minSize();}
    int  maxSize() override {return stamp.maxSize();}
};

#endif /* STAMP_DICT_H */
