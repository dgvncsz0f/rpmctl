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
#include <cstring>
#include <memory>
#include <popt.h>
#include <unicode/ustream.h>
#include <rpmctl/config.hh>
#include <rpmctl/bdb_environment.hh>
#include <rpmctl/autoptr_malloc_adapter.hh>
#include <rpmctl/stdout_envlist_callback.hh>
#include <rpmctl/ui/router.hh>
#include <rpmctl/ui/get_command.hh>

rpmctl::ui::get_command::get_command()
{}

rpmctl::ui::get_command::~get_command()
{}

std::string rpmctl::ui::get_command::description() const
{
  return("show values of variables");
}

int rpmctl::ui::get_command::exec(rpmctl::ui::input &input, rpmctl::ui::output &output)
{
  char *home=strdup(RPMCTL_DEFAULT_DBHOME), *ns=NULL, *key=NULL;
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autohome(new rpmctl::autoptr_malloc_adapter(home));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autons(new rpmctl::autoptr_malloc_adapter(ns));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autokey(new rpmctl::autoptr_malloc_adapter(key));
  struct poptOption options[] = { { "home",      'h',  POPT_ARG_STRING|POPT_ARGFLAG_SHOW_DEFAULT, &home, 0, "Specify a home directory for the database environment", NULL },
                                  { "namespace", 'n',  POPT_ARG_STRING, &ns,   0, "the package name", NULL },
                                  { "key",       'k',  POPT_ARG_STRING, &key,  0, "The name of the variable. Without this list all keys", NULL },
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
    else if (!output.validates_notnull(ns,  "namespace must not be null"))
    {
      exstatus = EXIT_FAILURE;
    }
    else
    {
      rpmctl::bdb_environment env(home==NULL ? RPMCTL_DEFAULT_DBHOME : home);
      if (key != NULL)
      {
        std::cout << env.get(ns, key, "<<<variable not defined>>>")
                  << std::endl;
      }
      else
      {
        stdout_envlist_callback cc;
        env.list(ns, cc);
      }
    }
  }

  return(exstatus);
}
