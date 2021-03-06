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

#include <iostream>
#include <unicode/ustream.h>
#include <rpmctl/parser.hh>
#include <rpmctl_test/helpers/memory_builder.hh>

rpmctl_test::memory_builder::~memory_builder()
{}

rpmctl_test::memory_builder::memory_builder(UnicodeString &buffer, const std::map<UnicodeString,UnicodeString> &e) :
  _buffer(buffer),
  _env(e)
{}

UnicodeString *rpmctl_test::memory_builder::on_start(const std::string &)
{
  return(&_buffer);
}

void rpmctl_test::memory_builder::on_text(const UnicodeString &txt, UnicodeString *buffer)
{
  buffer->append(txt);
}

void rpmctl_test::memory_builder::on_variable(const UnicodeString &ns, const UnicodeString &txt, UnicodeString *buffer)
{
  std::map<UnicodeString,UnicodeString>::const_iterator it = _env.find(ns +"::"+ txt);
  if (it != _env.end())
    buffer->append(it->second);
  else
    buffer->append("$("+ txt +")");
}

void rpmctl_test::memory_builder::on_qualified_variable(const UnicodeString &ns, const UnicodeString &txt, UnicodeString *buffer)
{
  std::map<UnicodeString,UnicodeString>::const_iterator it = _env.find(ns +"::"+ txt);
  if (it != _env.end())
    buffer->append(it->second);
  else
    buffer->append("$("+ ns +"::"+ txt +")");
}

void rpmctl_test::memory_builder::on_eof(UnicodeString *buffer)
{
  buffer->append("∎");
}

void rpmctl_test::memory_builder::on_error(UnicodeString *buffer)
{
  buffer->append("☠");
}
