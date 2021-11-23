template<class StampT> class StampLottery: public StampT
{
  protected:
    std::ref_vector<StampT> stamps;
    int oracle_size;
    int init_oracle_size(std::ref_vector<StampT> stamps_arg);

    int stored_min;
    int init_stored_min(std::ref_vector<StampT> stamps_arg);

  public:
    StampLottery(std::ref_vector<StampT> stamps_arg): stamps(stamps_arg), oracle_size(init_oracle_size(stamps_arg)), stored_min(init_stored_min(stamps_arg)) {};
    StampLottery(): stored_min(-1) {};

    virtual int  minSize() override;
    virtual int  maxSize() override;
    virtual std::string ExtractStr(Blob &blob) override;
    void Append(StampT & stamp);
};


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


template<class StampT> int
StampLottery<StampT>::minSize()
{
  return stored_min + oracle_size;
}

template<class StampT> int
StampLottery<StampT>::maxSize()
{
  return -1;  // FIXME this is true only for recurion case. Should fix it somehow if Lottery is used in other cases
}


template<class StampT> std::string
StampLottery<StampT>::ExtractStr(Blob &blob)
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
    if(blob.Size() < stamp.minSize())  // Skip all stamps that dose not fit
       continue;
    if ( stamp.isUnbounded() ||            // Unbounded is always ok
         stamp.maxSize() > blob.Size()  ||  // Variated that can consume all data is ok
         stamp.minSize() * 2 > blob.Size()  // Fixed or variated stamp that lefts less data then it's min size will also do
       )
    {
      actual_stamps.push_back(stamp);
    }
  }
  if (actual_stamps.empty())
  {
    // Add just everything that fits
    for(StampT & stamp : stamps)
    {
      if(blob.Size() < stamp.minSize())  // Skip all stamps that dose not fit
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
  if (stamp.minSize()<stored_min)
  {
    stored_min = stamp.minSize();
  }
  stamps.push_back(stamp);
  oracle_size = init_oracle_size(stamps);
}
