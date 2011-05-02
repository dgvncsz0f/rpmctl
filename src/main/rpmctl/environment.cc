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

#include <rpmctl/environment.hh>

rpmctl::environment_list_callback::~environment_list_callback()
{}

rpmctl::environment::~environment()
{}

rpmctl::nil_env::~nil_env()
{}

bool rpmctl::nil_env::has(const UnicodeString &, const UnicodeString &) throw(rpmctl::rpmctl_except)
{ return(false); }

UnicodeString rpmctl::nil_env::get(const UnicodeString &, const UnicodeString &, const UnicodeString &defval) throw(rpmctl::rpmctl_except)
{ return(defval); }

void rpmctl::nil_env::put(const UnicodeString &, const UnicodeString &, const UnicodeString &) throw(rpmctl::rpmctl_except)
{}

void rpmctl::nil_env::list(const UnicodeString &, rpmctl::environment_list_callback &) throw(rpmctl::rpmctl_except)
{}

void rpmctl::nil_env::list(rpmctl::environment_list_callback &) throw(rpmctl::rpmctl_except)
{}
