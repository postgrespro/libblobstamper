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

#ifndef STAMP_MATH_OP_H
#define STAMP_MATH_OP_H

#include "blob.h"
#include "stamp.h"
#include "galley.h"


class StampMathUnaryOp: public StampBaseStr
{
  protected:
    std::string op_name;
    std::shared_ptr<StampBaseStr> stamp;
  public:
    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    StampMathUnaryOp(std::string arg_op_name, std::shared_ptr<StampBaseStr> arg_stamp): op_name(arg_op_name), stamp(arg_stamp) {};
    virtual int maxSize() override {return -1;};
    virtual int minSize() override {return stamp->minSize();};
};



class StampMathBinaryOp: public StampBaseStr, public GalleySetBase
{
  protected:
    std::string op_name;
    std::shared_ptr<StampBaseStr> stamp1;
    std::shared_ptr<StampBaseStr> stamp2;
  public:
    virtual std::string ExtractStr(std::shared_ptr<Blob> blob) override;
    StampMathBinaryOp(std::string arg_op_name,
                      std::shared_ptr<StampBaseStr> arg_stamp1,
                      std::shared_ptr<StampBaseStr> arg_stamp2):
        GalleySetBase({arg_stamp1, arg_stamp2}),
        op_name(arg_op_name),
        stamp1(arg_stamp1),
        stamp2(arg_stamp2) {};
    virtual int maxSize() override {return -1;};

};

#endif // STAMP_MATH_OP_H
