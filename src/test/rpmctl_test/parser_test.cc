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
#include <rpmctl/parser.hh>
#include <rpmctl_test/helpers/file_utils.hh>
#include <rpmctl_test/helpers/memory_builder.hh>

namespace rpmctl_test
{

  TEST(parser_should_not_emit_on_variable_on_files_that_has_no_variables)
  {
    UnicodeString buffer;
    std::map<UnicodeString,UnicodeString> env;

    boost::filesystem::path file = fixtures_path() / "file_without_variables";
    memory_builder membuilder(buffer, env);
    rpmctl::parser<UnicodeString> parser(membuilder, "rpmctl");
    parser.run(file.string());

    CHECK_EQUAL(-1, buffer.indexOf("$("));
    CHECK_EQUAL(35148, buffer.length());
    CHECK_EQUAL(UnicodeString("∎")[0], buffer[buffer.length()-1]);
  }

  TEST(parser_should_emit_on_variable_on_files_that_contains_at_least_one)
  {
    UnicodeString buffer;
    std::map<UnicodeString,UnicodeString> env;
    env["rpmctl.version"] = "3";
    env["rpmctl.date"]    = "29 June 2007";
    env["rpmctl.copysym"] = "©";

    boost::filesystem::path file = fixtures_path() / "file_with_variables";
    memory_builder membuilder(buffer, env);
    rpmctl::parser<UnicodeString> parser(membuilder, "rpmctl");
    parser.run(file.string());

    CHECK_EQUAL(35146, buffer.length());
    CHECK_EQUAL(-1, buffer.indexOf("$(version)"));
    CHECK_EQUAL(-1, buffer.indexOf("$(date)"));
    CHECK_EQUAL(-1, buffer.indexOf("$(copysym)"));
    CHECK(buffer.indexOf("3, 29 June 2007") != -1);
    CHECK(buffer.indexOf("Copyright © 2007") != -1);
  }

  TEST(parser_should_work_on_empty_files)
  {
    UnicodeString buffer;
    std::map<UnicodeString,UnicodeString> env;

    boost::filesystem::path file = fixtures_path() / "empty_file";
    memory_builder membuilder(buffer, env);
    rpmctl::parser<UnicodeString> parser(membuilder, "rpmctl");
    parser.run(file.string());

    CHECK_EQUAL(1, buffer.length());
    CHECK_EQUAL(UnicodeString("∎")[0], buffer[buffer.length()-1]);
  }

}
