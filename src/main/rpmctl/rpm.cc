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

#include <rpm/rpmlib.h>
#include <rpm/rpmts.h>
#include <rpm/rpmio.h>
#include <rpm/rpmts.h>
#include <rpm/rpmfi.h>
#ifndef _RPM_4_4_COMPAT
#include <rpm/rpmtag.h>
#endif
#include <archive.h>
#include <archive_entry.h>
#include <rpmctl/rpm.hh>
#include <rpmctl/config.hh>

struct archive_handler
{
  archive_handler(FD_t fh) :
    _fh(fh),
    _buffer(new char[RPMCTL_MAXBUFSZ]),
    _bufsz(RPMCTL_MAXBUFSZ)
  {}

  ~archive_handler()
  {
    Fclose(_fh);
    delete[](_buffer);
  }

  FD_t _fh;
  char *_buffer;
  size_t _bufsz;
};

static
const char *__myheader_get_string(Header h, rpmTag tag)
{
#ifdef _RPM_4_4_COMPAT
    hPTR_t data  = NULL;
    hTYP_t type  = NULL;
    hCNT_t count = NULL;
    if (headerGetEntryMinMemory(h, tag, type, &data, count)==1)
      return(static_cast<const char*>(data));
    else
      return(NULL);
#else
  return(headerGetString(h, tag));
#endif
}

static
FD_t __read_rpm(const std::string &rpm, Header *rpmhdr)
{
  FD_t fd = Fopen(rpm.c_str(), "r.ufdio");
  if (Ferror(fd))
    throw(rpmctl::rpmctl_except(Fstrerror(fd)));

  rpmts ts = rpmtsCreate();
  rpmVSFlags flags = rpmVSFlags(_RPMVSF_NODIGESTS | _RPMVSF_NOSIGNATURES | RPMVSF_NOHDRCHK);
  rpmtsSetVSFlags(ts, flags);
  int rc = rpmReadPackageFile(ts, fd, rpm.c_str(), rpmhdr);
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

  return(fd);
}

static
FD_t __read_payload(const std::string &rpm)
{
  Header h;
  std::string ioflags("r.");
  FD_t fd = __read_rpm(rpm, &h);

  const char *compr = __myheader_get_string(h, RPMTAG_PAYLOADCOMPRESSOR);
  if (compr == NULL)
    ioflags += "gzip";
  else
    ioflags += std::string(compr);
  headerFree(h);

  FD_t gzdi = Fdopen(fd, ioflags.c_str());
  if (gzdi == NULL)
    throw(rpmctl::rpmctl_except(Fstrerror(gzdi)));
  return(gzdi);
}

static
ssize_t __myread(struct archive *, void *data, const void **buff)
{
  archive_handler *handle = static_cast<archive_handler*>(data);
  *buff = handle->_buffer;
  return(Fread(handle->_buffer, sizeof(char), handle->_bufsz, handle->_fh));
}

rpmctl::rpm_read_sink::~rpm_read_sink()
{}

rpmctl::memory_rpm_read_sink::~memory_rpm_read_sink()
{}

std::string rpmctl::memory_rpm_read_sink::string() const
{
  return(_buffer.str());
}

void rpmctl::memory_rpm_read_sink::operator()(const char *buffer, ssize_t n)
{
  _buffer.write(buffer, n);
}

void rpmctl::rpm::init()
{
  rpmReadConfigFiles(NULL, NULL);
}

void rpmctl::rpm::destroy()
{
  rpmFreeRpmrc();
}

rpmctl::rpm::rpm(const std::string &rpm) throw (rpmctl::rpmctl_except) :
  _rpm(rpm)
{
  Fclose(__read_rpm(rpm, &_rpmhdr));
}

rpmctl::rpm::~rpm()
{
  headerFree(_rpmhdr);
}

void rpmctl::rpm::conffiles(std::vector<std::string> &out)
{
#ifdef _RPM_4_4_COMPAT
  rpmfi fi = rpmfiNew(NULL, _rpmhdr, RPMTAG_BASENAMES, 0);
#else
  rpmfi fi = rpmfiNew(NULL, _rpmhdr, RPMTAG_BASENAMES, rpmfiFlags(RPMFI_FLAGS_QUERY));
#endif
  fi = rpmfiInit(fi, 0);
  while (rpmfiNext(fi)!=-1)
  {
    rpmfileAttrs attrs = rpmfileAttrs(rpmfiFFlags(fi));
    if (attrs & RPMFILE_CONFIG)
      out.push_back(rpmfiFN(fi));
  }
  rpmfiFree(fi);
}

void rpmctl::rpm::read_file(const std::string &f, rpm_read_sink &s)
{
  archive_handler handle(__read_payload(_rpm));

  struct archive *a = archive_read_new();
  archive_read_support_compression_all(a);
  archive_read_support_format_all(a);
  archive_read_open(a, &handle, NULL, __myread, NULL);

  struct archive_entry *e;
  while (archive_read_next_header(a, &e) == ARCHIVE_OK) {
    std::string filename = archive_entry_pathname(e);
    if (filename.substr(1) == f)
    {
      char buffer[RPMCTL_MAXBUFSZ];
      ssize_t read = archive_read_data(a, buffer, RPMCTL_MAXBUFSZ);
      s(buffer, read);
    }
    else
      archive_read_data_skip(a);
  }
  archive_read_finish(a);
}
