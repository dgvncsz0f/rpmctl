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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <dirent.h>
#include <cstring>
#include <boost/filesystem/operations.hpp>
#include <rpmctl/file_utils.hh>
#include <rpmctl/scoped_fh.hh>
#include <rpmctl_test/helpers/file_utils.hh>

boost::filesystem::path rpmctl_test::fixtures_path()
{
  const std::string &basedir = rpmctl::file_utils::remove_filename(__FILE__);
  boost::filesystem::path me(boost::filesystem::system_complete(basedir));
  return(me / ".." / ".." / "fixtures");
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

void rpmctl_test::touch(const std::string &file)
{
  close(open(file.c_str(), O_CREAT|O_WRONLY));
}

void rpmctl_test::cleanupdir(const std::string &file)
{
  struct stat x;
  if (stat(file.c_str(), &x)==0)
  {
    if (S_ISDIR(x.st_mode))
    {
      DIR *p = opendir(file.c_str());
      struct dirent *e;
      while ((e=readdir(p))!=NULL)
      {
        if (std::strcmp(e->d_name, ".")!=0 && std::strcmp(e->d_name, "..")!=0 && std::strcmp(e->d_name, ".gitkeep")!=0)
        {
          boost::filesystem::path me(file);
          rpmctl_test::rm_rf((me / e->d_name).string());
        }
      }
      closedir(p);
    }
    else if (S_ISLNK(x.st_mode) || S_ISREG(x.st_mode))
    {
      unlink(file.c_str());
    }
  }
}

void rpmctl_test::rm_rf(const std::string &file)
{
  struct stat x;
  if (stat(file.c_str(), &x)==0)
  {
    if (S_ISDIR(x.st_mode))
    {
      DIR *p = opendir(file.c_str());
      struct dirent *e;
      while ((e=readdir(p))!=NULL)
      {
        if (std::strcmp(e->d_name, ".")!=0 && std::strcmp(e->d_name, "..")!=0 && std::strcmp(e->d_name, ".gitkeep")!=0)
        {
          boost::filesystem::path me(file);
          rpmctl_test::rm_rf((me / e->d_name).string());
        }
      }
      rmdir(file.c_str());
      closedir(p);
    }
    else if (S_ISLNK(x.st_mode) || S_ISREG(x.st_mode))
    {
      unlink(file.c_str());
    }
  }
}
