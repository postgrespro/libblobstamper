#ifndef STAMP_ATOMIC_H
#define STAMP_ATOMIC_H

#include <string>
#include "helpers.h"

template<class T> class StampArithm: public StampFixed
{
  public:
    StampArithm() { size = sizeof(T);};
    std::string ExtractStr(Blob &blob) override;
};

template<class T> std::string
StampArithm<T>::ExtractStr(Blob &blob)
{
    std::string res;
    T *pT = (T *)this->ExtractBin(blob);
    if (! pT)
        return "";
    res = to_string_precise(*pT);
    free(pT);
    return res;
}

#endif  /* STAMP_ATOMIC_H */
