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

#ifndef STAMP_TEXT_H
#define STAMP_TEXT_H


#include "galley.h"

#include "stamp_dict.h"

class StampTextPulp: public StampBaseStr
{
  public:
    virtual int minSize() override { return  1; }
    virtual int maxSize() override { return -1; }
    std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};

class StampTextPulpWords:  public GalleyVectorStrStampBase<StampTextPulp>
{
  public:
    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};

class StampTextDictWords:  public GalleyVectorStrStampBase<StampDictLCAlphaSmall>
{
  public:
    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;
};

#endif /* STAMP_TEXT_H */
