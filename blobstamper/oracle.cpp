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

#include <math.h>

#include"oracle.h"

size_t
OracleProportion(ORACLE_TYPE oracle, size_t min, size_t max)
{
  /* Sorry explanation is in Russian. PR translation if you can */
  /* Считаем пропорацию, с округлением вниз.
   * Диапазон увиличиваем на единицу, чтобы хвост диапазона пресказания при
   * округлении вниз как раз попадал в последний элемент целевого диапозона.
   * Разброс предсказания увеличиваем на единицу, так чтобы ровно на конец
   * хвоста диапазона предсказания не попасть никогда и тогда он не округлиться 
   * в max + 1*/
  size_t delta = max - min + 1;
  size_t res = floor(((float) oracle) / ((float) ORACLE_MAX + 1) * delta);
  return min + res;
}

