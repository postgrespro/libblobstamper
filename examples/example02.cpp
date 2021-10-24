#include<stdio.h>
#include<string.h>

#include<string>
#include<iostream>

#include<blobstamper/blobstamper.h>

int main()
{
  char data[] = "abcdefjhi";
  Blob blob(data, strlen(data));
  StampArithm<short int> stamp;

  std::string s = stamp.ExtractStr(blob);
  std::vector<char> vec= stamp.ExtractBin(blob);
  short int i = stamp.ExtractValue(blob);

  sized_ptr<short int> spi = stamp.ExtractPValue(blob);
  short int *pi = spi;

  std::cout << "String value: '" << s <<"'\n";
  std::cout << "Bin value: '" << vec[0] <<"', '" << vec[1] <<"'\n";
  std::cout << "Value: " << i  <<"\n";
  std::cout << "PValue: " << *pi  <<"\n";
}