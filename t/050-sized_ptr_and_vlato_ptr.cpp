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

#include <exception>
#include <string>
#include <cstdlib>
#define WANT_TEST_EXTRAS
#include <tap++/tap++.h>

#include"blobstamper/helpers.h"

using namespace TAP;


typedef struct
{
  float f;
  long l;
  char c;
} FixedLengthObject;

typedef struct
{
  float f;
  long l;
  char c;
  short int arr[];
} VariableLengthObject;



int main() {
    TEST_START(9);

    // #tests for sized_ptr
    { /* 1..3 */
      long *l;
      size_t size = sizeof(long) * 5;
      l = (long*) malloc(size);
      for(int i=0; i<5; i++)
        l[i]=i*100;
      sized_ptr<long> sp(l, size);

      long * l2 = sp;
      is(l2[4], 400, "Can use memory from sized poiner");
      ok(l2 == l, "It is actually same pointer");
      is(sp.size(),size, "Size is stored as expected");
//      free(l); /*Do not need to free it. sized_ptr will do it for you*/
    }
    { /* 4..10 */
       VLATO_ptr<VariableLengthObject, short int> vp(offsetof(VariableLengthObject,arr), 5);
       is(vp.length(), 5, "Number of elements is ok");
       is(vp.size(), offsetof(VariableLengthObject,arr) + sizeof(short int) *5, "Size of object is ok");
       VariableLengthObject *p1 = vp;
       p1->arr[4]=100;

       sized_ptr<VariableLengthObject> sp = vp; // from now on vp does not exit
       VariableLengthObject *p2 = vp;
       ok(!p2, "now vp should not point anywere");
       is(sp.size(), offsetof(VariableLengthObject,arr) + sizeof(short int) *5, "Size of sized object is ok");

       VariableLengthObject *p3 = sp;
       ok(p1 == p3, "Still same object");
       is(p3->arr[4], 100, "stored data is in it's place");
    }
    TEST_END;
}
