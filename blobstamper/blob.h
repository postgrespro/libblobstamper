
#ifndef BLOB_H
#define BLOB_H

#include <string>
#include <list>

class StampBase;

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

        void * ShiftSingleStampBin(StampBase &stmp);
        std::string  ShiftSingleStampStr(StampBase &stmp);
};


#endif /*BLOB_H*/