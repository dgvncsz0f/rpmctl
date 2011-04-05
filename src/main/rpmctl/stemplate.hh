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

#ifndef __RPMCTL_STEMPLATE_HH__
#define __RPMCTL_STEMPLATE_HH__

#include <unicode/unistr.h>
#include <rpmctl/parser.hh>
#include <rpmctl/environment.hh>
#include <rpmctl/scoped_fh.hh>
#include <rpmctl/scoped_tmpfh.hh>

namespace rpmctl
{

  struct stemplate_handler
  {
    stemplate_handler(const std::string cfgfile);
    
    std::string _cfgfile;
    scoped_tmpfh _tmpfh;
  };

  class stemplate : public parser_events<stemplate_handler>
  {
  public:
    stemplate(environment &);
    virtual ~stemplate();

    virtual stemplate_handler *on_start(const std::string &filename);

    virtual void on_text(const UnicodeString &, stemplate_handler *);

    virtual void on_variable(const UnicodeString &, const UnicodeString &, stemplate_handler *);

    virtual void on_eof(stemplate_handler *);

    virtual void on_error(stemplate_handler *);

  private:
    environment &_e;
  };
}

#endif
