
#ifndef BLOB_H
#define BLOB_H

#include <string>

class StampGeneric;

class Blob
{
    protected:
        char*  data;
        int    size;
        int    begin;
        int    end;
    public:
        Blob(char * data, int size);
        bool isEmpty ();
        size_t Size();
        void Dump();
        Blob ShiftBytes(size_t n);
        void DataDup(char *& data_out, size_t& size_out);

        void * ShiftSingleStampBin(StampGeneric &stmp);
        std::string  ShiftSingleStampStr(StampGeneric &stmp);
};

#endif /*BLOB_H*/