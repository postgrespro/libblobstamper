/******************************************************************************
 *
 * Copyright 2021-2023 Nikolay Shaplov (Postgres Professional)
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

class StampTwoChars: public StampBaseStr
{
  public:
    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    virtual int  minSize() override {return 2;} /* This stamp shifts two characters only */
    virtual int  maxSize() override {return 2;} /* This stamp shifts two characters only */

};

std::string
StampTwoChars::ExtractStr(std::shared_ptr<Blob> blob)
{
    /* Chopping suitable data chunk from blob */
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();

    size_t buf_size = data.size() + 1;
    char * buf = (char *) malloc(buf_size);
    memcpy(buf, &data[0], data.size());
    buf[buf_size-1] = '\0';

    /* Save shited data as string */
    /* NEVER do this in prod, as in real live blob is binary and may have 0 in the middle of it */
    std::string res = buf;
    free(buf);

    return res;
}

/*****************************************************************************/
class StampSeveralChars: public StampBaseStr
{
  public:
    virtual int  minSize() override {return 2;} /* Minimal size of consumed data */
    virtual int  maxSize() override {return 8;} /* Maximal size of consumed data */
    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};

std::string
StampSeveralChars::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();
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

/*****************************************************************************/
class StampTwoCharsList: public StampBaseStr
{
  protected:
    std::shared_ptr<StampTwoChars> el_stamp;
    GalleyVectorStr galley;
  public:
    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    StampTwoCharsList(): el_stamp {std::make_shared<StampTwoChars>()}, galley {el_stamp} {};

    virtual int minSize() override {return el_stamp->minSize();};
    virtual int maxSize() override {return -1;};
};

std::string
StampTwoCharsList::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::string res = "";
    std::vector<std::string> list = galley.ExtractStrVector(blob);

    for (std::string point : list)
    {
        if (!res.empty()) res = res + ", ";
        res = res + point;
    }

    res = "(" + res + ")";
    return res;
}


