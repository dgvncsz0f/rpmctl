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
#include <rpmctl/config.hh>
#include <rpmctl/autoptr_array_adapter.hh>
#include <rpmctl/machine.hh>
#include <rpmctl/excepts.hh>
#include <rpmctl/bdb_environment.hh>

static
size_t __serialize(rpmctl::bytestring *buffer, const UnicodeString &ns, const UnicodeString &varname)
{
  size_t nslen      = rpmctl::machine::write(buffer, ns);
  size_t varnamelen = rpmctl::machine::write(buffer==NULL ? NULL : buffer+nslen, varname);
  return(nslen + varnamelen);
}

static
size_t __serialize(rpmctl::bytestring *buffer, const UnicodeString &s)
{
  size_t length = rpmctl::machine::write(buffer, s);
  return(length);
}

static
std::pair<UnicodeString,UnicodeString> __unserialize_tuple(const rpmctl::bytestring *buffer)
{
  UnicodeString ns;
  size_t offset = rpmctl::machine::read_string(ns, buffer);

  UnicodeString varname;
  rpmctl::machine::read_string(varname, buffer+offset);

  return(std::pair<UnicodeString,UnicodeString>(ns, varname));
}

static
UnicodeString __unserialize(const rpmctl::bytestring *buffer)
{
  UnicodeString s;
  rpmctl::machine::read_string(s, buffer);
  return(s);
}

static
int __index_package(Db *, const Dbt *key, const Dbt *, Dbt *skey)
{
  char *buffer = static_cast<char*>(key->get_data());
  UnicodeString ns = __unserialize_tuple(buffer).first;

  int32_t skeylength = __serialize(NULL, ns);
  char *skeybuffer   = static_cast<char*>(std::malloc(skeylength));
  __serialize(skeybuffer, ns);

  skey->set_flags(DB_DBT_APPMALLOC);
  skey->set_data(skeybuffer);
  skey->set_size(skeylength);

  return(0);
}

static
void __close(Dbc *cursor)
{
  if (cursor != NULL)
    cursor->close();
}

rpmctl::bdb_environment::bdb_environment(const std::string &envroot) throw(rpmctl::rpmctl_except) :
  _env(0),
  _master(NULL),
  _secondary(NULL)
{
  try
  {
    _env.open(envroot.c_str(), DB_CREATE | DB_INIT_LOG | DB_INIT_TXN | DB_INIT_MPOOL | DB_RECOVER, 0);
    _env.set_error_stream(&std::cerr);
    _env.set_flags(DB_AUTO_COMMIT, 1);

    _secondary = new Db(&_env, 0);
    _secondary->set_flags(DB_DUPSORT);
    _secondary->open( NULL,
                      "rpmctl_index.db",
                      NULL,
                      DB_BTREE,
                      DB_CREATE,
                      0
                    );

    _master = new Db(&_env, 0);
    _master->open( NULL,
                   "rpmctl.db",
                   NULL,
                   DB_BTREE,
                   DB_CREATE,
                   0
                 );
    _master->associate(NULL, _secondary, __index_package, 0);
  }
  catch (const DbException &e)
  {
    std::string what;
    what += "dbexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
  catch (const std::exception &e)
  {
    std::string what;
    what += "stdexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
}

rpmctl::bdb_environment::~bdb_environment()
{
  if (_secondary != NULL)
    _secondary->close(0);
  if (_master != NULL)
    _master->close(0);

  _env.close(0);

  delete(_secondary);
  delete(_master);
}

void rpmctl::bdb_environment::put(const UnicodeString &ns, const UnicodeString &key, const UnicodeString &val) throw(rpmctl::rpmctl_except)
{
  int32_t keylength = __serialize(NULL, ns, key);
  std::auto_ptr<rpmctl::autoptr_array_adapter<char> > keybuffer(new rpmctl::autoptr_array_adapter<char>(new char[keylength]));
  __serialize(**keybuffer, ns, key);

  int32_t vallength = __serialize(NULL, val);
  std::auto_ptr<rpmctl::autoptr_array_adapter<char> > valbuffer(new rpmctl::autoptr_array_adapter<char>(new char[vallength]));
  __serialize(**valbuffer, val);

  Dbt dbkey(**keybuffer, keylength);
  Dbt dbval(**valbuffer, vallength);
  try
  {
    _master->put(NULL, &dbkey, &dbval, 0);
  }
  catch (const DbException &e)
  {
    std::string what;
    what += "dbexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
}

bool rpmctl::bdb_environment::has(const UnicodeString &ns, const UnicodeString &key) throw(rpmctl::rpmctl_except)
{
  std::auto_ptr<rpmctl::autoptr_array_adapter<char> > keybuffer(NULL);

  int32_t keylength = __serialize(NULL, ns, key);
  keybuffer.reset(new rpmctl::autoptr_array_adapter<char>(new char[keylength]));
  __serialize(**keybuffer, ns, key);

  Dbt dbkey(**keybuffer, keylength);
  Dbt dbval;
  dbval.set_data(NULL);
  dbval.set_ulen(0);
  dbval.set_flags(DB_DBT_USERMEM);

  try
  {
    try
    {
      _master->get(NULL, &dbkey, &dbval, 0);
      return(false);
    }
    catch (const DbMemoryException &)
    {
      return(true);
    }
  }
  catch (const DbException &e)
  {
    std::string what;
    what += "dbexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
}

UnicodeString rpmctl::bdb_environment::get(const UnicodeString &ns, const UnicodeString &key, const UnicodeString &defval) throw(rpmctl::rpmctl_except)
{
  std::auto_ptr<rpmctl::autoptr_array_adapter<char> > keybuffer(NULL);
  std::auto_ptr<rpmctl::autoptr_array_adapter<char> > valbuffer(NULL);

  int32_t keylength = __serialize(NULL, ns, key);
  keybuffer.reset(new rpmctl::autoptr_array_adapter<char>(new char[keylength]));
  __serialize(**keybuffer, ns, key);

  Dbt dbkey(**keybuffer, keylength);
  Dbt dbval;
  dbval.set_data(NULL);
  dbval.set_ulen(0);
  dbval.set_flags(DB_DBT_USERMEM);

  try
  {
    try
    {
      _master->get(NULL, &dbkey, &dbval, 0);
      return(defval);
    }
    catch (const DbMemoryException &)
    {
      u_int32_t vallength = dbval.get_size();
      valbuffer.reset(new rpmctl::autoptr_array_adapter<char>(new char[vallength]));
      dbval.set_ulen(vallength);
      dbval.set_data(**valbuffer);
      _master->get(NULL, &dbkey, &dbval, 0);

      return(__unserialize(**valbuffer));
    }
  }
  catch (const DbException &e)
  {
    std::string what;
    what += "dbexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
}

void rpmctl::bdb_environment::list(const UnicodeString &ns, rpmctl::environment_list_callback &cc) throw(rpmctl::rpmctl_except)
{
  std::auto_ptr<rpmctl::autoptr_array_adapter<char> > keybuffer(NULL);
  int32_t keylength = __serialize(NULL, ns);
  keybuffer.reset(new rpmctl::autoptr_array_adapter<char>(new char[keylength]));
  __serialize(**keybuffer, ns);

  Dbt dbkey(**keybuffer, keylength);
  Dbt dbpval, dbpkey;

  Dbc *cursor = NULL;
  try
  {
    _secondary->cursor(NULL, &cursor, 0);

    int ret = cursor->pget(&dbkey, &dbpkey, &dbpval, DB_SET);
    while (ret != DB_NOTFOUND)
    {
      char *buffer;
      buffer = static_cast<char*>(dbpkey.get_data());
      UnicodeString key = __unserialize_tuple(buffer).second;
      buffer = static_cast<char*>(dbpval.get_data());
      UnicodeString val = __unserialize(buffer);
      cc(ns, key, val);
      ret = cursor->pget(&dbkey, &dbpkey, &dbpval, DB_NEXT_DUP);
    }
  }
  catch (const DbException &e)
  {
    __close(cursor);
    std::string what;
    what += "dbexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
  __close(cursor);

}
