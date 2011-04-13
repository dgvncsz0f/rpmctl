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

#include <memory>
#include <popt.h>
#include <rpmctl/autoptr_array_adapter.hh>
#include <rpmctl/ui/router.hh>
#include <rpmctl/ui/command.hh>

static
void __version_flag(rpmctl::ui::option_args *options, int *arg)
{
  rpmctl::ui::option_args option = { "version",
                                     '\0',
                                     POPT_ARG_NONE,
                                     arg,
                                     0,
                                     "version and license of this software",
                                     NULL
                                   };
  options[0] = option;
}

static
void __popt_add_table(rpmctl::ui::option_args *options, rpmctl::ui::option_args *help_options)
{
  rpmctl::ui::option_args option = { NULL,
                                     '\0',
                                     POPT_ARG_INCLUDE_TABLE,
                                     help_options,
                                     0,
                                     "Help options:",
                                     NULL
                                   };
  options[0] = option;
}

static
void __popt_end(rpmctl::ui::option_args *options)
{
  rpmctl::ui::option_args option = { NULL, '\0', 0, NULL, 0, NULL, NULL };
  options[0] = option;
}

static
void __print_help(poptContext ctx)
{
  poptPrintHelp(ctx, stderr, 0);
}

rpmctl::ui::router::router()
{}

rpmctl::ui::router::~router()
{}

void rpmctl::ui::router::bind(rpmctl::ui::command *command)
{
  command->visit(*this);
}

std::vector<rpmctl::ui::option_args*> &rpmctl::ui::router::options()
{
  return(_options);
}

rpmctl::ui::command *rpmctl::ui::router::lookup(int argc, const char *argv[])
{
  int version=0, help=0;
  int rc;
  rpmctl::ui::option_args *options_   = new rpmctl::ui::option_args[_options.size() + 3];
  rpmctl::ui::option_args help_opts[] = { { "help", 'h', POPT_ARG_NONE, &help, 0, "this message", NULL },
                                            POPT_TABLEEND
                                        };

  rpmctl::autoptr_array_adapter<rpmctl::ui::option_args> *adapter = new rpmctl::autoptr_array_adapter<rpmctl::ui::option_args>(options_);
  std::auto_ptr<rpmctl::autoptr_array_adapter<rpmctl::ui::option_args> > options(adapter);

  __version_flag(**options, &version);
  __popt_add_table((**options)+_options.size()+1, help_opts);
  __popt_end((**options)+_options.size()+2);

  for (unsigned int i=0; i<_options.size(); i+=1)
  {
    __popt_add_table((**options)+i+1, _options[i]);
  }

  poptContext optctx = poptGetContext(argv[0], argc, argv, **options, 0);
  poptSetOtherOptionHelp(optctx, "[OPTION...] <command> [ARG...]");

  while ((rc=poptGetNextOpt(optctx)) > 0);
  if (help || argc<2)
  {
    __print_help(optctx);
  }

  poptFreeContext(optctx);
  return(NULL);
}
