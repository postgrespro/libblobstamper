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
#include <cassert>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include "blobstamper/blobstamper.h"

#include "test-chars-stamps.h"

using namespace TAP;

char short_sample[]="1234567";
char longer_sample[]="z1234567*89abcde&fghijklmnopqrstuvwxyzAB%CDEFGHIJKLMNOPQRSTUVWXYZ!";
unsigned char bin_sample[]= {49, 22, 152, 226, 89, 119, 247, 115, 43, 42, 243, 71, 234, 231, 91, 35, 78, 108, 115, 39, 181, 248, 211, 52, 47, 94, 60, 237, 18, 39, 148, 62, 205, 214, 156, 184, 18, 201, 84, 183, 74, 134, 94, 72, 14, 116, 145, 109, 1, 230, 17, 95, 154, 100, 60, 15, 12, 102, 20, 115, 35, 183, 47, 176, 78, 176, 189, 113, 131, 93, 206, 62, 158, 166, 131, 95, 232, 217, 32, 171, 87, 31, 172, 160, 66, 160, 222, 134, 253, 1, 7, 191, 91, 125, 81, 148, 41, 46, 38, 171, 83, 215, 79, 34, 23, 215, 183, 118, 236, 191, 59, 160, 135, 58, 32, 199, 170, 183, 213, 53, 162, 138, 178, 118, 23, 202, 133, 8, 192, 183, 195, 199, 250, 29, 230, 34, 159, 10, 145, 74, 121, 85, 168, 204, 192, 25, 232, 88, 85, 76, 61, 168, 247, 128, 141, 176, 112, 113, 100, 201, 82, 183, 219, 236, 226, 171, 85, 97, 160, 1, 50, 250, 161, 51, 61, 220, 167, 227, 195, 17, 164, 211, 189, 130, 52, 167, 169, 42, 17, 29, 95, 15, 178, 165, 110, 87, 149, 214, 55, 12, 236, 138, 2, 245, 158, 84, 140, 24, 225, 169, 115, 16, 130, 253, 237, 182, 95, 109, 4, 28, 249, 4, 254, 166, 62, 107, 228, 113, 130, 127, 70, 79, 140, 41, 84, 218, 134, 146, 88, 65, 24, 174, 252, 253, 226, 214, 22, 92, 248, 14, 29, 60, 180, 94, 30, 186, 0};



//class SimpeRecursionNode;
//class TestRNode1;
//class TestRNode2;
//class TestRNode3;


template<class T> using my_refvector = std::vector<std::reference_wrapper<T>>; /* This defines my_refvector type a shortcut to  std::vector<std::reference_wrapper<T>> */

class Variants
{
  protected:
    int variant_count = 1;
    int variant_selected = -1;
    bool enabled = true;

  public:
    void SetVariantCount(int i) {variant_count = i;};
    int GetVariantCount(){if(enabled) return variant_count; return 0;};
    void SetVariantSelected(int i) {variant_selected = i;};
    int GetVariantSelected(){return variant_selected;};
    void Enable(){enabled = true;};
    void Disable(){enabled = false;};
};

Variants&
VariantPicker(my_refvector<Variants> ref_vec, unsigned char seed)
{
  unsigned char max = 255;
  int total = 0;
  for(Variants& v : ref_vec)
  {
    total += v.GetVariantCount();
  }
  double step = (double)max / total;
  int i = 0;
  double sum = 0;


  Variants * last_variant;

  for(Variants& v : ref_vec)
  {
    if (v.GetVariantCount() == 0)
      continue;
    last_variant = &v;
    for(i=0; i <v.GetVariantCount(); i++)
    {
      sum += step /** v.GetVariantCount()*/;
      if (seed <= sum)
      {
        v.SetVariantSelected(i);
        return v;
      }
    }
  }

  // there mught be problem with precision that will not allow us to catch last item
  // but delta should be small. In other cases terminating
  if (abs( seed - sum) >0.1)
  {
    fprintf(stderr,"Something is really wrong\n");
    exit(1);
  }
  /* FIXME here we should solve case when there in no variants. Now it will segfault here */
  last_variant->SetVariantSelected(last_variant->GetVariantCount()-1);

  return ref_vec.back(); /*FIXME here should set last variant!!!*/
}

std::vector<unsigned int>
fit_sizes(std::vector<unsigned int> seeds, unsigned int fit_in)
{
  long seed_sum = 0;
  for(int seed  : seeds)
  {
    seed_sum += seed;
  }
  double proportion = (double) fit_in / (double) seed_sum;

  double remainder = 0;

  std::vector<unsigned int> res;

  int res_sum = 0;
  for(int seed  : seeds)
  {
    double d_size = (double) seed * proportion + remainder;
    unsigned int size = round(d_size);
    remainder = d_size - size;
    res.push_back(size);
    res_sum += size;
  }
  assert(res_sum == fit_in);
  return res;
}




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


class SimpeRecursionNode: public StampBase, public Variants
{

  public:
    GalleySimpleRecusion * recursor;
    virtual std::string do_recursion(Blob &blob) = 0;

    virtual std::string tmpID() = 0;
};
class TestRNode1: public SimpeRecursionNode
{
    std::vector<std::string> operations = {"+","-","*","/","^"}	;
  public:
    virtual std::string do_recursion(Blob &blob) override;
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */

    TestRNode1() {SetVariantCount(operations.size());};

    std::string tmpID() override {return "A";};
};

class TestRNode2: public SimpeRecursionNode
{
  public:
    virtual std::string do_recursion(Blob &blob) override;
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */

    std::string tmpID() override {return "B";}
};
class TestRNode3: public SimpeRecursionNode
{
  public:
    virtual std::string do_recursion(Blob &blob) override;
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */

    std::string tmpID() override {return "C";}
};

class GalleySimpleRecusion : public GalleyBase
{
   TestRNode1 node1;
   TestRNode2 node2;
   TestRNode3 node3;

    std::vector<std::reference_wrapper<SimpeRecursionNode>> stamps = {node1/*, node2, node3*/};

  public:
    int minSize() override {return 1; }; // FIXME correct it
    int maxSize() override {return -1;}; /* Sereies always takes as much data as it can take */


    virtual std::string do_recursion(Blob &blob);

};




std::string
TestRNode1::do_recursion(Blob &blob)
{
  int variant_n = GetVariantSelected(); /* Copy it as early as possible as it can be overwritten while recursion */

  StampArithm<unsigned char> stamp_char;
  
  StampArithm<unsigned short int> stamp_sint;

  unsigned short int spl_val;

  if (blob.Size() < (stamp_sint.minSize() + 4* stamp_char.minSize()) )
  {
    try{
      std::string val = stamp_char.ExtractStr(blob);
      return val;
    }
    catch (OutOfData)
    {
printf("___________________________ %i\n", blob.Size());
      return "Q";
    }

  }

  try{
    spl_val = stamp_sint.ExtractValue(blob);
  }
  catch (OutOfData)
  {
    return "Z";
  }

  std::vector<unsigned int> split_data_in = {spl_val, (unsigned short int) std::numeric_limits<unsigned short int>::max() - spl_val };
  std::vector<unsigned int> split_data = fit_sizes(split_data_in, blob.Size() - 4* stamp_char.minSize());

  printf("llll - %i %i \n",  split_data[0],  split_data[1]);
  
  Blob blob_left =  blob.ShiftBytes(split_data[0]+ 2*stamp_char.minSize() );

printf("~~~ %i\n",variant_n);

  return "(" + recursor->do_recursion(blob_left) + " " + operations[variant_n] +" "+ recursor->do_recursion(blob)+")";
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


  StampArithm<unsigned char> stamp_char;
  unsigned char c;
  try{
    c = stamp_char.ExtractValue(blob);
  }
  catch (OutOfData)
  {
    return "0";
  }


  std::vector<std::reference_wrapper<Variants>> stamps_v = {node1/*, node2, node3*/};


/*  printf("=================================\n");


    for(int i = 0; i<256;i+=1)
    {

      SimpeRecursionNode& res_node = (SimpeRecursionNode&) VariantPicker(stamps_v, i);
      printf("--- %s - %i \n",res_node.tmpID().c_str(), res_node.GetVariantSelected());
    }

*/


//  c = c % 3;

//  SimpeRecursionNode &node = stamps[c];
//   TestRNode3 node;



    SimpeRecursionNode& node = (SimpeRecursionNode&) VariantPicker(stamps_v, c);

   printf("--- %s - %i -  %i \n",node.tmpID().c_str(), c, node.GetVariantSelected());


   node.recursor = this;
   return node.do_recursion(blob);
}





int
main()
    {

    std::vector<unsigned int> v = {1,20,30,10,1};

    std::vector<unsigned int> res = fit_sizes(v, 6);
    for( unsigned int i : res)
    {
      printf("*** %i\n",i);
    }
    

/*
    char c1='a';
    char c2='b';
    char c3='c';
    char c4='d';

    WeightedRefPicker<char> wrp {{c1,1},{c2,2},{c3,3},{c4,4}};

    for(int i = 0; i<=255;i+=1)
    {
      char &rc = wrp.pick(i);
      fprintf(stderr,"^^ %i %c\n",i,rc);

    } */

    TEST_START(1);
    {
//       Blob blob(short_sample, strlen(short_sample));
       Blob blob((char *)bin_sample, strlen((char *)bin_sample));
       GalleySimpleRecusion galley;

fprintf(stderr,"--1\n");

       std::string str = galley.do_recursion(blob);

fprintf(stderr,"--99\n");

fprintf(stderr,"-- %s --\n",str.c_str());

       ok(1);
    }
    TEST_END;
}
