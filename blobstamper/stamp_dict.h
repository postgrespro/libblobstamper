
#ifndef STAMP_DICT_H
#define STAMP_DICT_H

#include <limits.h>

#include "blob.h"
#include "stamp.h"
#include "stamp_arithm.h"
#include "dict.h"

class StampDict: public StampFixed
{
  protected:
    StampArithm<char>  stamp8;
    StampArithm<unsigned short int> stamp16;
    StampArithm<unsigned int> stamp32;
    StampArithm<unsigned long long> stamp64;
    StampFixed& stamp;
    DictBase& dict;
    unsigned long long stamp_max_value;

    StampFixed& GuessStamp(DictBase & dict);

  public:
    StampDict(DictBase & dict_arg) : dict{dict_arg},  stamp{GuessStamp(dict_arg)} {};
    std::string ExtractStr(Blob &blob) override;
    int  minSize() override {return stamp.minSize();}
    int  maxSize() override {return stamp.maxSize();}
};

#endif /* STAMP_DICT_H */
