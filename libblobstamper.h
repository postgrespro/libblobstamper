

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


typedef struct wflMemCtx
{
    //Nothing for now;
}wflMemCtx ;

extern wflMemCtx static_ctx;

wflMemCtx * wflCreateMemCtx();
void  wflDestroyMemCtx(wflMemCtx * mctx);


typedef struct wflBlobDsc
{
  struct wflMemCtx * mctx;
  char * data;
  int    begin;
  int    end;
}wflBlobDsc;


void wflBlobDump(wflBlobDsc* blob);
void* wflMalloc(wflMemCtx * mctx, size_t size);
void wflFree(wflMemCtx * mctx, void* ptr);

//wflBlobDsc* wflShiftN(wflBlobDsc* blob, size_t n);
//std::string wflShiftDouble(wflBlobDsc* blob);
//std::string wflShiftPgPoint(wflBlobDsc* blob);
//std::string wflShiftPgPath(wflBlobDsc* blob);



extern "C" {

int poly_contain_prepare(char* in, int in_size, char ** res1, char ** res2);

}