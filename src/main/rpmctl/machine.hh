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

#ifndef __RPMCTL_MACHINE_HH__
#define __RPMCTL_MACHINE_HH__

#include <cstdlib>
#include <cstring>
#include <inttypes.h>
#include <unicode/unistr.h>

namespace rpmctl
{
  typedef char bytestring;
  
  /*! This class provides minimalist serialization primitives.
   *
   * TODO:provide unsigned int versions
   */
  class machine
  {
  public:
    static size_t write(bytestring *b, int8_t n);

    static int8_t read_int8t(const bytestring *b);

    static size_t write(bytestring *b, int16_t n);

    static size_t write(bytestring *b, int32_t n);

    static size_t write(bytestring *b, const char *s, size_t len);

    static size_t write(bytestring *b, const UnicodeString &s);

    static int16_t read_int16t(const bytestring *b);

    static int32_t read_int32t(const bytestring *b);

    static size_t read_string(char *s, const bytestring *b);

    static size_t read_string(UnicodeString &s, const bytestring *b);
  };

}

#endif
