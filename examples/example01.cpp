#include<stdio.h>
#include<string.h>

#include<string>
#include<iostream>

#include<blobstamper/blobstamper.h>

int main()
{
  char data[] = "abcde";
  auto blob = std::make_shared<Blob>(data, strlen(data));
  StampArithm<short int> stamp;
  std::string s = stamp.ExtractStr(blob);

  std::cout << "Stamp minSize: " << stamp.minSize() << "\n";
  std::cout << "Stamp maxSize: " << stamp.maxSize() << "\n";
  std::cout << "Extracted value: " << s <<"\n";
  std::cout << "Remaining blob: " << blob->asString() << "\n";
}
