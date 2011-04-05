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

#ifndef __RPMCTL_PARSER_HH__
#define __RPMCTL_PARSER_HH__

#include <stdexcept>
#include <stdlib.h>
#include <unicode/unistr.h>
#include <unicode/ustdio.h>
#include <unicode/ustream.h>
#include <rpmctl/config.hh>
#include <rpmctl/parser.hh>
#include <rpmctl/scoped_fh.hh>

static
int32_t __exread(UnicodeString &out, UFILE *fh, size_t bufsz)
{
  UChar *buffer = new UChar[bufsz];
  int32_t length = u_file_read(buffer, bufsz, fh);
  if (length == -1)
  {
    delete[](buffer);
    throw(std::runtime_error("error reading file"));
  }
  out.append(UnicodeString(buffer, length));
  delete[](buffer);
  return(length);
}

namespace rpmctl
{
  template<typename T>
  class parser_events
  {
  public:
    virtual ~parser_events()
    {}

    /*! This event is invoked when the parsing is started. The data
     *  this function returns in here is provided to all subsequent
     *  events.
     *
     *  \return An opaque value that is provided to all events invoked
     *  by the parser.
     */
    virtual T *on_start(const std::string &) = 0;

    /*! This event is invoked when regular text is found in the config
     *  file.
     *
     *  \param The text found.
     *  \param The data that `on_start' provided;
     */
    virtual void on_text(const UnicodeString &, T *) = 0;

    /*! This event is invoked when a variable is found in a config
     *  file.
     *
     *  \param The prefix for this variable (usually the package name);  
     *  \param The variable found;
     *  \param The data that `on_start' provided;
     */
    virtual void on_variable(const UnicodeString &, const UnicodeString &, T *) = 0;

    /*! This event is invoked when the parser finishes reading the
     *  file successfully.
     *  
     *  \param The data that `on_start' provided;
     */ 
    virtual void on_eof(T *) = 0;

    /*! This is called if there was any error instead of on_eof.
     *
     * \param The data that `on_start' provided;
     */
    virtual void on_error(T *) = 0;
  };

  template<typename T>
  class parser
  {
  public:
    parser(parser_events<T> &e, const UnicodeString &prefix) :
      _e(e),
      _prefix(prefix)
    {}

    void run(const std::string &file)
    {
      T *data = _e.on_start(file);

      try
      {
	rpmctl::scoped_fh fh(file, "r");

	UnicodeString txt;
	while (!u_feof(*fh))
	{
	  __exread(txt, *fh, RPMCTL_MAXBUFSZ);
	  do
	  {
	    int32_t n = txt.indexOf("$(");
	    if (n==-1)
	    {
	      _e.on_text(txt.tempSubString(0, txt.length()-1), data);
	      txt.remove(0, txt.length()-1);
	      break;
	    }
	    else
	    {
	      int32_t m = txt.indexOf(")", n);
	      if (m!=-1)
	      {
		_e.on_text(txt.tempSubString(0, n), data);
		_e.on_variable(_prefix, txt.tempSubString(n+2, m-n-2), data);
		txt.remove(0, m+1);
	      }
	      else
	      {
		_e.on_text(txt.tempSubString(0, n), data);
		txt.remove(0, n);
		break;
	      }
	    }
	  } while (true);
	}

	_e.on_text(txt, data);
	_e.on_eof(data);
      }
      catch (const std::exception &)
      {
	_e.on_error(data);
	throw;
      }
    }

  private:
    parser_events<T> &_e;
    const UnicodeString &_prefix;
  };
}

#endif
