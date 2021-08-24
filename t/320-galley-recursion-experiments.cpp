/******************************************************************************
 *
 * Copyright 2021 Nikolay Shaplov (Postgres Professional)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#include <string.h>

#include <exception>
#include <string>
#include <cstdlib>
#include <vector>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"

#include "test-chars-stamps.h"

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";


//class SimpeRecursionNode;
//class TestRNode1;
//class TestRNode2;
//class TestRNode3;


template<class T> class WeightedRef
{
  public:
    std::reference_wrapper<T> ref;
    char weight;
    WeightedRef(T& arg_ref): ref{arg_ref}, weight{1} {}
    WeightedRef(T& arg_ref, char weight_arg): ref{arg_ref}, weight{weight_arg} {}
};


template<class T> class WeightedRefPicker
{
  std::vector<WeightedRef<T>> vec;
  public:
  WeightedRefPicker(std::initializer_list<WeightedRef<T>> arg): vec{arg} {}
  std::reference_wrapper<T> pick(unsigned char c);
};


template<class T> std::reference_wrapper<T>
WeightedRefPicker<T>::pick(unsigned char c)
{
  unsigned char max = 255;
  int total = 0;
  for(WeightedRef<T> & wr : vec)
  {
    total += wr.weight;
  }
  double step = (double)max / total;
  int i = 0;
  double sum = 0;


  for(WeightedRef<T> & wr : vec)
  {
    sum += step * wr.weight;
    if (c <= sum)
      return wr.ref;
  }

  // there mught be problem with precision that will not allow us to catch last item
  // but delta should be small. In other cases terminating
  if (abs(c-sum) >0.1)
  {
    fprintf(stderr,"Something is really wrong\n");
    exit(1);
  }
  return vec.back().ref;
}




class GalleySimpleRecusion;


class SimpeRecursionNode: public StampBase
{

  public:
    GalleySimpleRecusion * recursor;
    virtual std::string do_recursion(Blob &blob) = 0;

};
class TestRNode1: public SimpeRecursionNode
{
  public:
    virtual std::string do_recursion(Blob &blob) override;
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
};
class TestRNode2: public SimpeRecursionNode
{
  public:
    virtual std::string do_recursion(Blob &blob) override;
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
};
class TestRNode3: public SimpeRecursionNode
{
  public:
    virtual std::string do_recursion(Blob &blob) override;
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */
};

class GalleySimpleRecusion : public GalleyBase
{
   TestRNode1 node1;
   TestRNode2 node2;
   TestRNode3 node3;

    std::vector<std::reference_wrapper<SimpeRecursionNode>> stamps = {node1, node2, node3};

  public:
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */


    virtual std::string do_recursion(Blob &blob);

};




std::string
TestRNode1::do_recursion(Blob &blob)
{
  try{
  Blob tmp = blob.ShiftBytes(1);
  }
  catch (OutOfData)
  {
    return "0";
  }

  return "1 + " + recursor->do_recursion(blob);
}

std::string
TestRNode2::do_recursion(Blob &blob)
{
  try{
  Blob tmp = blob.ShiftBytes(1);
  }
  catch (OutOfData)
  {
    return "0";
  }

  return "2 + " + recursor->do_recursion(blob);
}


std::string
TestRNode3::do_recursion(Blob &blob)
{
  try{
  Blob tmp = blob.ShiftBytes(1);
  }
  catch (OutOfData)
  {
    return "0";
  }

  return "3 + " + recursor->do_recursion(blob);
}

std::string
GalleySimpleRecusion::do_recursion(Blob& blob)
{
  StampArithm<char> stamp_char;
  char c;
  try{
    c = stamp_char.ExtractValue(blob);
  }
  catch (OutOfData)
  {
    return "0";
  }

  c = c % 3;

  SimpeRecursionNode &node = stamps[c];
//   TestRNode3 node;
   node.recursor = this;
   return node.do_recursion(blob);
}






int
main()
    {

    char c1='a';
    char c2='b';
    char c3='c';
    char c4='d';

    WeightedRefPicker<char> wrp {{c1,1},{c2,2},{c3,3},{c4,4}};

    for(int i = 0; i<=255;i+=1)
    {
      char &rc = wrp.pick(i);
      fprintf(stderr,"^^ %i %c\n",i,rc);

    }

    TEST_START(1);
    {
       Blob blob(short_sample, strlen(short_sample));
       GalleySimpleRecusion galley;

fprintf(stderr,"--1\n");

       std::string str = galley.do_recursion(blob);

fprintf(stderr,"--99\n");

fprintf(stderr,"-- %s --\n",str.c_str());

       ok(1);
    }
    TEST_END;
}
