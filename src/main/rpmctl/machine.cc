/*
 * Copyright (c) 2010, Diego Souza
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *   * Neither the name of the <ORGANIZATION> nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <rpmctl/machine.hh>

size_t rpmctl::machine::write(char *b, int8_t n)
{
  if (b != NULL)
    b[0] = static_cast<char>(n);
  return(sizeof(char));
}

int8_t rpmctl::machine::read_int8t(const char *b)
{
  return(static_cast<int8_t>(b[0]));
}

size_t rpmctl::machine::write(bytestring *b, int16_t n)
{
  if (b != NULL)
  {
    write(b, static_cast<int8_t>(n));
    write(b+1, static_cast<int8_t>((n >> 8) & 0xFF));
  }
  return(2);
}

size_t rpmctl::machine::write(bytestring *b, int32_t n)
{
  if (b != NULL)
  {
    write(b, static_cast<int16_t>(n));
    write(b+2, static_cast<int16_t>((n >> 16) & 0xFFFF));
  }
  return(4);
}

size_t rpmctl::machine::write(bytestring *b, const char *s, size_t len)
{
  if (b != NULL)
  {
    write(b, static_cast<int32_t>(len));
    std::memcpy(b+4, s, len);
  }
  return(4 + len);
}

size_t rpmctl::machine::write(bytestring *b, const UnicodeString &s)
{
  int32_t len = s.extract(0, s.length(), NULL, "UTF-8") + 1;
  if (b != NULL)
  {
    write(b, len);
    s.extract(0, s.length(), b+4, "UTF-8");
  }
  return(4 + len);
}

int16_t rpmctl::machine::read_int16t(const bytestring *b)
{
  int8_t one = read_int8t(b);
  int8_t two = read_int8t(b+1);
  return(one | ((two << 8) & 0xFF));
}

int32_t rpmctl::machine::read_int32t(const bytestring *b)
{
  int16_t one = read_int16t(b);
  int16_t two = read_int16t(b+2);
  return(one | ((two << 16) & 0xFFFF));
}

size_t rpmctl::machine::read_string(char *s, const bytestring *b)
{
  int32_t n = read_int32t(b);
  if (s != NULL)
    std::memcpy(s, b+4, n);
  return(4+n);
}

size_t rpmctl::machine::read_string(UnicodeString &s, const bytestring *b)
{
  int32_t n = read_int32t(b);
  s.append(UnicodeString(b+4, n-1, "UTF-8"));
  return(4+n);
}
