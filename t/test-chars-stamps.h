
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
    if (blob.Size() < min_size)
        return "";
    char * buf;
    size_t size = max_size;
    if (blob.Size() < max_size)
        size = blob.Size();

    Blob blob2 = blob.ShiftBytes(size);
    if (blob2.isEmpty())
        return "";

    /* Save shited data as string */
    /* NEVER do this in prod, as in real live blob is binary and may have 0 in the middle of it */
    size_t buf_size;
    blob2.DataDup(buf, buf_size);
    buf = (char *) realloc((void *)buf, buf_size + 1);
    buf[buf_size] = '\0';
    std::string res = buf;
    free(buf);

    return res;
}

