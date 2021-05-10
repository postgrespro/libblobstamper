


#include "blob.h"
#include "stamp.h"
#include "stamp_atomic.h"
#include "galley.h"
#include <vector>


std::list<std::string>
GalleySeries::Extract(Blob &blob)
{
  std::list<std::string> res;

  if (stamp.isFixedSize())
  {
    while (1)
    {
      std::string el = blob.ShiftSingleStampStr(stamp);
      if (el.empty())
        break;
      res.push_back(el);
    }
  }
  else
  {
    if (stamp.maxSize() == -1) // if unlimited size
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
        std::string str = blob2.ShiftSingleStampStr(stamp);
        res.push_back(str);
      }
      free(count_oracle);
    }
    else
    {
      printf("Not implemented yet!");
      exit(1);
    }
  }
  return res;
}


int
GalleySeries::minSize()
{
  if (stamp.isFixedSize())
  {
    return stamp.minSize(); // When size is fixed, series can have only one member with no extra data used
  }
  else
  {
    if (stamp.maxSize() == -1) // if unlimited size
    {
      return stamp.minSize() + ORACLE_SIZE * 2; // One -- count oracle, one -- size oracle
    }
    else
    {
      printf("Not implemented yet!");
      exit(1);
    }
  }
}
