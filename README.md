# LibBlobStamper

Tool for building structured data out of random input.

When you do fuzzing testing of a project that is complex enough to have
syntax parser, and core functionality behind it, your fuzzer will probably
spend a lot of cpu working with syntax parser. Fuzzing syntax parser is a
good thing, but it does not really help to fuzz core functionality. One of the
general approaches here may be is to generate such input data that is syntactically
correct from the perspective of syntax parser, and meanwhile totally random form 
the perspective of core functionality.

LibBlobStamper is a tool for building convertors form random binary data
into random data with required syntax.

LibBlomStamper is written in C++, can be used for generating both text data
with desired syntax, and structured binary data (e.g. C-structures). You can
build this convertor right into you binary test-unit, so this conversion will be
opaque both for fuzzer, or DSE-tools.


## Overview

### Blob

Blob is a chunk of binary data that presumably came fuzzer, it is considered to be random.
Blob data is used by Stamps for constructing structured values (syntactically correct strings or C-structures).
When Stamp uses chunk of Blob's data, this data is removed from the Blob.
You can use stamp on a Blob as many times as you like until you are out of Blob data.

### Stamps

Stamp is a C++ object that "bites" chunk of binary data from Blob and converts it
into certain structured representation (text string with syntax that is provided
by stamp or C-structure)

```
  char data[] ="abcdefghijk";
  Blob blob(data, strlen(data));  // Blob with "random" data

  StampArithm<short int> stamp;  // Stamp for getting short integer (both string and value representations)

  std::string s;
  short int i;

  s = stamp.ExtractStr(blob);    // bite short int data from blob and save it to a string. Will get "25185"
  i = stamp.ExtractValue(blob);  // bite short int data from blob and save it to short int vriable. Will get 25699
```

As you can see Stamps can extract values of various type.
Each extracted type is provided with proper extract method:

* `ExtractStr` returns `std::string` value.
That string will be formatted according to the syntax that is implemented in this extract method

* `ExtractValue` returns "value" of C-structure or of another C-variable.
In the example above it is value of `short int`-variable.

* `ExtractPValue` same as `ExtractValue`, but returns pointer to the value.
Or, more precisely, `sized_ptr<T>` pointer (see below (FIXME not written yet))

* `ExtractBin` same as `ExtractPValue` but returns extracted structure as array of characters (`std::vector<char>`).
You will can work with it as binary buffer or cast it to desired type manually.

Stamp must have at least one of the extract method implemented.
In your own stamp you will probably implement only extract methods you need, either string or binary.
`StampArithm<T>` has all of them, but this seems to be an exceptional case.

#### Stamp Sizes

Amount of data that can be consumed by Stamp is called Stamp Size.
Depending on Min Stamp Size and Max Stamp Size, Stamps cam be divided into three groups:

* *Fixed Size Stamps*: Stamp consumes fixed amount of data (Min Stamp Size == Max Stamp Size). 
For example `StampArithm<T>` stamp always consumes `sizeof(T)` bytes.

* *Variated Size Stamp*: Min Stamp Size != Max Stamp Size.
For example stamp that generates string with random Latin letters 3 to 16 character long.
It consumes 3..16 bytes and "normalizes" them to Latin character bytes.

* *Unbounded Size Stamp*: Stamp that has Min Size, but will consume any amount of data if provided. 

Min and Max Stamp Sizes are available via `minSize()` and `maxSize()` methods.
For Unbound Size Stamps `maxSize()` is set to `-1`.

Also please note, that stamps are greedy, they will try to consume all data they can.
E.g. Unbounded Size Stamp will consume all data from the Blob.
Variated Size Stamp will try to eat `MaxSize()` bytes, but will be satisfied with anything grater or equal to `minSize()`.

To limit Stamps appetite you should use Galleys.

### Galleys

Galley is a way to squeeze several Stamps into one object.
You can think about LibBlobStamper's Galley as about letterpress galley: you have several stamps, you put them into a galley, and now you have one bigger stamp.
You would definitely need Galley if you want to split Blob data between several Unbounded Stamps.
Each Stamp tries to use all data, and Galley is the way to divide available data between Stamps.
For Variated Stamps story is the same: they must not always get all data they want.

There are two types of Galleys in LibBlobStamper now: GalleyVector and GalleySet.

#### Galley Vector

Galley Vector is used to slice all Blob data into parts using one selected stamp.
For Fixed Size Stamp, blob will be chopped to parts that fits the Stamp, and all these parts will be fed to the Stamp.
For Variated and Unbounded Stamps Galley will use tricky algorithm to decide how to split the Blob data (the algorithm will be discussed later) and then will apply target stamp to each data chunk.
Galley will return `std::vector<std::string>` or `std::vercor<T>`, depending on what extract type you are going to use.

Example:

```
  char data[] ="abcdefghijk";
  Blob blob1(data, strlen(data));  // Blob with "random" data
  Blob blob2(data, strlen(data));  // Another Blob with same data

  StampArithm<short int> stamp;  // Stamp for short integer data (both string and value)

  GalleyVectorStr galley_s(stamp);
  GalleyVectorV<short int> galley_v(stamp);

  std::vector<std::string> res_s = galley_s.ExtractStrVector(blob1);
  std::vector<short int>   res_v = galley_v.ExtractValuesVector(blob2);
```

#### Galley Set

Galley Set allows simultaneously apply stamps of different types.
Same as Galley Vector it uses tricky algorithm to divide Blob Data between stamps, but in this case these are different Stamps.

For now Galley Set works with String and Binary extracted types.
It is not quite clear how to implement Galley with Values extracted type using C++ facilities.

Example:

```
  char data[] ="abcdefghijk";
  Blob blob(data, strlen(data));  // Blob with "random" data

  StampArithm<short int> stamp_i;  // Stamp for short integer data (both string and value)
  StampArithm<float> stamp_f;      // Stamp for float numeric data (both string and value)

  GalleySetStr galley({stamp_i, stamp_f});

  std::vector<std::string> res = galley.ExtractStrSet(blob);
```

#### Creating Stamps from Galleys

Galleys and Stamps inherit same base class, so you can make Stamp from a Galley by implementing appropriate Extract method.
This will be explained below in "Creating Stamps" section.

### Recursion

LibBlobStamper have been designed keeping in mind that it should be able to create strings with nested syntax (e.g. arithmetic expressions).
This work is still in progress, it is quite raw to be documented properly, but you can explore `examples/exampleZZ.cpp` to see current status of Stamp recursion.


## Creating your own Stamps

General idea: you should inherit from base class that provides Extract method we need (e.g. inherit from `StampBaseStr` to get `ExtractStr()`). 
Implement `minSize()` and `maxSize()` methods, and Extract method you've chosen.

Normally you will seldom need to work with raw blob data.
Most probably you will combine existing basic stamps to create complex one.

### Creating String Stamp

Let's imagine you need to generate stamp for complex numbers `a + ib` (e.g. `12+ 3i`). 
Let's imagine that `a` and `b` are not really big integers.
To build this stamp we will use two arithmetic stamps that will give us text representation of `short int`, and we will combine them the way we want.

Class definition will look like this.
We define stamps we will use while building string right inside the class.

```
class ComplexIntStamp: public StampBaseStr
{
  protected:
    StampArithm<short int> stampA, stampB;
  public:
    virtual int minSize() override;
    virtual int maxSize() override;
    virtual std::string ExtractStr(Blob &blob) override;
};
```
Actually here we can have one `StampArithm<short int>` stamp, and apply it two times.  
But for making an example more clear, we will explicitly declare both stamps.

As we are going to apply each stamp only once, we can calculate min and max
sizer of our new stamp as sum of min and max sizes of stamps we have used:

```
int ComplexIntStamp::minSize()
{
  return stampA.minSize() + stampB.minSize();
}

int ComplexIntStamp::maxSize()
{
  return stampA.maxSize() + stampB.maxSize();
}
```

Now we will implement Extract Method.
We just extract two values with `stampA` and `stampB` and combine them into string we want.

```
std::string ComplexIntStamp::ExtractStr(Blob &blob)
{
  std::string A, B;
  A = stampA.ExtractStr(blob);
  B = stampB.ExtractStr(blob);
  return A + " + " + B + "i";
}
```

Now you can use your stamp the way any stamp is used:

```
int main()
{
  char data[] = "abcdef";
  Blob blob(data, strlen(data));
  ComplexIntStamp stamp;

  std::string s = stamp.ExtractStr(blob);

  std::cout << "String value: '" << s <<"'\n";
}
```
As you can see, creating a new stamp is quite simple thing.

### Creating Value Stamp

Let's imagine there is a C-structure we want to fill with random data from fuzzer:

```
typedef struct {
  short int re;
  short int im;
} complex_short;
```

Same story with complex short int, but this time we want it represented as C-structure.

To create the Stamp to produce this structure from Blob Data we should inherit
from `StampBaseV<complex_short>`

```
class ComplexIntStamp: public StampBaseV<complex_short>
{
  protected:
    StampArithm<short int> stampA, stampB;
  public:
    virtual int minSize() override;
    virtual int maxSize() override;
    virtual complex_short ExtractValue(Blob &blob) override;
};
```

`minSize()` and `maxSize()` methods here would be the same as in String Stamp above, because here we extract same two `short int` values. 
We process them differently, but values are same.

```
int ComplexIntStamp::minSize()
{
  return stampA.minSize() + stampB.minSize();
}

int ComplexIntStamp::maxSize()
{
  return stampA.maxSize() + stampB.maxSize();
}
```

In `ExtractValue` method we locally create desired structure, fill it with
values fetched from the Blob, and return the structure by value.

```
complex_short ComplexIntStamp::ExtractValue(Blob &blob)
{
  complex_short res;
  res.re = stampA.ExtractValue(blob);
  res.im = stampB.ExtractValue(blob);
  return res;
}
```

Then we can use stamp for extracting `complex_short` directly from the Blob:

```
int main()
{
  char data[] = "abcdef";
  Blob blob(data, strlen(data));
  ComplexIntStamp stamp;

  complex_short cs = stamp.ExtractValue(blob);

  std::cout << " re=" << cs.re <<" im="<< cs.im <<"'\n";
}
```

#### Creating Stamp from Galley

As it was mentioned before, Galley inherits from same base class as Stamp do.
So you may add Extract method to a Galley, and use this new object as a Stamp.
To add extract method to Galley you should use multiple inheritance.

```
class ArrayOfComplexIntStamp:  public GalleyVectorStr, public StampBaseStr
{
  protected:
    ComplexIntStamp * item_stamp_p;
  public:
    ArrayOfComplexIntStamp(): GalleyVectorStr(*(item_stamp_p = new ComplexIntStamp())) {};
    ~ArrayOfComplexIntStamp() {delete item_stamp_p;};

    virtual std::string ExtractStr(Blob &blob) override;
};
```
Because of order initialization issue, we have to initialize the stamp inside the call of parent class constructor via `new` method, and then destroy in in the destructor.

Here do not need to implement `maxSize()` and `maxSize()`, as Galley properly implements them for us.
So we need only to implement Extract method we need.


```
std::string ArrayOfComplexIntStamp::ExtractStr(Blob &blob)
{
  std::vector<std::string> data = ExtractStrVector(blob);
  std::string res = "";

  for(std::string s : data)
  {
    if (!res.empty())
    {
      res+=", ";
    }
    res+= s;
  }
  res = "[" + res + "]";
  return res;
}
```

## Understanding LibBlobStamper internals
### How does Galley Vector works for non Unbounded Size Stamps

Lets imagine we have an Unbounded Size Stamp, it produce some Item.
Here we do not care if it is C-structure, or string, we just know that this Item is used by the function we want to fuzz.
Even more, function we want to fuzz accepts an array of these Items, so we want to provide one or more Item to the target function.
Number of Items is not predefined, so it should be random, to test all possible cases.
But it should not be really random, because we need reproducible tests, so we would use Blob Data as a source or randomness.
E.g. take first byte of the Blob Data, and it would tell us how many Items we would have, and use the rest of Blob Data for Items' content.
In reality it is not that simple, but this example shows the idea well.

So here we come to the concept of Oracle.
Each time we need to make a decision "how many...", "how log", "in what proportion" etc, we take a piece of Blob and make decision using that value.
This peace of Blob is called Oracle.
In Galleys `unsigned short int` value is used for Oracle.

General idea of applying the Oracle is following.
We get Oracle Value from the Blob, and use Max Oracle Value calculate Target Ratio. 
Knowing possible Max and Min values for Target value, we use Target Ratio to calculate desired Target Value. 

First step: for Unbounded Sized Stamps we should determinate into how may chunks we going to split the Blob.
We should have at least one chunk, so minimum value here is 1.
Maximum number of chunks we will get, if we split Blob into chunks sized as `minSize()` of the stamp.
So maximum number of chunks is Blob Size / Stamp's minSize().
Using Oracle, we find target ratio, and using target ratio find desired Chunks Count, somewhere between 1 and Max Chunks Count.

As a second step we should determinate size of each chunk.
We reserve `minSize()` for each chunk (fixed part of chunk), and then spread the rest data among the chunk, so each chunk will get it's own variable part.
To do that, we get an Oracle for each chunk, calculate total sum of these Oracles.
Then for each chunk we calculate a ratio, as a ratio of chunk's oracle to total sum of all Oracles. 
After it we use chunks ratio to spread Blob data that are available for variable parts among chunks.

### How does Galley Vector works for non Variated Size Stamps

With Unbounded Size Stamps we had problem that one single stamp will try to consume all available data data.
With Variated Size Stamp we do not have such problem, stamp's size is limited, so we can chop Blob data for the stamps chunk by chunk, until we are out of data.
But we should keep in mind that Variated Size Stamp is also greedy, and will try to consume `maxSize()` bytes if they are available. 
And for fuzzing purposes we need results for all kind of stamp sizes.
That is why before chopping each chunk, we will get Oracle, that will predict us how much data (from `minSize()` till `maxSize()`) will be chopped this time.
This will allow us to have vector of results produced from chunks of various sizes.

### How does Galley Set works

For Galley Set, story is almost the same as for Galley Vector, with one correction: in Galley Set we have all types of Stamps (Fixed, Variated, Unbounded) mixed.
So First we will reserve fixed part of the chunk for fixed part of each stamp.
The rest data we should divide between Variated and Unbounded Stamps.
To divide it, we getting Oracle, and conclude in what ratio remaining data would be divided among Variated and Unbounded Stamp.
If Variated Stamps get too much data, extra data is given to Unbounded stamps as a "gift".
After that data is shared among each group of Stamps in similar way they were
shared in Galley Vector.

## Tricks and tips
### Galley Vector to Array C++ trick

If you have Stamp with Value Extract Method, and you are going to use `GalleyVectorV<T>` to extract array of values, you should know, that `ExtractValuesVector` method of a Galley will return you values as `std::vector<T>`.
`std::vector<T>` has actual array inside, and you can just `memcpy` it, without iterating over it.
It would be something like that:

```
ComplexIntStamp stamp;
GalleyVectorV<complex_short> galley(stamp);
complex_short *result;
int result_size;
std::vector<complex_short> vec = galley.ExtractValuesVector(blob);
result_len = vec.size();

result = (complex_short *) malloc(sizeof(complex_short) * result_len));
memcpy((void*) result, (void*) &vec[0], sizeof(complex_short) * result_len);
```

## Further reading

1. Read examples. They are in `examples` directory.
You can start study LibBlobStamper with playing with them.

2. Read the code.
Code is quite clear (I hope), except for the code of Galley, that are a bit intricated.

3. Read tests.
Tests are located in `t` directory.
Much effort were put into covering all LibBlobStamper functionality with tests.
So if some functionality is not covered with this README, or with standalone examples,
you can find examples of usage in the tests.

## Authors

LibBlobStamper were created by Nikolay Shaplov from Postgres Porfessional at
2021-2022.

You can contact me via e-mail: `n.shaplov@postgrespro.ru` or via matrix: `@n.shaplov:postgrespro.ru`.
E-mail lists and chat rooms will be created when they are needed.

You can also do Pull Requests and File Bugs at GitHub: https://github.com/postgrespro/libblobstamper


