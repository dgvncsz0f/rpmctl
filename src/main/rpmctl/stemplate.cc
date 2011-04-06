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
#include <cstdio>
#include <unicode/ustdio.h>
#include <rpmctl/stemplate.hh>
#include <rpmctl/scoped_tmpfh.hh>

static
void __exfwrite(UFILE *fh, const UnicodeString &txt)
{
  int32_t length = txt.length();
  while (length > 0)
  {
    int32_t offset = txt.length() - length;
    const UChar *buffer = txt.getBuffer();
    length -= u_file_write(buffer + offset, length, fh);
  }
}

rpmctl::stemplate_handler::stemplate_handler(const std::string cfgfile) :
  _cfgfile(cfgfile),
  _tmpfh()
{}

rpmctl::stemplate::stemplate(environment &e) :
  _e(e)
{}

rpmctl::stemplate::~stemplate()
{}

rpmctl::stemplate_handler *rpmctl::stemplate::on_start(const std::string &cfgfile)
{
  return(new stemplate_handler(cfgfile));
}

void rpmctl::stemplate::on_text(const UnicodeString &txt, stemplate_handler *data)
{
  __exfwrite(*(data->_tmpfh), txt);
}

void rpmctl::stemplate::on_variable(const UnicodeString &ns, const UnicodeString &key, stemplate_handler *data)
{
  UnicodeString rawvar = "$(" + ns + "::" + key +")";
  __exfwrite(*(data->_tmpfh), _e.get(ns, key, rawvar));
}

void rpmctl::stemplate::on_eof(stemplate_handler *data)
{
  const std::string &tmpfile = data->_tmpfh.tmpfile();
  const std::string &cfgfile = data->_cfgfile;
  if (std::rename(tmpfile.c_str(), cfgfile.c_str()) != 0)
    throw(std::runtime_error("could not move file "+ tmpfile +"to its destination: " + cfgfile));
  delete(data);
}

void rpmctl::stemplate::on_error(stemplate_handler *data)
{
  delete(data);
}
