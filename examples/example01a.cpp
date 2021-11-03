#include<stdio.h>
#include<string.h>

#include<string>
#include<iostream>

#include<blobstamper/blobstamper.h>

int main()
{
  char data[] = "abcde";
  Blob blob(data, strlen(data));
  StampArithm<short int> stamp;
  stamp.Load(blob);

  std::string s = stamp.UnloadStr();

  std::cout << "Stamp minSize: " << stamp.minSize() << "\n";
  std::cout << "Stamp maxSize: " << stamp.maxSize() << "\n";
  std::cout << "Extracted value: " << s <<"\n";
  std::cout << "Remaining blob: " << blob.asString() << "\n";
}