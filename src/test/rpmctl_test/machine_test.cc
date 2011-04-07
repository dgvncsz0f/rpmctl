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

#include <cstdlib>
#include <cstring>
#include <unicode/unistr.h>
#include <UnitTest++.h>
#include <rpmctl/machine.hh>

namespace rpmctl_test
{

  TEST(rw_for_int8_t_type)
  {
    char buffer[1];
    for (int k=0; k<100; k+=1)
    {
      int8_t n = static_cast<int8_t>(std::rand());
      rpmctl::machine::write(buffer, n);
      CHECK_EQUAL(n, rpmctl::machine::read_int8t(buffer));
    }
  }

  TEST(rw_for_int16_t_type)
  {
    char buffer[2];
    for (int k=0; k<100; k+=1)
    {
      int16_t n = static_cast<int16_t>(std::rand());
      rpmctl::machine::write(buffer, n);
      CHECK_EQUAL(n, rpmctl::machine::read_int16t(buffer));
    }
  }

  TEST(rw_for_int32_t_type)
  {
    char buffer[4];
    for (int k=0; k<100; k+=1)
    {
      int32_t n = static_cast<int32_t>(std::rand());
      rpmctl::machine::write(buffer, n);
      CHECK_EQUAL(n, rpmctl::machine::read_int32t(buffer));
    }
  }

  TEST(rw_for_cstr_type)
  {
    char buffer[1024];
    char rbuffer[1024];
    rpmctl::machine::write(buffer, static_cast<const char *>("foobar"), std::strlen("foobar")+1);
    rpmctl::machine::read_string(rbuffer, buffer);
    CHECK_EQUAL(0, std::strcmp("foobar", rbuffer));
  }

  TEST(rw_for_ustr_type)
  {
    char buffer[1024];
    UnicodeString rbuffer;
    rpmctl::machine::write(buffer, UnicodeString("foobar"));
    rpmctl::machine::read_string(rbuffer, buffer);
    CHECK(UnicodeString("foobar") == rbuffer);
  }
}
