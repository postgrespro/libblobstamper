
#ifndef BLOB_H
#define BLOB_H

#include <string>
#include <list>
#include <vector>


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
        std::vector<char> ChopBlank(StampBase &stmp);
        void DataDup(char *& data_out, size_t& size_out);
        std::vector<char> asVector();


        std::vector<char> ShiftSingleStampBin(StampBase &stmp);
        std::string  ShiftSingleStampStr(StampBase &stmp);


};

class OutOfData  /*An exeption. Experemental for now*/
{

};

#endif /*BLOB_H*/