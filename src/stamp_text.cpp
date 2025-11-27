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

#include "blobstamper/stamp_text.h"

std::string
StampStringLatin1::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();

    std::vector<char>::iterator iterator;

    iterator = data.begin();
    std::string res;
    while (iterator != data.end())
    {
      if (*iterator == '\0') { *iterator = ' '; }
      res.push_back(*iterator++);
    }

    return res;
}

std::string
StampStringASCII::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();

    std::vector<char>::iterator iterator;

    iterator = data.begin();
    std::string res;
    while (iterator != data.end())
    {
      *iterator &= 0x7F; // Remove first bit. This will made it ASCII (or more precisely 7bit encoding including character 0x01..0x1F)
      if (*iterator == '\0') { *iterator = ' '; }
      res.push_back(*iterator++);
    }
    return res;
}

// FIXME: translate into English

/* Эта функция "нормализует" три байта до utf-32 символа. */

/* В utf-32 для кодировки одного символа используется три байта, четвертый всегда не используется */
/* Первый и второй байт при этом используется полностью, а третий принимает значение от 0x00 до 0x10 */

/*
 * Нам надо нормолизовать первые три байта так, чтобы нормализованные значения были распределены равномерно
 * если нормализованные данные были распределены равномерно, или блико к тому. Проблема в том, как равномерно
 * распределить третий байт. Для значений 0x00..0x0F можно добиться равномерного распредеоения обнулив старшие
 * 4 бита. Тогда вероятность каждого из значений бует 1/16
 * Но у нас еще есть 0x10 который нельзя получить откидыванием бит, но который нам тоже нужен. При этом вероятонстью
 * равной или схожей с 0x00..0x0F.
 * Поэтому мы если старшие четыре бита принимают значение 0xF, то мы будем считать, что результат у нас 0x10. Вероятнсть этого 1/16
 * Во всех остальных случаях будем возвращать значение младших бит. За счет вышеуказанного кейса вероятность каждого значения станет 1/16-1/256
 * 1/256 -- это тот случай когда старшие четыре бита приняли значение 0xF
 * Будем считать это распредеделение вероятностей достаточно равномерным для наших задач
 */

/*inline*/ uint32_t
ThreeBytesToUtf32UInt(char *buf)
{
  uint32_t res = 0;
  char * res_buf = (char *) &res;
  res_buf[3] = 0; // Старший байт всегда ноль
  res_buf[1] = buf[1];
  res_buf[0] = buf[0];

  if ( (buf[2] & 0xF0) == 0xF0) // вероятность этого 1/16
  {
    res_buf[2] = 0x10;
  } else
  {
    res_buf[2] = buf[2] & 0x0F;  // Значения от 0 до 0x0F, вероятность каждого из них 1/16 - 1/256 (с учетом предыдущей ветки if'а)
  }
  if ( (res_buf[2] == 0) && ((res_buf[1] & 0xF8) == 0xD8))  // 0x0000FyXX y=8..F, X=0..F
  {
    // Это суррогатные UTF-16 символы нулевого плейна которые в одиночку встречаться не должны. https://en.wikipedia.org/wiki/Universal_Character_Set_characters#Surrogates
    // Заменяем их на подчеркивание
    res_buf[1] = 0;
  res_buf[0] = '_';
  }
  return res;
}

size_t
append_with_utf8(unsigned char * buf, uint32_t c32)
{
  // check if sum in ascii diapazone
  if(c32 <= 0x7F)
  {
    buf[0] = c32 & 0x7F;
    return 1;
  }

  // if contains 2 bytes
  if(c32 <= 0x7FF)
  {
    // get 1st byte
    buf[0] = 6 << 5;
    buf[0] += c32 >> 6;
    // get 2nd byte
    buf[1] = 2 << 6;
    buf[1] += c32 & 0x3f;
  return 2;
  }

  // if contains 3 bytes
  if(c32 <= 0xffff)
  {
    buf[0] = 0xe << 4;
    buf[0] += c32 >> 0xc;

    buf[1] = 2 << 6;
    buf[1] += (c32 >> 6) & 0x3f;

    buf[2] = 2 << 6;
    buf[2] += c32 & 0x3f;
    return 3;
  }

  // if contains 4 bytes
  if(c32 <= 0x10ffff)
  {
    buf[0] = 0x1e << 3;
    buf[0] += (c32 >> 0x12) & 7;

    buf[1] = 2 << 6;
    buf[1] += (c32 >> 0xc) & 0x3f;

    buf[2] = 2 << 6;
    buf[2] += (c32 >> 6) & 0x3f;

    buf[3] = 2 << 6;
    buf[3] += c32 & 0x3f;
    return 4;

  } else {
    /* FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME FIXME                FIXME  */
    printf("error in parsing utf32, maybe its not utf32?");
    return 0;
  }
}

void
utf24_to_utf8(char * in, size_t in_size, char **out, size_t *out_size)
{
  in_size = (in_size / 3) * 3;
  *out_size = (in_size / 3) * 4 + 1; // +1 for final zero
  *out = (char *) malloc(*out_size);

  unsigned char * p = (unsigned char*) *out;
  for(int i=0; i<in_size; i+=3)
  {
    uint32_t utf32 = ThreeBytesToUtf32UInt(in + i);
    p = p + append_with_utf8(p, utf32);
  }
  *p = '\0'; // final zero
}

std::string
StampStringUTF8::ExtractStr(std::shared_ptr<Blob> blob)
{
    std::vector<char> data = blob->Chop(minSize(), maxSize())->AsByteVector();

    char * raw_res;
    size_t raw_res_size;

    utf24_to_utf8(&data[0], data.size(), &raw_res, &raw_res_size);

    std::string res = raw_res;
    free(raw_res);

    return res;
}
