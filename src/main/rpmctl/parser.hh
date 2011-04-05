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

#include <unicode/unistr.h>

#include <map>

namespace rpmctl
{
  template<typename T>
  class parser_events
  {
  public:
    virtual ~parser_events();

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
     *  \param The variable found
     *  \param The data that `on_start' provided;
     */
    virtual void on_variable(const UnicodeString &, T *) = 0;

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

  class memory_builder : public parser_events<UnicodeString>
  {
  public:
    memory_builder(UnicodeString &, const std::map<UnicodeString,UnicodeString> &);
    virtual ~memory_builder();

    virtual UnicodeString *on_start(const std::string &);
    virtual void on_text(const UnicodeString &, UnicodeString *);
    virtual void on_variable(const UnicodeString &, UnicodeString *);
    virtual void on_eof(UnicodeString *);
    virtual void on_error(UnicodeString *);

  private:
    UnicodeString &_buffer;
    std::map<UnicodeString,UnicodeString> _env;
  };

  template<typename T>
  class parser
  {
  public:
    parser(parser_events<T> &);

    void run(const std::string &);

  private:
    parser_events<T> &_e;
  };
}

#include "parser.ht"

#endif
