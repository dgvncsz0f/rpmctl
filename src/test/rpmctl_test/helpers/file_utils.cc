/* Copyright (c) 2011, Diego Souza
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

#include <sys/stat.h>
#include <rpmctl/scoped_fh.hh>
#include <rpmctl_test/helpers/file_utils.hh>

boost::filesystem::path rpmctl_test::fixtures_path()
{
  boost::filesystem::path me(boost::filesystem::system_complete(__FILE__));
  return(me.remove_filename() / ".." / ".." / "fixtures");
}

UnicodeString rpmctl_test::read_file(const std::string &file)
{
  UChar buffer[1024];
  UnicodeString txt;
  rpmctl::scoped_fh fh(file, "r");
  while (! u_feof(*fh))
  {
    int32_t length = u_file_read(buffer, sizeof(buffer), *fh);
    txt.append(UnicodeString(buffer, length));
  }
  return(txt);
}

bool rpmctl_test::file_exists(const std::string &file)
{
  struct stat x;
  return(stat(file.c_str(), &x) == 0);
}
