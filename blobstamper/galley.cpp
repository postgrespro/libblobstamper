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

#include <vector>
#include <functional>
#include <cmath>

#include "blob.h"
#include "stamp.h"
#include "stamp_arithm.h"
#include "galley.h"


int
GalleyVectorBase::minSize()
{
  if (stamp.isFixedSize())
  {
    return stamp.minSize(); // When size is fixed, series can have only one member with no extra data used
  }
  else
  {
    if (stamp.isUnbounded())
    {
      return stamp.minSize() + ORACLE_SIZE * 2; // One -- count oracle, one -- size oracle
    }
    else
    {
      return stamp.minSize() + ORACLE_SIZE; // At leas one element with an oracle
    }
  }
}



std::vector<std::string>
GalleyVectorStr::ExtractStrVector(Blob &blob)
{
  std::vector<Blob> blobs = extract_internal(blob);
  std::vector<std::string> res(blobs.size());

  for(int i = 0; i<blobs.size(); i++)
  {
    res[i] = s_stamp.ExtractStr(blobs[i]);
  }
  return res;
}

std::vector<std::vector<char>>
GalleyVectorBin::ExtractBinVector(Blob &blob)
{
  std::vector<Blob> blobs = extract_internal(blob);
  std::vector<std::vector<char>> res(blobs.size());

  for(int i = 0; i<blobs.size(); i++)
  {
    res[i] = b_stamp.ExtractBin(blobs[i]);
  }
  return res;
}

std::vector<Blob>
GalleyVectorBase::extract_internal(Blob &blob)
{
  if (blob.Size()<stamp.minSize())
  {
     throw OutOfData(); /* FIXME: May be later add option that allows empty lists if needed*/
  }
  std::vector<Blob> res;
  if (stamp.isFixedSize())
  {
    int size = stamp.minSize();
    while (blob.Size() >= size)
    {
      Blob el = blob.ShiftBytes(size);
      res.push_back(el);
    }
  }
  else
  {
    if (stamp.isUnbounded())
    {
      /*
         The idea of this part is following:
         We get two bytes from blob, and treat it as an oracle that says in how many parts blob shold be split.

         We use stamp.minSize() and blob.Size() to determinate how many parts are possible at maximum (count_max)
         and normalize oracle so it would fit count_max in any case. Thus we get count_target the number of parts we
         wil have

         Then we shift a size oracle for each part

         Then we normalize size oracles to they in total will give us the size of remaining blob. Here we should 
         keep in mind that part cant' be shorter than stamp.minSize()

         Then we chop blob into selected sizes, and stap each chopped part with stamp.
      */

      /* Getting count oracle and normalze it to fit available size */
      size_t count_max = (blob.Size() - ORACLE_SIZE) / (stamp.minSize() + ORACLE_SIZE);  //First oracle - for number of items, and second one is oracle for each item size

      ORACLE_STAMP stamp_oracle;
      ORACLE_TYPE count_oracle = stamp_oracle.ExtractValue(blob);

      ORACLE_TYPE count_target = count_max * count_oracle / ORACLE_MAX + 1; /* +1 -- это грубая эмуляция округления вверх. oracle == ORACLE_MAX-1 == 65534 должен дать count_max*/
      if (count_target > count_max) count_target = count_max; // В случае если oracle оказался рваен ORACLE_MAX

      /* Getting size oracles for each part */
      std::vector<ORACLE_TYPE> size_oracles;
      int size_oracle_total = 0;
      for(int i = 0; i<count_target; i++)
      {
        ORACLE_TYPE o = stamp_oracle.ExtractValue(blob);
        size_oracles.push_back(o);
        size_oracle_total += o;
      }
      if (size_oracle_total == 0) /* special case: if total size is zero (means all oracles are zero) we will spread data evenly between all items*/
      {
        for(int i = 0; i<count_target; i++)
        {
          size_oracles[i] = 1;
          size_oracle_total += 1;
        }
      }

      /* Calculating available vairable size, that will be destributed between parts according to size oracles */
      int data_size = blob.Size();
      int fixed_data_size = stamp.minSize() * count_target;
      int var_data_size = data_size - fixed_data_size;

      /* normalizing oracles so they fit total variable size, chop to parts and stamp parts */
      float remainder = 0; /* we do not want waste bytes because of rounding, so we keep the remainder, and reuse it. Thus we will use all bytes (alomost, may loose last one due to remainder=0.99999)*/
      for(ORACLE_TYPE o : size_oracles)
      {
        float el_size_f = stamp.minSize() + (float) o / size_oracle_total * var_data_size + remainder;
        int el_size = el_size_f;
        remainder = el_size_f - el_size;

        Blob blob2 = blob.ShiftBytes(el_size);
        res.push_back(blob2);
      }
    }
    else
    {
      /* Stamp is variated size */
      int fixed_size = stamp.minSize();
      int var_size = stamp.maxSize() - fixed_size;
      ORACLE_STAMP stamp_oracle;
      while(1)
      {
        if(stamp.minSize() + stamp_oracle.minSize() > blob.Size())
          break;

        ORACLE_TYPE oracle = stamp_oracle.ExtractValue(blob);

        int size = (double) oracle / ORACLE_MAX * (var_size + 1); /* +1 -- это грубая эмуляция округления вверх. oracle == ORACLE_MAX-1 == 65534 должен дать count_max*/
        if (size > var_size) size = var_size; // In case we've hit oracle == ORACLE_MAX boundary
        size += fixed_size;
        Blob blob2 = blob.ShiftBytes(size);
        res.push_back(blob2);
      }
    }
  }
  return res;
}

/**********************************************/

std::vector<Blob>
GalleySetBase::extract_internal(Blob &blob)
{
    std::vector<Blob> res;
    int fixed_total_size = 0;       // Summ of sizes of fixed parts of all stamps
    int max_varited_total_size = 0; // Summ of sizes of variable parts of variated stamps
    ORACLE_STAMP oracle_stamp;

    bool has_variated_stamps = false;
    bool has_unbounded_stamps = false;

    int unbounded_count = 0;
    int variated_count = 0;

    /* Loop throight stamps calculating total sizes and seeing what kind of stamps do we have*/
    for(StampBase & s : stamps)
    {
        fixed_total_size += s.minSize();
        if (s.isVariated())
        {
            max_varited_total_size += s.maxSize() - s.minSize();
            has_variated_stamps = true;
            variated_count++;
        }
        if (s.isUnbounded())
        {
            has_unbounded_stamps = true;
            unbounded_count++;
        }
    }
    fixed_total_size += ORACLE_SIZE * variated_count; /* We will predict stamp size for each variated stamp */

    if (unbounded_count > 1) /* One unbounded stamp will take all data. Nothing to predict */
        fixed_total_size += ORACLE_SIZE * unbounded_count;


    /* If we have both variated and unbounded stamp we will need oracle to devide availabe data between them */
    if (has_variated_stamps && has_unbounded_stamps)
        fixed_total_size += ORACLE_SIZE;

    /* We will probably not use all data that can be teoretically consumed by variated stamps.
       This is a variable that will set limits to gariated stamps greed (will be rediced later */
    int varited_total_size_limit = max_varited_total_size;

    if(fixed_total_size > blob.Size()) /* Not enought data case*/
    {
       throw OutOfData();
    }

    int avaliable_nonfixed_size = blob.Size() - fixed_total_size; /* The ammount of data available for non-fixed part of variated or unbounded stamps*/
    if (varited_total_size_limit > avaliable_nonfixed_size)
        varited_total_size_limit = avaliable_nonfixed_size; /* Can't use more than we have */


    if (has_variated_stamps && has_unbounded_stamps)
    {
        /* try do devide available data between variated and unbounded stamps */
        /* if predicted variated size is smaller than varited_total_size_limit we will decrice that limit */

        ORACLE_TYPE  oracle = oracle_stamp.ExtractValue(blob);
        int predicted_variated_limit =  round ((double) oracle / (double) ORACLE_MAX * (double) (avaliable_nonfixed_size));

        if (varited_total_size_limit > predicted_variated_limit)
            varited_total_size_limit = predicted_variated_limit;
    }

    /* now fetching oracles for non-fixed stamps and calculating total sized and modifiers*/

    double predicted_variated_total_size = 0;
    double total_unbounded_modifiers = 0;

    std::vector<double> size_modifiers;
    for(StampBase & s : stamps)
    {
        ORACLE_TYPE o_value = 0;
        double modifier = 0;
        if (!s.isFixedSize())
        {
            if (s.isUnbounded() && unbounded_count <=1 )
            {
               modifier = 1; //Nothing to predict, it will use all space
            } else
            {
              o_value = oracle_stamp.ExtractValue(blob);
              modifier = (double) o_value / (double) ORACLE_MAX;
            }
            if (s.isUnbounded())
            {
                total_unbounded_modifiers += modifier;
            } else
            {
                predicted_variated_total_size += (s.maxSize() - s.minSize()) * modifier;
            }
        }
        size_modifiers.push_back(modifier);
    }
    if (total_unbounded_modifiers == 0) // If we got all zeros, will treat them as all 1. Will work well, and give same result of equal sizes of all parts
    {
        total_unbounded_modifiers = 0;
        for(int i=0; i<stamps.size();i++)
        {
            StampBase &s = stamps[i];
            if (s.isUnbounded())
            {
                size_modifiers[i] = 1;
                total_unbounded_modifiers += 1;
            }
        }
    }
    double k_variated = 1;
    double k_unbounded = 1;

    /* predicted_variated_total_size is sum of "predicted" sized of variated_size stamp data. We may proportially shrink it, so it is fit the availabe data, if we have too few data available
       but we may not enflate it, as it can grow size of some elements beyond it's maxSize(). But here we do round, that might round it up,
       hoping that there would be no harm in it  (in rare cases (if ever) size of element's data can became one byte bigger than maxSize() and this byte will be ignored)
    */

    int final_variated_total_size;
    int final_unbounded_total_size;

    if (predicted_variated_total_size < varited_total_size_limit)
         final_variated_total_size = round(predicted_variated_total_size);  /*round up the prediction for actual use*/
    else
         final_variated_total_size = varited_total_size_limit;              /* if prediction is out of limits will use limits as boundaries*/

    if (has_variated_stamps)
       k_variated = final_variated_total_size / predicted_variated_total_size;  /* will later deflate variated stamps predicted size  with k_variated so they in sum will fit the limits */

    final_unbounded_total_size = avaliable_nonfixed_size - final_variated_total_size; /* the rest will be used by unbounded stamps*/
    if (has_unbounded_stamps)
       k_unbounded = final_unbounded_total_size / total_unbounded_modifiers;  /* calculate the proportion in which modifiers will be mapped to sizes to use all space*/

    double variated_remainder = 0;
    double unbounded_remainder = 0;

    /* Now loop across stamps calculating data size for non-fixed stams using predicted modifiers and calculated coefficients, */
    /* chopping this sizes out of the blob and saving them a result vector*/
    for(int i=0; i<stamps.size();i++)
    {
        StampBase &s = stamps[i];
        double modifier = size_modifiers[i];
        int el_size;
        if (s.isFixedSize())
        {
           el_size = s.minSize();
        }
        if (s.isVariated())
        {
          double len = (s.maxSize() - s.minSize()) * modifier * k_variated + variated_remainder;
          el_size = round(len);
          variated_remainder = len - el_size;
          el_size += s.minSize();
        }
        if (s.isUnbounded())
        {
          double len =  modifier * k_unbounded + unbounded_remainder;
          el_size = round(len);
          unbounded_remainder = len - el_size;
          el_size +=s.minSize();
        }
        Blob blob2 = blob.ShiftBytes(el_size);
        res.push_back(blob2);
    }
    return res;
}

void
GalleySetBase::LoadAll(Blob &blob)
{
    std::vector<Blob> blobs = extract_internal(blob);
    for(int i=0; i<blobs.size(); i++)
    {
        Blob blob = blobs[i];
        StampBase & stamp = stamps[i];
        stamp.Load(blob);
    }
}


std::vector<std::string>
GalleySetStr::ExtractStrSet(Blob &blob)
{
    std::vector<std::string> res;
    std::vector<Blob> blobs = extract_internal(blob);
    for(int i=0; i<blobs.size(); i++)
    {
        Blob blob = blobs[i];
        StampBaseStr & stamp = s_stamps[i];
        std::string str = stamp.ExtractStr(blob);
        res.push_back(str);
    }
    return res;
}

std::vector<std::vector<char>>
GalleySetBin::ExtractBinSet(Blob &blob)
{
    std::vector<std::vector<char>> res;
    std::vector<Blob> blobs = extract_internal(blob);
    for(int i=0; i<blobs.size(); i++)
    {
        Blob blob = blobs[i];
        StampBaseBin & stamp = b_stamps[i];
        std::vector<char> v = stamp.ExtractBin(blob);
        res.push_back(v);
    }
    return res;
}

int
GalleySetBase::minSize()
{
    bool has_variated_stamps = false;
    bool has_unbounded_stamps = false;

    int unbounded_count = 0;
    int variated_count = 0;

    int res = 0;

    /* Loop throight stamps calculating total sizes and seeing what kind of stamps do we have*/
    for(StampBase & s : stamps)
    {
        res += s.minSize();
        if (s.isVariated())
        {
            has_variated_stamps = true;
            variated_count++;
        }
        if (s.isUnbounded())
        {
            has_unbounded_stamps = true;
            unbounded_count++;
        }
    }
    if (has_variated_stamps && has_unbounded_stamps)
        res += ORACLE_SIZE;  // Need another oracle to predict how space is devided between variated and unbounded stamps

    res += ORACLE_SIZE * variated_count; /* We will predict stamp size for each variated stamp */

    if (unbounded_count > 1) /* One unbounded stamp will take all data. Nothing to predict */
        res += ORACLE_SIZE * unbounded_count;

   return res;
}

int
GalleySetBase::maxSize()
{
    int res = 0;

    /* Loop throight stamps calculating total sizes and seeing what kind of stamps do we have*/
    for(StampBase & s : stamps)
    {
        res += s.maxSize();
        if (s.isVariated())
        {
            res += ORACLE_SIZE;  // Each variated stamp needs an oracle to predict it's size. It also affects max size
        }
        if (s.isUnbounded())
        {
            return -1; // Junst one unbounded stamp makes all thing unbounded
        }
    }
   return res;
}
