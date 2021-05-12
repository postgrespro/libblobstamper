#ifndef DICT_H
#define DICT_H


#include<string>
#include<vector>


class DictBase
{
  protected:
    std::vector<std::string> data;
  public:
    size_t size();
    std::string get(size_t n); // FIXME May be it would be good to use operator[]  later.
    DictBase() {data = {};};
};

class DictLCAlphaSmall : public DictBase
{
  public:
     DictLCAlphaSmall();
};

#endif /* DICT_H */