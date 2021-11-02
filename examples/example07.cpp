#include<stdio.h>
#include<string.h>

#include<string>
#include<iostream>

#include<blobstamper/blobstamper.h>

typedef struct Point3D
{
  short int X;
  short int Y;
  short int Z;
} Point3D;

class StampPoint3D: public StampBaseStr, public StampBaseV<Point3D>
{
  protected:
    StampArithm<short int> stampX, stampY, stampZ;
  public:
    virtual int minSize() override;
    virtual int maxSize() override;
    virtual std::string ExtractStr(Blob &blob) override;
    virtual Point3D ExtractValue(Blob &blob) override;
};

int StampPoint3D::minSize()
{
  return stampX.minSize() + stampY.minSize() + stampZ.minSize();
}

int StampPoint3D::maxSize()
{
  return stampX.maxSize() + stampY.maxSize() + stampZ.maxSize(); 
}

std::string StampPoint3D::ExtractStr(Blob &blob)
{
  std::string X,Y,Z;
  X = stampX.ExtractStr(blob);
  Y = stampY.ExtractStr(blob);
  Z = stampZ.ExtractStr(blob);
  return "(" + X + ", " + Y + ", " + Z + ")";
}

Point3D StampPoint3D::ExtractValue(Blob &blob)
{
  Point3D res;
  res.X = stampX.ExtractValue(blob);
  res.Y = stampY.ExtractValue(blob);
  res.Z = stampZ.ExtractValue(blob);
  return res;
}


class StampPolyLine3D:  public GalleyVectorStr, public StampBaseStr
{
  protected:
    StampPoint3D * item_stamp_p;
  public:
    StampPolyLine3D(): GalleyVectorStr(*(item_stamp_p = new StampPoint3D())) {};
    ~StampPolyLine3D() {delete item_stamp_p;};

    virtual std::string ExtractStr(Blob &blob) override;
};


std::string StampPolyLine3D::ExtractStr(Blob &blob)
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
  res = "(" + res + ")";
  return res;
}

int main()
{
  char data[] = "abcdef" "abcdef" "ABCDEF" "012345";
  Blob blob(data, strlen(data));
  StampPolyLine3D stamp;

  std::string s = stamp.ExtractStr(blob);
  std::cout << s <<"\n";
}
