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

#ifndef STAMP_ENUMERATOR_H
#define STAMP_ENUMERATOR_H

#include"galley.h"
#include"stamp.h"
#include"blob.h"

#include<string>

class StampStrEnumerator: public GalleyVectorStr, public StampBaseStr
{
  protected:
    StampBaseStr & stamp_str;
    const std::string separator;
    const std::string left_bracket;
    const std::string right_bracket;
  public:
    StampStrEnumerator(StampBaseStr &arg_stamp,
                       const std::string arg_sep,
                       const std::string arg_l,
                       const std::string arg_r
                      ):
                    stamp_str(arg_stamp),
                    separator(arg_sep),
                    left_bracket(arg_l),
                    right_bracket(arg_r),
                    GalleyVectorStr(arg_stamp) {};

    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;

};


#endif  //STAMP_ENUMERATOR_H
