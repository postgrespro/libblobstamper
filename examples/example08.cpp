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

class StampStrEnumerator: public GalleyVectorStr, public StampBaseStr
{
  protected:
    StampBaseStr & stamp_str;
    const std::string separator;
    const std::string left_bracket;
    const std::string right_bracket;
  public:
    StampStrEnumerator(StampBaseStr &arg_stamp,
                       const std::string arg_sep,
                       const std::string arg_l,
                       const std::string arg_r
                      ):
                    stamp_str(arg_stamp),
                    separator(arg_sep),
                    left_bracket(arg_l),
                    right_bracket(arg_r),
                    GalleyVectorStr(arg_stamp) {};

    virtual std::string ExtractStr(Blob &blob) override;

};


std::string StampStrEnumerator::ExtractStr(Blob &blob)
{
  std::vector<std::string> data = ExtractStrVector(blob);
  std::string res = "";

  for(std::string s : data)
  {
    if (!res.empty())
    {
      res+= separator;
    }
    res+= s;
  }
  res = left_bracket + res + right_bracket;
  return res;
}



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
  StampPoint3D stamp;

  StampStrEnumerator stamp_e(stamp, " - ", "[", "]");


  std::string s = stamp_e.ExtractStr(blob);
  std::cout << s <<"\n";
}
