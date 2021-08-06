#ifndef STAMP_ATOMIC_H
#define STAMP_ATOMIC_H

#include <string>
#include "helpers.h"
#include "stamp.h"

template<class T> class StampArithm: public StampFixed, public StampBaseV<T>
{
  public:
    StampArithm() { size = sizeof(T);};
    virtual std::string ExtractStr(Blob &blob) override;
    virtual T ExtractValue(Blob &blob) override;
};

template<class T> std::string
StampArithm<T>::ExtractStr(Blob &blob)
{
    std::string res;
    std::vector<char> v = this->ExtractBin(blob);

    if (v.size() == 0)
        return "";

    T *pT = (T *) &v[0];

    return to_string_precise(*pT);
}

template<class T> T
StampArithm<T>::ExtractValue(Blob &blob)
{
  std::vector<char> v = blob.ChopBlank(*this);  /* Chop out blank of maxSize */
  T *pT = (T *) &v[0]; /* And interpret it as value of arithmetic type */
  return *pT;
}


#endif  /* STAMP_ATOMIC_H */
