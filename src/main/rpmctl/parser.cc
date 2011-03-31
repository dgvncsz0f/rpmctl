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

#include <stdexcept>
#include <stdlib.h>
#include <iostream>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#include <rpmctl/config.hh>
#include <rpmctl/parser.hh>
#include <rpmctl/scoped_fh.hh>

typedef std::pair<int32_t,int32_t> int32t_pair;

static
void __exread(UnicodeString &out, UFILE *fh, size_t bufsz)
{
  UChar *buffer = new UChar[bufsz];
  int32_t length = u_file_read(buffer, bufsz, fh);
  if (length == -1)
    throw(std::runtime_error("error reading file"));
  out.append(UnicodeString(buffer, length));
}

static
int32t_pair __findvar(const UnicodeString &txt)
{
  int32_t spos = txt.indexOf("$(");
  int32_t epos = (spos==-1 ? -1 : txt.indexOf(")", spos));
  return(int32t_pair(spos, epos));
}

rpmctl::parser_events::~parser_events()
{}

rpmctl::parser::parser(parser_events &e) :
  _e(e)
{}

void rpmctl::parser::run(const std::string &file)
{
  void *data = _e.on_start(file);

  try
  {
    rpmctl::scoped_fh fh(file, "r");

    while (!u_feof(*fh))
    {
      UnicodeString txt;
      __exread(txt, *fh, 2*RPMCTL_MAXVARLEN);
      
      int32t_pair pos = __findvar(txt);
      if (pos.first==-1 || pos.second==-1)
	_e.on_text(txt, data);
      else
      {
	_e.on_text(txt.tempSubString(0, pos.first), data);
	_e.on_variable(txt.tempSubString(pos.first+2, pos.second-pos.first-2), data);
	_e.on_text(txt.tempSubString(pos.second+1), data);
      }
    }

    _e.on_eof(data);
  }
  catch (const std::exception &)
  {
    _e.on_error(data);
    throw;
  }
}

