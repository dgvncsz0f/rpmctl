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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <unicode/ustdio.h>
#include <UnitTest++.h>
#include <rpmctl/scoped_fh.hh>
#include <rpmctl/scoped_file.hh>
#include <rpmctl_test/helpers/file_utils.hh>

namespace rpmctl_test
{

  TEST(scoped_fh_keeps_file_open_while_object_is_valid)
  {
    std::string file = ".scoped_fh_test#1";
    rpmctl::scoped_file tmp(file);

    rpmctl::scoped_fh fh(file, "w");
    int fd = fileno(u_fgetfile(*fh));
    CHECK(fd>0);
  }

  TEST(scoped_fh_keeps_closes_file_after_dctor_is_invoked)
  {
    std::string file = ".scoped_fh_test#2";
    rpmctl::scoped_file tmp(file);

    int fd = -1;
    {
      rpmctl::scoped_fh fh(file, "w");
      fd = fileno(u_fgetfile(*fh));
    }

    struct stat x;
    CHECK(fstat(fd, &x) == -1);
    CHECK(errno == EBADF);
  }

  TEST(scoped_fh_open_method_closes_previously_opened_file)
  {
    std::string file = ".scoped_fh_test#3";
    rpmctl::scoped_file tmp1(file + "_1");
    rpmctl::scoped_file tmp2(file + "_2");

    rpmctl::scoped_fh sfh(file + "_1", "w");
    int fd = fileno(u_fgetfile(*sfh));

    struct stat old_s;
    CHECK(fstat(fd, &old_s) == 0);

    sfh.open(file + "_2", "w");
    struct stat new_s;

    int fd2 = fileno(u_fgetfile(*sfh));
    if (fd != fd2)
    {
      CHECK(fstat(fd, &new_s)==-1);
      CHECK(errno == EBADF);
    }
    else
    {
      CHECK(fstat(fd, &new_s)==0);
      CHECK(new_s.st_ino != old_s.st_ino);
    }
  }

}
