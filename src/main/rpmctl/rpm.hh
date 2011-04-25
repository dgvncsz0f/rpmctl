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

#ifndef __RPMCTL_RPM_HH__
#define __RPMCTL_RPM_HH__

#include <cstdlib>
#include <string>
#include <vector>
#include <rpm/rpmlib.h>
#include <rpmctl/excepts.hh>

namespace rpmctl
{
  class rpm;

  class rpm_read_sink
  {
  public:
    virtual ~rpm_read_sink();

    /*! Gets a given amount of data.
     *
     * \param Bytes read from file;
     * \param Number os bytes read;
     */
    virtual void operator()(const char *, ssize_t) = 0;

    /*! Called when RPM has finished reading the file.
     */
    virtual void eof() = 0;

    /*! Same as eof() but this one takes the rpm class that was used
     *  to read the file.
     */
    virtual void eof(rpm &);
  };

  class rpm
  {
  public:
    /*! Initializes the rpm engine (rpmReadConfigFile).
     */
    static
    void init();

    /*! Releases all memory required by init method.
     */
    static
    void destroy();

    rpm(const std::string &) throw (rpmctl_except);

    ~rpm();

    /*! The name declared in this package
     */
    std::string name();

    /*! Extracts all config files declared in the package provided in
     *  the ctor of this class.
     *
     *  \param The container that will take the config file names;
     */
    void conffiles(std::vector<std::string> &);
    
    /*! Reads the content of a file inside a RPM package invoking
     *  rpm_read_file_callback repeatedly until the contents of the
     *  file are consumed.
     *
     *  \param The file you want to read from the RPM package;
     *  \param The object that will handle the contents of the file;
     */
    void read_file(const std::string &file, rpm_read_sink &) throw (rpmctl_except);

    /*! Reset the file permissions of a given file.
     */
    void set_perms(const std::string &file) throw (rpmctl_except);

  private:
    const std::string _rpm;
    Header _rpmhdr;
  };

}

#endif
