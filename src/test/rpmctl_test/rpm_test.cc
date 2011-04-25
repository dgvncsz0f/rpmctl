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
#include <rpmctl/rpm_read_sinks.hh>
#include <rpmctl_test/helpers/file_utils.hh>

namespace rpmctl_test
{

  TEST(rpm_ctor_raises_exception_when_files_is_not_an_rpm_package)
  {
    boost::filesystem::path file = fixtures_path() / "empty_file";
    try
    {
      rpmctl::rpm rpm(file.string());
      CHECK(false);
    }
    catch (const rpmctl::rpmctl_except &)
    {}
  }

  TEST(rpm_conffiles_is_able_to_read_files_defined_with_config_macro_inside_a_rpm)
  {
    boost::filesystem::path file = fixtures_path() / "foobar.rpm";
    rpmctl::rpm rpm(file.string());

    std::vector<std::string> files;
    rpm.conffiles(files);
    CHECK(files.size() == 3);
    CHECK(files[0] == "/etc/foobar/cfg1" || files[0] == "/etc/foobar/cfg2" || files[0] == "/etc/foobar/cfg3");
    CHECK(files[1] == "/etc/foobar/cfg1" || files[1] == "/etc/foobar/cfg2" || files[1] == "/etc/foobar/cfg3");
    CHECK(files[2] == "/etc/foobar/cfg1" || files[2] == "/etc/foobar/cfg2" || files[2] == "/etc/foobar/cfg3");
  }

  TEST(rpm_conffiles_reads_nothing_if_file_has_no_config_files_defined)
  {
    boost::filesystem::path file = fixtures_path() / "foobar-noconf.rpm";
    rpmctl::rpm rpm(file.string());

    std::vector<std::string> files;
    rpm.conffiles(files);
    CHECK(files.size() == 0);
  }

  TEST(rpm_name_returns_the_name_declared_in_the_spec)
  {
    boost::filesystem::path file = fixtures_path() / "foobar-noconf.rpm";
    rpmctl::rpm rpm(file.string());
    CHECK(rpm.name() == "foobar-noconf");

    file = fixtures_path() / "foobar.rpm";
    rpmctl::rpm rpm2(file.string());
    CHECK(rpm2.name() == "foobar");
  }

  TEST(rpm_read_file_reads_contents_of_files_inside_an_rpm_package_that_matches_the_given_filename)
  {
    boost::filesystem::path file = fixtures_path() / "foobar-noconf.rpm";
    rpmctl::rpm rpm(file.string());

    rpmctl::memory_sink sink;
    rpm.read_file("/etc/foobar/cfg1", sink);
    CHECK(sink.string() == "$(cfg1)");
  }

  TEST(rpm_read_file_raises_exception_if_filename_does_not_match)
  {
    boost::filesystem::path file = fixtures_path() / "foobar-noconf.rpm";
    rpmctl::rpm rpm(file.string());

    rpmctl::memory_sink sink;
    try
    {
      rpm.read_file("/foo/bar", sink);
      CHECK(false);
    }
    catch (const rpmctl::rpmctl_except &)
    {}
  }

}
