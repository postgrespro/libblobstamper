
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

