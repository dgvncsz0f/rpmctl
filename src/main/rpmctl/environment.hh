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

#ifndef __RPMCTL_ENVIRONMENT_HH__
#define __RPMCTL_ENVIRONMENT_HH__

#include <unicode/unistr.h>
#include <rpmctl/excepts.hh>

namespace rpmctl
{

  class environment_list_callback
  {
  public:
    virtual ~environment_list_callback();

    virtual void operator()(const UnicodeString &ns, const UnicodeString &key, const UnicodeString &val) = 0;
  };

  class environment
  {
  public:
    virtual ~environment();

    virtual bool has(const UnicodeString &, const UnicodeString &key) throw(rpmctl_except) = 0;

    virtual UnicodeString get(const UnicodeString &ns, const UnicodeString &key, const UnicodeString &defval) throw(rpmctl_except) = 0;

    virtual void put(const UnicodeString &ns, const UnicodeString &key, const UnicodeString &val) throw(rpmctl_except) = 0;

    virtual void list(const UnicodeString &ns, environment_list_callback &) throw(rpmctl_except) = 0;
  };

  class nil_env : public environment
  {
  public:
    virtual ~nil_env();

    virtual bool has(const UnicodeString &, const UnicodeString &) throw(rpmctl_except);

    virtual UnicodeString get(const UnicodeString &, const UnicodeString &, const UnicodeString &) throw(rpmctl_except);

    virtual void put(const UnicodeString &, const UnicodeString &, const UnicodeString &) throw(rpmctl_except);

    virtual void list(const UnicodeString &, environment_list_callback &) throw(rpmctl_except);
  };
}

#endif
