
#ifndef BLOB_H
#define BLOB_H

#include <string>

class StampGeneric;

class Blob
{
    private:
    public:
        Blob(char * data, int size);
        bool isEmpty ();
        void Dump();
        Blob ShiftBytes(size_t n);

        void * ShiftSingleStampBin(StampGeneric &stmp);
        std::string  ShiftSingleStampStr(StampGeneric &stmp);

        char*  data;  /*FIXME потом сделать private*/
        int    size;
        int    begin;
        int    end;
};

#endif /*BLOB_H*/