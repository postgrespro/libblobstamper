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

#include "blob.h"
#include "stamp.h"
#include "stamp_arithm.h"
#include "stamp_dict.h"
#include "dict.h"
#include "galley.h"
#include "stamp_enumerator.h"
#include "stamp_lottery.h"
#include "stamp_math_op.h"
#include "stamp_text.h"
#include "stamp_json.h"
