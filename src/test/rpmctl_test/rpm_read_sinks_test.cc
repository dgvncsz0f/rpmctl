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
#include <UnitTest++.h>
#include <rpmctl/rpm.hh>
#include <rpmctl/scoped_tmpfh.hh>
#include <rpmctl/rpm_read_sinks.hh>
#include <rpmctl_test/helpers/file_utils.hh>

namespace rpmctl_test
{

  TEST(memory_sink_pushes_everything_into_memory_and_let_it_avaialbe_as_a_string_object)
  {
    rpmctl::memory_sink mem;
    mem("foobar", 6);
    mem.eof();
    CHECK(mem.string() == "foobar");
  }

  TEST(file_sink_writes_everything_into_the_file)
  {
    rpmctl::scoped_tmpfh tmpfile;
    rpmctl::file_sink fsink(tmpfile.tmpfile());
    fsink("foobar", 6);
    fsink.eof();

    CHECK(rpmctl_test::read_file(tmpfile.tmpfile()) == "foobar");
  }

}
