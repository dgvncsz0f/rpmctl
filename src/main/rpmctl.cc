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

#include <unicode/uclean.h>
#include <rpmctl/rpm.hh>
#include <rpmctl/ui/command.hh>
#include <rpmctl/ui/router.hh>
#include <rpmctl/ui/put_command.hh>
#include <rpmctl/ui/def_command.hh>
#include <rpmctl/ui/get_command.hh>
#include <rpmctl/ui/apply_command.hh>

int main(int argc, const char **argv)
{
  rpmctl::rpm::init();

  rpmctl::ui::put_command put_command;
  rpmctl::ui::def_command def_command;
  rpmctl::ui::get_command get_command;
  rpmctl::ui::apply_command apply_command;
  rpmctl::ui::router router;
  router.bind("put", &put_command);
  router.bind("def", &def_command);
  router.bind("get", &get_command);
  router.bind("apply", &apply_command);
  int exstatus = router.route(argc, argv);

  u_cleanup();
  rpmctl::rpm::destroy();

  return(exstatus);
}

