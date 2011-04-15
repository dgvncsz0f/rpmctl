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

#ifndef __RPMCTL_UI_ROUTER_HH__
#define __RPMCTL_UI_ROUTER_HH__

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <popt.h>

namespace rpmctl
{

  namespace ui
  {
    class command;


    class input
    {
    public:
      input(int, const char **);

      void want_help(bool );
      bool want_help() const;

      int argc() const;
      const char **argv() const;

    private:
      const char **_argv;
      int _argc;
      bool _wanthelp;
    };

    class output
    {
    public:
      output(struct poptOption *, const std::string &);
      output(struct poptOption *, const std::string &, const std::string &);

      void print_help();
      void print_help(const std::vector<std::string> &);
      void print_help(struct poptOption *);

      bool validates_notnull(void *, const std::string &);

      void print_error(const std::string &);
      void print_error(poptContext, int);

      const std::string &progname() const;
      const std::string &cmdname() const;

    private:
      struct poptOption *_globalopts;
      std::string _progname;
      std::string _cmdname;
    };

    class router
    {
    public:
      router();
      ~router();

      /*! Bind a given command to a name in the CLI.
       *
       * \param The name by which users can invoke this command;
       * \param The command itself.
       */
      void bind(const std::string &, command *);

      /*! The main routine. It parses the command line arguments and
       *  invoke the proper command.
       * 
       *  \param The number of arguments, as provided to the main function;
       *  \param The arguments (sized according argc) as provided to the main function;
       *  \return 0=success; failure if something else.
       */
      int route(int argc, const char **argv);

      /*! The commands available in this router so far.
       */
      std::vector<std::string> commands() const;

    private:
      std::map<std::string, command *> _table;
    };

  }

}

#endif
