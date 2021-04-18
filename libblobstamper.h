

class Blob
{
    private:
    public:
        Blob(char * data, int size);
        bool isEmpty ();

        char * data;  /*FIZME потом сделать private*/
        int    size;
        int    begin;
        int    end;

};

typedef struct wflBlobDsc
{
  struct wflMemCtx * mctx;
  char * data;
  int    begin;
  int    end;
}wflBlobDsc;


void wflBlobDump(wflBlobDsc* blob);

Blob wflShiftN(Blob &blob, size_t n);
std::string wflShiftDouble(Blob &blob);
std::string wflShiftPgPoint(Blob &blob);
std::string wflShiftPgPath(Blob &blob);



