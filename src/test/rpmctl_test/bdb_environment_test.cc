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

#include <unicode/unistr.h>
#include <UnitTest++.h>
#include <rpmctl/bdb_environment.hh>
#include <rpmctl_test/helpers/file_utils.hh>

namespace rpmctl_test
{

  std::string bdb_environment_setup()
  {
    boost::filesystem::path file = fixtures_path() / "tmp";
    rpmctl_test::cleanupdir(file.string());
    return(file.string());
  }

  TEST(bdb_environment_test_should_return_the_default_when_variable_is_not_defined)
  {
    std::string dbhome = bdb_environment_setup();
    rpmctl::bdb_environment env(dbhome);
    CHECK(UnicodeString("default") == env.get("namespace", "foobar", "default"));
  }

  TEST(bdb_environment_test_should_return_the_variable_defined_in_previous_put_invocation)
  {
    std::string dbhome = bdb_environment_setup();
    rpmctl::bdb_environment env(dbhome);
    env.put("namespace", "foobar", "foobar");
    CHECK(UnicodeString("foobar") == env.get("namespace", "foobar", "default"));
  }

  TEST(bdb_environment_test_should_perform_durable_updates)
  {
    std::string dbhome = bdb_environment_setup();
    {
      rpmctl::bdb_environment env(dbhome);
      env.put("namespace", "foobar", "foobar");
    }
    rpmctl::bdb_environment env(dbhome);
    CHECK(UnicodeString("foobar") == env.get("namespace", "foobar", "default"));
  }

  TEST(bdb_environment_should_raise_exception_if_cant_create_database)
  {
    try
    {
      rpmctl::bdb_environment env("/some/nonsense/path");
      CHECK(false);
    }
    catch (const rpmctl::rpmctl_except &)
    {}
  }

}
