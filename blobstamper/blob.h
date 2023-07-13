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

#ifndef BLOB_H
#define BLOB_H

#include <string>
#include <list>
#include <vector>
#include <memory>


class StampBase;

class Blob
{
    protected:
        char*  data;
        size_t    size;
        size_t    begin;
        size_t    end;
    public:
        Blob(char * data, size_t size);
        bool isEmpty ();
        size_t Size();
        void Dump();

        std::vector<char> AsByteVector();
        std::shared_ptr<Blob> Chop(size_t chop_size);
        std::shared_ptr<Blob> Chop(size_t min_size, size_t max_size);

        void DataDup(char *& data_out, size_t& size_out);
        std::vector<char> asVector();
        std::string asString(); /* Should not be used in prod, for tests and examples only*/
};

class OutOfData  /*An exeption. Experemental for now*/
{

};

class NotImplemented  /*An exeption */
{

};

#endif /*BLOB_H*/
