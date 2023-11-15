/******************************************************************************
 *
 * Copyright 2021 Nikolay Shaplov (Postgres Professional)
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

#include <string>
#include <vector>
#include <iostream>

#include "blobstamper/stamp_json.h"
#include <sys/stat.h>

#include <fstream>


long GetFileSize(std::string filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

int
main(int argc, char *argv[])
{
    if (argc<2)
    {
        fprintf(stderr, "Please specify input file name as first argument\n");
        exit(1);
    }
    std::string file_name = argv[1];

    std::fstream f{file_name};
    if (!f)
    {
        std::cerr << "Unable to open file '"<<file_name<<"'\n";
        exit (1);
    }
    long file_size = GetFileSize(file_name);

    std::vector<char> buffer (file_size,0);
    f.read(&buffer[0], buffer.size());

    auto blob = std::make_shared<Blob>(&buffer[0], buffer.size());

    auto stamp_j = std::make_shared<StampJSON>();
    std::string s = stamp_j->ExtractStr(blob);

    printf("%s\n", s.c_str());

}
