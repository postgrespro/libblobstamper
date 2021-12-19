# LibBlobStamper

Tool for building structured data out of random input.

When you do fuzzing testing of a project that is complex enoght to have
syntax parser, and core functionaluty behind it, your fuzzer will probably
spend a lot of cpu working with syntax parser. Fuzzing syntax parser is a
good thing, but it does not really help to fuzz core functionality. One of the
general approaches here may be is to generate such imput data that is syntaxycally
corrent from the perspectice of syntax parser, and meanwhile totally random form 
the perspective of core functionality.

LibBlobStamper is a tool for building convertors form random binary data
into random data with required syntax.

LibBlomStamper is written in C++, can be used for genertaing both text data
with desired sintax, and structured binary data (e.g. C-structures). You can
build this convertor right into you binary test-unit, so this converstion will be
opaque both for fuzzer, or DSE-tools.

## Synopsys

```c++
  Blob blob(data, data_length);
  StampArithm<short int> stamp;  // Stamp that work with arithmetic data (integers, and floats)

  std::string s = stamp.ExtractStr(blob);          // Get integrer formatted as string
  std::vector<char> vec= stamp.ExtractBin(blob);   // Get binary data represeted as byte array
  short int i = stamp.ExtractValue(blob)           // Get data represented as a value, that can be used in the code
```

```c++
typedef struct // Some complex structure, e.g. complex number
{
  float re,im;
} Complex;

class StampComplex: public StampBaseStr, public StampBaseV<Complex> // Stamp will have String and Value result representation
{
  private:
    StampArithm<float> stamp; // Substamp that is used while building result
  public:
    int maxSize() override {return stamp.maxSize()*2;}; // Should set minimum and maximum ammount of data that can be consumed by stamp
    int minSize() override {return stamp.minSize()*2;};
    virtual Complex ExtractValue(Blob &blob) override;  // Method that constructs "value" representation
    virtual std::string ExtractStr(Blob &blob) override; // Method that constructs "string" representation
};

Complex StampComplex::ExtractValue(Blob &blob)
{
  Complex c;
  c.re = stamp.ExtractValue(blob); // Just fill the structure with float values and return it
  c.im = stamp.ExtractValue(blob);
  return c;
}

std::string StampComplex::ExtractStr(Blob &blob)
{
  std::string s_re = stamp.ExtractStr(blob);  // Get numbers as a string
  std::string s_im = stamp.ExtractStr(blob);
  return s_re + " + i" + s_im;                // Make result string out of them
}

// ......................... Somwhere in main() function ....................................

  Blob blob(data, data_len);

  StampComplex stamp;
  std::string s = stamp.ExtractStr(blob);
  Complex c = stamp.ExtractValue(blob);
```

## Basic Definitions

### Blob

Blob is a chunk of binary data that came from fuzzer, or other source, it is
considered to be random. Stamps consumes data from the Blob, for costructing
values and for "divination": if Stamp to make some desidion, you get some data from
the Blob and iterpret it as a "prediction" telling it what to do. E.g. tell the stamp in what
propotion remaining data should be split for two subordinate Stamps, that can consume any
ammount of data. Since blob data is considered to be random, you will cover all possible
proportions in a long run. Blob implemented as C++ onject.

### Stamp

Stamp is a C++ object that knows how to convert some chunk of data into certain structured
representation.

Basic properties of Stamp from the perspective of data consumation:

* Stamp should know how much data it can consume to produce structured result. Minimanl
ammount ammount and maximum ammount. These values should be available via minSize() and maxSize()
methods.

* Stamp is greedy, it consimes as much data as it can. If your blob is long enought, the Stamp will
byte maxSize() bytes from it. If you want to limit Stamp's appetite, you should wrap it in a Galley
somehow.

All stamps iherits StampBase class.








