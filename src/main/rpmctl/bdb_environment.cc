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
#include <iostream>
#include <rpmctl/excepts.hh>
#include <rpmctl/bdb_environment.hh>

class myautoptr_adapter
{
public:
  myautoptr_adapter(char *array) :
    _array(array)
  {}
  ~myautoptr_adapter()
  {
    delete[](_array);
  }

  char *operator*() const
  {
    return(_array);
  }

private:
  char *_array;
};

rpmctl::bdb_environment::bdb_environment(const std::string &envroot) throw(rpmctl::rpmctl_except) :
  _env(0),
  _db(NULL)
{
  try
  {
    _env.open(envroot.c_str(), DB_CREATE | DB_INIT_LOG | DB_INIT_TXN | DB_INIT_MPOOL | DB_RECOVER, 0);
    _env.set_error_stream(&std::cerr);

    _db = new Db(&_env, 0);
    _db->open(NULL, "rpmctl.db", NULL, DB_BTREE, DB_CREATE | DB_AUTO_COMMIT, 0);
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
  _db->close(0);
  _env.close(0);
  delete(_db);
}

void rpmctl::bdb_environment::put(const UnicodeString &key, const UnicodeString &val) throw(rpmctl::rpmctl_except)
{
  int32_t keylength = key.extract(0, key.length(), NULL, "UTF-8");
  std::auto_ptr<myautoptr_adapter> keybuffer(new myautoptr_adapter(new char[keylength+1]));
  key.extract(0, key.length(), **keybuffer, "UTF-8");

  int32_t vallength = val.extract(0, val.length(), NULL, "UTF-8");
  std::auto_ptr<myautoptr_adapter> valbuffer(new myautoptr_adapter(new char[vallength+1]));
  val.extract(0, val.length(), **valbuffer, "UTF-8");

  Dbt dbkey(**keybuffer, keylength);
  Dbt dbval(**valbuffer, vallength);
  try
  {
    _db->put(NULL, &dbkey, &dbval, DB_OVERWRITE_DUP);
  }
  catch (const DbException &e)
  {
    std::string what;
    what += "dbexception: ";
    what += e.what();
    throw(rpmctl::rpmctl_except(what));
  }
}

UnicodeString rpmctl::bdb_environment::get(const UnicodeString &key, const UnicodeString &defval) throw(rpmctl::rpmctl_except)
{
  std::auto_ptr<myautoptr_adapter> keybuffer(NULL);
  std::auto_ptr<myautoptr_adapter> valbuffer(NULL);

  int32_t keylength = key.extract(0, key.length(), NULL, "UTF-8");
  keybuffer.reset(new myautoptr_adapter(new char[keylength+1]));
  key.extract(0, key.length(), **keybuffer, "UTF-8");

  Dbt dbkey(**keybuffer, keylength);
  Dbt dbval;
  dbval.set_data(NULL);
  dbval.set_ulen(0);
  dbval.set_flags(DB_DBT_USERMEM);

  try
  {
    try
    {
      _db->get(NULL, &dbkey, &dbval, 0);
      return(defval);
    }
    catch (const DbMemoryException &)
    {
      u_int32_t vallength = dbval.get_size();
      valbuffer.reset(new myautoptr_adapter(new char[vallength]));
      dbval.set_ulen(vallength);
      dbval.set_data(**valbuffer);
      _db->get(NULL, &dbkey, &dbval, 0);
      
      UnicodeString retval(**valbuffer, vallength, "UTF-8");
      return(retval);
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
