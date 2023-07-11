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
#include"stamp_math_op.h"

std::string
StampMathUnaryOp::ExtractStr(std::shared_ptr<Blob> blob)
{
  return  op_name + "(" +  stamp.ExtractStr(blob) + ")";
}


std::string
StampMathBinaryOp::ExtractStr(std::shared_ptr<Blob> blob)
{
  std::vector<std::shared_ptr<Blob>> blobs = extract_internal(blob);
  return (std::string)"(" +  stamp1.ExtractStr(blobs[0]) + " "+  op_name + " " + stamp2.ExtractStr(blobs[1]) + ")";
}

