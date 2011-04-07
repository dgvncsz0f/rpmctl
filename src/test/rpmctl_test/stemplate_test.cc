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
#include <rpmctl/scoped_file.hh>
#include <rpmctl/environment.hh>
#include <rpmctl/stemplate.hh>
#include <rpmctl_test/helpers/file_utils.hh>
#include <rpmctl_test/helpers/memory_builder.hh>

namespace rpmctl_test
{

  TEST(on_start_with_on_eof_should_create_an_empty_file)
  {
    rpmctl::scoped_file file(".stemplate_test#0");

    rpmctl::nil_env env;
    rpmctl::stemplate stemplate(env);
    rpmctl::stemplate_handler *handler = NULL;

    handler = stemplate.on_start(*file);
    stemplate.on_eof(handler);

    UnicodeString txt = rpmctl_test::read_file(*file);
    CHECK(rpmctl_test::file_exists(*file) == true);
    CHECK(txt == "");
  }

  TEST(on_text_should_write_to_file_as_is)
  {
    rpmctl::scoped_file file(".stemplate_test#1");

    rpmctl::nil_env env;
    rpmctl::stemplate stemplate(env);
    rpmctl::stemplate_handler *handler = NULL;

    handler = stemplate.on_start(*file);
    stemplate.on_text("foobar", handler);
    stemplate.on_text(" ", handler);
    stemplate.on_text("foobaz", handler);
    stemplate.on_eof(handler);

    UnicodeString txt = rpmctl_test::read_file(*file);
    CHECK(rpmctl_test::file_exists(*file) == true);
    CHECK(txt == "foobar foobaz");
  }

  TEST(on_variable_should_write_the_original_variable_if_it_cant_find_it_onto_environment)
  {
    rpmctl::scoped_file file(".stemplate_test#2");

    rpmctl::nil_env env;
    rpmctl::stemplate stemplate(env);
    rpmctl::stemplate_handler *handler = NULL;

    handler = stemplate.on_start(*file);
    stemplate.on_variable("namespace", "foobar", handler);
    stemplate.on_eof(handler);

    UnicodeString txt = rpmctl_test::read_file(*file);
    CHECK(rpmctl_test::file_exists(*file) == true);
    CHECK(txt == "$(foobar)");
  }

  TEST(on_qualified_variable_should_write_the_original_variable_if_it_cant_find_it_onto_environment)
  {
    rpmctl::scoped_file file(".stemplate_test#2");

    rpmctl::nil_env env;
    rpmctl::stemplate stemplate(env);
    rpmctl::stemplate_handler *handler = NULL;

    handler = stemplate.on_start(*file);
    stemplate.on_qualified_variable("namespace", "foobar", handler);
    stemplate.on_eof(handler);

    UnicodeString txt = rpmctl_test::read_file(*file);
    CHECK(rpmctl_test::file_exists(*file) == true);
    CHECK(txt == "$(namespace::foobar)");
  }

  TEST(on_error_should_not_overwrite_the_original_file)
  {
    rpmctl::scoped_file file(".stemplate_test#2");

    rpmctl::nil_env env;
    rpmctl::stemplate stemplate(env);
    rpmctl::stemplate_handler *handler = NULL;

    handler = stemplate.on_start(*file);
    stemplate.on_error(handler);

    CHECK(rpmctl_test::file_exists(*file) == false);
  }

}
