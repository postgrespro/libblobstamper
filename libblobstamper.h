
#include <string>
class Blob
{
    private:
    public:
        Blob(char * data, int size);
        bool isEmpty ();
        void Dump();
        Blob ShiftBytes(size_t n);

        char * data;  /*FIXME потом сделать private*/
        int    size;
        int    begin;
        int    end;
};

Blob wflShiftN(Blob &blob, size_t n);
std::string wflShiftDouble(Blob &blob);
std::string wflShiftPgPoint(Blob &blob);
std::string wflShiftPgPath(Blob &blob);



