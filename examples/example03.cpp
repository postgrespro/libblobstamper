#include<stdio.h>
#include<string.h>

#include<string>
#include<iostream>

#include<blobstamper/blobstamper.h>

class StampPoint3D: public StampBaseStr
{
  protected:
    StampArithm<short int> stampX, stampY, stampZ;
  public:
    virtual int minSize() override;
    virtual int maxSize() override;
    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};

int StampPoint3D::minSize()
{
  return stampX.minSize() + stampY.minSize() + stampZ.minSize();
}

int StampPoint3D::maxSize()
{
  return stampX.maxSize() + stampY.maxSize() + stampZ.maxSize();
}

std::string StampPoint3D::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::string X,Y,Z;
  X = stampX.ExtractStr(blob);
  Y = stampY.ExtractStr(blob);
  Z = stampZ.ExtractStr(blob);
  return "(" + X + ", " + Y + ", " + Z + ")";
}


int main()
{
  char data[] = "abcdef";
  auto blob = std::make_shared<Blob>(data, strlen(data));
  StampPoint3D stamp;

  std::string s = stamp.ExtractStr(blob);

  std::cout << "String value: '" << s <<"'\n";
}
