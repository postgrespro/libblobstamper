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

#ifndef STAMP_ORACLE_H
#define STAMP_ORACLE_H

#include <cstddef>
#include <limits>

#define ORACLE_TYPE unsigned short int
#define ORACLE_STAMP StampArithm<ORACLE_TYPE>
#define ORACLE_SIZE sizeof(ORACLE_TYPE)
#define ORACLE_MAX std::numeric_limits<ORACLE_TYPE>::max()

size_t OracleProportion(ORACLE_TYPE oracle, size_t min, size_t max);

#endif  /* STAMP_ORACLE_H */

