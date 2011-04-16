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
#include <memory>
#include <popt.h>
#include <stdexcept>
#include <unicode/ustream.h>
#include <rpmctl/bdb_environment.hh>
#include <rpmctl/stemplate.hh>
#include <rpmctl/parser.hh>
#include <rpmctl/autoptr_malloc_adapter.hh>
#include <rpmctl/ui/router.hh>
#include <rpmctl/ui/apply_command.hh>

rpmctl::ui::apply_command::apply_command()
{}

rpmctl::ui::apply_command::~apply_command()
{}

std::string rpmctl::ui::apply_command::description() const
{
  return("read the config file(s) and performs variable expansion");
}

int rpmctl::ui::apply_command::exec(rpmctl::ui::input &input, rpmctl::ui::output &output)
{
  char *home=NULL, *package=NULL, *file=NULL;
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autohome(new rpmctl::autoptr_malloc_adapter(home));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autopkg(new rpmctl::autoptr_malloc_adapter(package));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autofile(new rpmctl::autoptr_malloc_adapter(file));
  struct poptOption options[] = { { "home",      'h',  POPT_ARG_STRING, &home,    0, "Specify a home directory for the database environment [default=/var/lib/rpmctl]", NULL },
                                  { "package",   'p',  POPT_ARG_STRING, &package, 0, "the name of a rpm package", NULL },
                                  { "file",      'f',  POPT_ARG_STRING, &file,    0, "The name of a file", NULL },
                                  POPT_TABLEEND
                                };
  
  int exstatus = EXIT_SUCCESS;
  if (input.want_help())
    output.print_help(options);
  else
  {
    int rc=0;
    poptContext optctx = poptGetContext(input.argv()[0], input.argc(), input.argv(), options, 0);
    while ((rc=poptGetNextOpt(optctx)) > 0);
    if (rc<-1)
    {
      output.print_error(optctx, rc);
    }
    else if (!output.validates_notnull(package, "package must not be null"))
    {
      exstatus = EXIT_FAILURE;
    }
    else
    {
      rpmctl::bdb_environment env(home==NULL ? "/var/lib/rpmctl" : home);
      if (file != NULL)
      {
        rpmctl::stemplate engine(env);
        rpmctl::parser<stemplate_handler> parser(engine, package);
        parser.run(file);
      }
      else
      {
        throw(std::runtime_error("unsupported operation exception"));
      }
    }
  }

  return(exstatus);
}