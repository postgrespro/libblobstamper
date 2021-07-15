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
    std::vector<char> bin = this->ExtractBin(blob);

    if (bin.size() == 0)
        return "";

    T *pT = (T *) &bin[0];

    return to_string_precise(*pT);
}

#endif  /* STAMP_ATOMIC_H */
