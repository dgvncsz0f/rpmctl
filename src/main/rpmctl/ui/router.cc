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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <cstring>
#include <sstream>
#include <string>
#include <popt.h>
#include <rpmctl/autoptr_array_adapter.hh>
#include <rpmctl/autoptr_malloc_adapter.hh>
#include <rpmctl/ui/router.hh>
#include <rpmctl/ui/command.hh>

static
int __without_globals(const char **newargv, int argc, const char **argv)
{
  int newargc=0;
  for (int k=0; k<argc; k+=1)
  {
    if (std::strcmp(argv[k], "--help")!=0 && std::strcmp(argv[k], "--version")!=0)
      newargv[newargc++] = argv[k];
  }
  return(newargc);
}

rpmctl::ui::input::input(int argc, const char **argv) :
  _argv(argv),
  _argc(argc),
  _wanthelp(false)
{}

void rpmctl::ui::input::want_help(bool v)
{
  _wanthelp = v;
}

bool rpmctl::ui::input::want_help() const
{
  return(_wanthelp);
}

const char **rpmctl::ui::input::argv() const
{
  return(_argv);
}

int rpmctl::ui::input::argc() const
{
  return(_argc);
}

rpmctl::ui::output::output(struct poptOption *opts, const std::string &progname) :
  _globalopts(opts),
  _progname(progname),
  _cmdname("<command>")
{}

rpmctl::ui::output::output(struct poptOption *opts, const std::string &progname, const std::string &cmdname) :
  _globalopts(opts),
  _progname(progname),
  _cmdname(cmdname)
{}

void rpmctl::ui::output::print_error(const std::string &msg)
{
  std::cerr << _progname 
            << ": "
            << msg 
            << std::endl;
}

void rpmctl::ui::output::print_error(poptContext ctx, int rc)
{
  std::ostringstream message;
  message << poptBadOption(ctx, POPT_BADOPTION_NOALIAS)
          << ": "
          << poptStrerror(rc);
  print_error(message.str());
}

void rpmctl::ui::output::print_help()
{
  std::string help_message;
  help_message += "[OPTION...] "+ _cmdname +" [ARG...]";

  struct poptOption myoptions[] = { { NULL, '\0', POPT_ARG_INCLUDE_TABLE, _globalopts, 0, NULL, NULL },
                                    POPT_TABLEEND
                                  };

  const char *argv[] = { _progname.c_str(), NULL };
  poptContext optctx = poptGetContext(argv[0], 1, argv, myoptions, 0);
  poptSetOtherOptionHelp(optctx, help_message.c_str());
  poptPrintHelp(optctx, stderr, 0);
  poptFreeContext(optctx);
}

void rpmctl::ui::output::print_help(struct poptOption *options)
{
  std::string help_message;
  help_message += "[OPTION...] "+ _cmdname +" [ARG...]";

  struct poptOption myoptions[] = { { NULL, '\0', POPT_ARG_INCLUDE_TABLE, options,     0, NULL,              NULL },
                                    { NULL, '\0', POPT_ARG_INCLUDE_TABLE, _globalopts, 0, "Global options:", NULL },
                                    POPT_TABLEEND
                                  };

  const char *argv[] = { _progname.c_str(), NULL };
  poptContext optctx = poptGetContext(argv[0], 1, argv, myoptions, 0);
  poptSetOtherOptionHelp(optctx, help_message.c_str());
  poptPrintHelp(optctx, stderr, 0);
  poptFreeContext(optctx);
}

rpmctl::ui::router::router()
{}

rpmctl::ui::router::~router()
{}

void rpmctl::ui::router::bind(const std::string &name, rpmctl::ui::command *command)
{
  _table[name] = command;
}

int rpmctl::ui::router::route(int argc, const char **argv)
{
  int version=0, help=0;
  struct poptOption options[] = { { "help",    '\0', POPT_ARG_NONE, &help,     0, "this message",                         NULL },
                                  { "version", '\0', POPT_ARG_NONE, &version,  0, "version and license of this software", NULL },
                                  POPT_TABLEEND
                                };

  int rc=0;
  poptContext optctx = poptGetContext(argv[0], argc, argv, options, 0);
  while ((rc=poptGetNextOpt(optctx)) > 0);

  int exstatus=EXIT_SUCCESS;
  const char *cmdstr = poptGetArg(optctx);
  std::map<std::string, rpmctl::ui::command*>::const_iterator it = _table.find(cmdstr==NULL ? "" : cmdstr);
  if (it!=_table.end())
  {
    std::auto_ptr<autoptr_malloc_adapter> auto_newargv(new autoptr_malloc_adapter(std::malloc(sizeof(char*) * argc)));
    const char **newargv = static_cast<const char**>(**auto_newargv);
    int newargc = __without_globals(newargv, argc, argv);
    
    rpmctl::ui::input input(newargc, newargv);
    rpmctl::ui::output output(options, argv[0], cmdstr);
    rpmctl::ui::command *command = it->second;
    
    input.want_help(help!=0);
    exstatus = command->exec(input, output);
  }
  else
  {
    rpmctl::ui::output output(options, argv[0]);
    if (rc == -1)
      output.print_help();
    else
      output.print_error(optctx, rc);
  }

  poptFreeContext(optctx);
  return(exstatus);
}
