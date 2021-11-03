#include<stdio.h>
#include<string.h>

#include<string>
#include<iostream>

#include<blobstamper/blobstamper.h>

int main()
{
  char data[] = "abcdefjhi";
  Blob blob(data, strlen(data));
  StampArithm<short int> stamp1;
  StampArithm<short int> stamp2;
  StampArithm<short int> stamp3;
  StampArithm<short int> stamp4;

  stamp1.Load(blob);
  stamp2.Load(blob);
  stamp3.Load(blob);
  stamp4.Load(blob);

  std::string s = stamp1.UnloadStr();
  std::vector<char> vec= stamp2.UnloadBin();
  short int i = stamp3.UnloadValue();

  sized_ptr<short int> spi = stamp4.UnloadPValue();
  short int *pi = spi;

  std::cout << "String value: '" << s <<"'\n";
  std::cout << "Bin value: '" << vec[0] <<"', '" << vec[1] <<"'\n";
  std::cout << "Value: " << i  <<"\n";
  std::cout << "PValue: " << *pi  <<"\n";
}