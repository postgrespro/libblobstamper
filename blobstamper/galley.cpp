
#include <vector>
#include <functional>
#include <cmath>

#include "blob.h"
#include "stamp.h"
#include "stamp_atomic.h"
#include "galley.h"


int
GalleySeries::minSize()
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



std::list<std::string>
GalleySeries::ExtractStr(Blob &blob)
{
  std::list<std::string> res;
  std::list<Blob> blobs = extract_internal(blob);
  for(Blob blob : blobs)
  {
    std::string str= blob.ShiftSingleStampStr(stamp);
    res.push_back(str);
  }
  return res;
}

std::list<void *>
GalleySeries::ExtractBin(Blob &blob)
{
  std::list<void *> res;
  std::list<Blob> blobs = extract_internal(blob);
  for(Blob blob : blobs)
  {
    void * data= blob.ShiftSingleStampBin(stamp);
    res.push_back(data);
  }
  return res;
}


std::list<Blob>
GalleySeries::extract_internal(Blob &blob)
{
  std::list<Blob> res;

  if (stamp.isFixedSize())
  {
    int size = stamp.minSize();
    while (1)
    {
      Blob el = blob.ShiftBytes(size);
      if (el.isEmpty())
        break;
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
      ORACLE_TYPE *count_oracle;
      count_oracle = (ORACLE_TYPE *) blob.ShiftSingleStampBin(stamp_oracle);

      ORACLE_TYPE count_target = count_max * (*count_oracle) / ORACLE_MAX + 1; /* +1 -- это грубая эмуляция округления вверх. oracle == ORACLE_MAX-1 == 65534 должен дать count_max*/
      if (count_target > count_max) count_target = count_max; // В случае если oracle оказался рваен ORACLE_MAX

      /* Getting size oracles for each part */
      std::vector<ORACLE_TYPE> size_oracles;
      int size_oracle_total = 0;
      for(int i = 0; i<count_target; i++)
      {
        ORACLE_TYPE *o = (ORACLE_TYPE *) blob.ShiftSingleStampBin(stamp_oracle);
        size_oracles.push_back(*o);
        size_oracle_total += *o;
        free(o);
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
      free(count_oracle);
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
        ORACLE_TYPE *oracle = (ORACLE_TYPE *) blob.ShiftSingleStampBin(stamp_oracle);
        int size = (double) *oracle / ORACLE_MAX * (var_size + 1); /* +1 -- это грубая эмуляция округления вверх. oracle == ORACLE_MAX-1 == 65534 должен дать count_max*/
        if (size > var_size) size = var_size; // In case we've hit oracle == ORACLE_MAX boundary
        size += fixed_size;
        Blob blob2 = blob.ShiftBytes(size);
        res.push_back(blob2);
        free(oracle);
      }
    }
  }
  return res;
}

/*
class GalleyVector : public GalleyBase
{
  protected:
    std::vector<std::reference_wrapper<StampBase>> stamps;
  public:
    GalleyVector(std::vector<std::reference_wrapper<StampBase>> arg) : stamps(arg) {};
    std::vector<Blob> extract_internal(Blob &blob);
    std::vector<std::string> ExtractStr(Blob &blob);
//    std::list<void *> ExtractBin(Blob &blob);

    int minSize() override {return -2;}; // FIXME
    int maxSize() override {return -3;}; //FIXME /* Sereies always takes as much data as it can take */
/*};*/

std::vector<Blob>
GalleyVector::extract_internal(Blob &blob)
{
    std::vector<Blob> res;
    int fixed_total_size = 0;       // Summ of sizes of fixed parts of all stamps
    int max_varited_total_size = 0; // Summ of sizes of variable parts of variated stamps
    ORACLE_STAMP oracle_stamp;

    bool has_variated_stamps = false;
    bool has_unbounded_stamps = false;

    /* Loop throight stamps calculating total sizes and seeing what kind of stamps do we have*/
    for(StampBase & s : stamps)
    {
        fixed_total_size += s.minSize();
        if (s.isVariated())
        {
            max_varited_total_size += s.maxSize() - s.minSize();
            has_variated_stamps = true;
            fixed_total_size += ORACLE_SIZE;
        }
        if (s.isUnbounded())
        {
            has_unbounded_stamps = true;
            fixed_total_size += ORACLE_SIZE;
        }
    }

    /* If we have both variated and unbounded stamp we will need oracle to devide availabe data between them */
    if (has_variated_stamps && has_unbounded_stamps)
        fixed_total_size += ORACLE_SIZE;

    /* We will probably not use all data that can be teoretically consumed by variated stamps.
       This is a variable that will set limits to gariated stamps greed (will be rediced later */
    int varited_total_size_limit = max_varited_total_size;

    int avaliable_nonfixed_size = blob.Size() - fixed_total_size; /* The ammount of data available for non-fixed part of variated or unbounded stamps*/
    if (varited_total_size_limit > avaliable_nonfixed_size)
        varited_total_size_limit = avaliable_nonfixed_size; /* Can't use more than we have */


    if (has_variated_stamps && has_unbounded_stamps)
    {
        /* try do devide available data between variated and unbounded stamps */
        /* if predicted variated size is smaller than varited_total_size_limit we will decrice that limit */

        ORACLE_TYPE * oracle = (ORACLE_TYPE *) blob.ShiftSingleStampBin(oracle_stamp);
        int predicted_variated_limit =  round ((double) *oracle / (double) ORACLE_MAX * (double) (avaliable_nonfixed_size));
        free(oracle);

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
            ORACLE_TYPE * oracle = (ORACLE_TYPE *) blob.ShiftSingleStampBin(oracle_stamp);
            o_value = * oracle;
            free(oracle);
            modifier = (double) o_value / (double) ORACLE_MAX;
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

std::vector<std::string>
GalleyVector::ExtractStr(Blob &blob)
{
    std::vector<std::string> res;
    std::vector<Blob> blobs = extract_internal(blob);
    for(int i=0; i<stamps.size(); i++)
    {
        Blob blob = blobs[i];
        StampBase & stamp = stamps[i];
      std::string str= blob.ShiftSingleStampStr(stamp);
      res.push_back(str);
    }
    return res;
}

int
GalleyVector::minSize()
{
    bool has_variated_stamps = false;
    bool has_unbounded_stamps = false;

    int res = 0;

    /* Loop throight stamps calculating total sizes and seeing what kind of stamps do we have*/
    for(StampBase & s : stamps)
    {
        res += s.minSize();
        if (s.isVariated())
        {
            has_variated_stamps = true;
            res += ORACLE_SIZE;  //Each variated stamp needs an oracle to predict it's size
        }
        if (s.isUnbounded())
        {
            has_unbounded_stamps = true;
            res += ORACLE_SIZE;  //Each unbounded stamp needs an oracle to predict it's size
        }
    }
    if (has_variated_stamps && has_unbounded_stamps)
        res += ORACLE_SIZE;  // Need another oracle to predict how space is devided between variated and unbounded stamps

   return res;
}

int
GalleyVector::maxSize()
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


