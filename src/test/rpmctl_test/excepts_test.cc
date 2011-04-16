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

#include <cstring>
#include <UnitTest++.h>
#include <rpmctl/excepts.hh>

namespace rpmctl_test
{

  TEST(rpmctl_except_what_should_return_string_provided_in_ctor)
  {
    rpmctl::rpmctl_except except("foobar");
    CHECK(std::strcmp("foobar", except.what()) == 0);
  }

  TEST(rpmctl_except_copy_ctor_should_copy_message_from_original_obj)
  {
    rpmctl::rpmctl_except except_a("foobar");
    rpmctl::rpmctl_except except_b(except_a);
    CHECK(std::strcmp(except_a.what(), except_b.what()) == 0);
  }

  TEST(rpmctl_except_assignment_operator_should_copy_message_from_original_obj)
  {
    rpmctl::rpmctl_except except_a("foobar");
    rpmctl::rpmctl_except except_b("foobaz");
    except_b = except_a;
    CHECK(std::strcmp(except_a.what(), except_b.what()) == 0);
  }

}
