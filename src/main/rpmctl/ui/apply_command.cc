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
#include <stdexcept>
#include <unicode/ustream.h>
#include <rpmctl/config.hh>
#include <rpmctl/rpm.hh>
#include <rpmctl/rpm_read_sinks.hh>
#include <rpmctl/bdb_environment.hh>
#include <rpmctl/stemplate.hh>
#include <rpmctl/file_utils.hh>
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
  char *home=strdup(RPMCTL_DEFAULT_DBHOME), *ns=NULL, *file=NULL, *package=NULL;
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autohome(new rpmctl::autoptr_malloc_adapter(home));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autons(new rpmctl::autoptr_malloc_adapter(ns));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autofile(new rpmctl::autoptr_malloc_adapter(file));
  std::auto_ptr<rpmctl::autoptr_malloc_adapter> autopkg(new rpmctl::autoptr_malloc_adapter(package));
  std::auto_ptr<rpmctl::rpm> autorpm(NULL);

  struct poptOption options[] = { { "home",      'h',  POPT_ARG_STRING|POPT_ARGFLAG_SHOW_DEFAULT, &home,    0, "Specify a home directory for the database environment", NULL },
                                  { "namespace", 'n',  POPT_ARG_STRING,                           &ns,      0, "the default namespace for unqualified variables", NULL },
                                  { "package",   'p',  POPT_ARG_STRING,                           &package, 0, "the filename of the RPM package to read original files from", NULL },
                                  { "file",      'f',  POPT_ARG_STRING,                           &file,    0, "The name of a file to perform variable expansion", NULL },
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

    if (package != NULL)
      autorpm.reset(new rpmctl::rpm(package));

    if (rc<-1)
    {
      output.print_error(optctx, rc);
    }
    else if (!output.validates_notnull(file, "file must not be null"))
    {
      exstatus = EXIT_FAILURE;
    }
    else if (ns==NULL && package==NULL)
    {
      output.print_error("either --package or --namespace must be provided");
      exstatus = EXIT_FAILURE;
    }
    else if (ns!=NULL && package!=NULL)
    {
      if ((*autorpm).name() != std::string(ns))
      {
        output.print_error("--namespace must match the name declared in the provided RPM package");
        exstatus = EXIT_FAILURE;
      }
    }
    else
    {
      std::string mynamespace = (ns==NULL ? (*autorpm).name() : std::string(ns));
      rpmctl::bdb_environment env(home==NULL ? "/var/lib/rpmctl" : home);
      rpmctl::stemplate engine(env);
      rpmctl::parser<stemplate_handler> parser(engine, mynamespace.c_str());
      
      const std::string &basedir = rpmctl::file_utils::remove_filename(file);
      rpmctl::scoped_tmpfh tmpfile(basedir);
      if (package!=NULL)
      {
        rpmctl::file_sink fsink(tmpfile.tmpfile());
        (*autorpm).read_file(file, fsink);
        parser.run(tmpfile.tmpfile());
        rpmctl::file_utils::move(tmpfile.tmpfile(), file);
        (*autorpm).set_perms(file);
      }
      else
      {
        parser.run(file);
      }
    }
  }   
  return(exstatus);
}
