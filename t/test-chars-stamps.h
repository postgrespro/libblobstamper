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

/* This stamps chops first two bytes and treat them as string */
/* Never do this in real live, as blob is binary and may have \0 in the middle of it*/

class StampTwoChars: public StampFixed
{
  public:
    StampTwoChars();
    std::string ExtractStr(Blob &blob) override;
};

StampTwoChars::StampTwoChars() : StampFixed()
{
    size = 2;  /* This stamp shifts two characters only */
}

std::string
StampTwoChars::ExtractStr(Blob &blob)
{
    char * buf;
    size_t buf_size;

    Blob blob2 = blob.ShiftBytes(size);
    if (blob2.isEmpty())
        return "";

    /* Save shited data as string */
    /* NEVER do this in prod, as in real live blob is binary and may have 0 in the middle of it */
    blob2.DataDup(buf, buf_size);
    buf = (char *) realloc((void *)buf, buf_size + 1);
    buf[buf_size] = '\0';
    std::string res = buf;
    free(buf);

    return res;
}

class StampSeveralChars: public StampVariated
{
  public:
    StampSeveralChars();
    std::string ExtractStr(Blob &blob) override;
};

StampSeveralChars::StampSeveralChars()
{
    min_size = 2;
    max_size = 8;
}

std::string
StampSeveralChars::ExtractStr(Blob &blob)
{

    std::vector<char> data = blob.ChopBlank(*this);
    /* Save optained data as string */
    /* NEVER do this in prod, as in real live blob is binary and may have 0 in the middle of it */
    char * buf;
    buf = (char *) malloc(data.size() + 1);
    memcpy((void *) buf, (void *) &data[0], data.size());
    buf[data.size()] = '\0';

    std::string res = buf;
    free(buf);

    return res;
}

class StampTwoCharsList: public StampUnbounded
{
  protected:
    StampTwoChars el_stamp;
    GalleyVector galley;
  public:
    std::string ExtractStr(Blob &blob) override;
    StampTwoCharsList(): el_stamp {}, galley {el_stamp} {};

    virtual int minSize() override {return el_stamp.minSize();};
};

std::string
StampTwoCharsList::ExtractStr(Blob &blob)
{
    std::string res = "";
    std::vector<std::string> list = galley.ExtractStr(blob);

    for (std::string point : list)
    {
        if (!res.empty()) res = res + ", ";
        res = res + point;
    }

    res = "(" + res + ")";
    return res;
}


