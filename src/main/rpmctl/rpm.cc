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

#include <rpm/rpmtag.h>
#include <rpm/rpmio.h>
#include <rpm/rpmts.h>
#include <archive.h>
#include <rpmctl/rpm.hh>

void rpmctl::rpm::init()
{
  rpmReadConfigFiles(NULL, NULL);
}

rpmctl::rpm::rpm(const std::string &rpm) throw (rpmctl::rpmctl_except) :
  _rpm(rpm)
{
  FD_t fd = Fopen(rpm.c_str(), "r.ufdio");
  if (Ferror(fd))
    throw(rpmctl::rpmctl_except(Fstrerror(fd)));

  rpmts ts = rpmtsCreate();
  rpmVSFlags flags = rpmVSFlags(_RPMVSF_NODIGESTS | _RPMVSF_NOSIGNATURES | RPMVSF_NOHDRCHK);
  rpmtsSetVSFlags(ts, flags);
  int rc = rpmReadPackageFile(ts, fd, rpm.c_str(), &_rpmhdr);
  Fclose(fd);
  rpmtsFree(ts);

  switch (rc)
  {
  case RPMRC_OK:
  case RPMRC_NOKEY:
  case RPMRC_NOTTRUSTED:
    break;
  case RPMRC_NOTFOUND:
    throw(rpmctl::rpmctl_except("file is not an RPM package"));
  case RPMRC_FAIL:
  default:
    throw(rpmctl::rpmctl_except("error reading header from package"));
  }
}

rpmctl::rpm::~rpm()
{
  headerFree(_rpmhdr);
}

void rpmctl::rpm::conffiles(std::vector<std::string> &out)
{
  rpmfi fi = rpmfiNew(NULL, _rpmhdr, RPMTAG_BASENAMES, rpmfiFlags(RPMFI_FLAGS_QUERY));
  fi = rpmfiInit(fi, 0);
  while (rpmfiNext(fi)!=-1)
  {
    rpmfileAttrs attrs = rpmfiFFlags(fi);
    if (attrs & RPMFILE_CONFIG)
      out.push_back(rpmfiFN(fi));
  }
  rpmfiFree(fi);
}

void rpmctl::rpm::read_file(const std::string &, rpm_read_sink &)
{
  throw(std::runtime_error("TODO:fixme"));
}
