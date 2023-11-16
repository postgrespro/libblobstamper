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

#ifndef STAMP_LOTTERY_H
#define STAMP_LOTTERY_H

#include<limits>
template<class StampT> class StampLottery: public StampT
{
  protected:
    std::ref_vector<StampT> stamps;
    int oracle_size;
    int init_oracle_size(std::ref_vector<StampT> stamps_arg);

    int stored_min;
    int init_stored_min(std::ref_vector<StampT> stamps_arg);
    int stored_max;
    int init_stored_max(std::ref_vector<StampT> stamps_arg);

  public:
    StampLottery(std::ref_vector<StampT> stamps_arg): stamps(stamps_arg),
                                                      oracle_size(init_oracle_size(stamps_arg)),
                                                      stored_min(init_stored_min(stamps_arg)),
                                                      stored_max(init_stored_max(stamps_arg)) {};
    StampLottery(): stored_min(-1), stored_max(-2), oracle_size(1)  {};

    virtual int  minSize() override;
    virtual int  maxSize() override;

    virtual bool soft_maxsize_filter(StampT &stamp, int data_size) {return true;}; /* Allow to skip stamps that would leave to much data unused. But not active here*/

    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    void Append(StampT & stamp);
};


template<class StampT> class StampLottery4Recursion: public StampLottery<StampT>
{
  public:
    StampLottery4Recursion(std::ref_vector<StampT> stamps_arg): StampLottery<StampT>(stamps_arg) {};
    StampLottery4Recursion(): StampLottery<StampT>()  {};
    virtual bool soft_maxsize_filter(StampT &stamp, int data_size) override;
};


template<class StampT> bool
StampLottery4Recursion<StampT>::
soft_maxsize_filter(StampT &stamp, int data_size)
{
  if ( stamp.isUnbounded() ||            // Unbounded is always ok
       stamp.maxSize() > data_size  ||  // Variated that can consume all data is ok
       stamp.minSize() + stamp.maxSize() > data_size  // Fixed or variated stamp that lefts less data then it's min size will also do
      )
      return true;
  return false;
}

template<class StampT> int
StampLottery<StampT>::
init_stored_min(std::ref_vector<StampT> stamps_arg)
{
  int min = std::numeric_limits<int>::max();

  for(StampT & stamp : stamps)
  {

    if (min > stamp.minSize())
        min = stamp.minSize();
  }
  return min;
}

template<class StampT> int
StampLottery<StampT>::
init_stored_max(std::ref_vector<StampT> stamps_arg)
{
  int max = 0;

  for(StampT & stamp : stamps)
  {
    if (stamp.maxSize() == -1)
      return -1;

    if (max < stamp.maxSize())
        max = stamp.maxSize();
  }
  return max;
}


template<class StampT> int
StampLottery<StampT>::init_oracle_size(std::ref_vector<StampT> stamps_arg)
{
  unsigned long size = stamps_arg.size();
  if (size < std::numeric_limits<unsigned char>::max())
    return 1;
  if (size < std::numeric_limits<unsigned short int>::max())
    return 2;
  if (size < std::numeric_limits<unsigned int>::max())
    return 4;
  return 8;
}



/* StampLottery is used for recustion. Lottery contains trams that uses this very lottery
 Calculating sizes on fly leads to infinite recrsion. So we calculate sizes when lottery 
item is added, and use stored value, when it is needed */

template<class StampT> int
StampLottery<StampT>::minSize()
{
  return stored_min + oracle_size;
}

template<class StampT> int
StampLottery<StampT>::maxSize()
{
  if (stored_max == -1)
    return -1;
  return stored_max + oracle_size;
}


template<class StampT> std::string
StampLottery<StampT>::ExtractStr(std::shared_ptr<Blob> blob)
{
  unsigned long oracle;
  unsigned long oracle_max;

  switch (oracle_size)
  {
     case 1:
     {
        StampArithm<unsigned char> stamp;
        oracle = stamp.ExtractValue(blob);
        oracle_max = std::numeric_limits<unsigned char>::max();
        break;
     }
     case 2:
     {
        StampArithm<unsigned short> stamp;
        oracle = stamp.ExtractValue(blob);
        oracle_max = std::numeric_limits<unsigned short>::max();
        break;
     }
     case 4:
     {
        StampArithm<unsigned int> stamp;
        oracle = stamp.ExtractValue(blob);
        oracle_max = std::numeric_limits<unsigned int>::max();
        break;
     }
     case 8:
     {
        StampArithm<unsigned long> stamp;
        oracle = stamp.ExtractValue(blob);
        oracle_max = std::numeric_limits<unsigned long>::max();
        break;
     }
     default:
        abort(); // Should never get here
 }

  /* Actually we use only stamps that short enogh to consume blob's available data*/
  std::ref_vector<StampT> actual_stamps;
  for(StampT & stamp : stamps)
  {
    if(blob->Size() < stamp.minSize())  // Skip all stamps that dose not fit
      continue;
    if (soft_maxsize_filter(stamp, blob->Size()))
      actual_stamps.push_back(stamp);
  }
  if (actual_stamps.empty())
  {
    // Add just everything that fits
    for(StampT & stamp : stamps)
    {
      if(blob->Size() < stamp.minSize())  // Skip all stamps that dose not fit
         continue;
      actual_stamps.push_back(stamp);
    }
  }

  if (actual_stamps.empty())
        throw OutOfData();  // This should never happen

  long long index = ((double) oracle) / oracle_max * actual_stamps.size();
  if ( index == actual_stamps.size())  index--; /* If we hit the boundary step inside a bit*/

  StampT& stamp = actual_stamps[index];
  return stamp.ExtractStr(blob);
}


template<class StampT> void
StampLottery<StampT>::Append(StampT & stamp)
{
  if (stamp.maxSize() == -1)
  {
    stored_max = -1;
  } else
  {
    if (stamp.maxSize() > stored_max)  /* this case includes case when stored_max have not beed initialized (==-2)*/
      stored_max = stamp.maxSize();
  }

  if (stored_min == -1) /* stored_min have not been initializes*/
    stored_min = stamp.minSize();

  if (stamp.minSize() < stored_min)
    stored_min = stamp.minSize();

  stamps.push_back(stamp);
  oracle_size = init_oracle_size(stamps);
}

#endif  // STAMP_LOTTERY_H
