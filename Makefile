# Copyright (c) 2011, Diego Souza
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#   * Neither the name of the <ORGANIZATION> nor the names of its contributors
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

BIN_FIND  = $(shell which find 2>/dev/null)
BIN_TEST  = $(shell which test 2>/dev/null)
BIN_TOUCH = $(shell which touch 2>/dev/null)

INC_FILES = $(wildcard src/main/rpmctl/*.hh) $(wildcard src/main/rpmctl/ui/*.hh) 
TPL_FILES = $(wildcard src/main/rpmctl/*.ht)
SRC_FILES = $(wildcard src/main/rpmctl/*.cc) $(wildcard src/main/rpmctl/ui/*.cc)
OBJ_FILES = $(addsuffix .o, $(basename $(SRC_FILES)))

INC_FILES_TEST = $(wildcard src/test/rpmctl_test/*.hh) $(wildcard src/test/rpmctl_test/helpers/*.hh)
SRC_FILES_TEST = $(wildcard src/test/rpmctl_test/*.cc) $(wildcard src/test/rpmctl_test/helpers/*.cc)
OBJ_FILES_TEST = $(addsuffix .o, $(basename $(SRC_FILES_TEST)))

TEST = run_tests
MAIN = rpmctl

DIST = $(CURDIR)/dist

override CPPFLAGS += -W -Wall -pedantic -fPIC -Isrc/main -Isrc/test
override LDFLAGS  += 

compile:
	$(MAKE) __compile_obj

build:
	$(MAKE) __build_main

tests:
	@$(MAKE) __build_test
	@$(DIST)/bin/$(TEST)

ifeq ($(BIN_FIND),)
clean:
	rm -f $(OBJ_FILES) $(OBJ_FILES_TEST)
	rm -rf $(DIST)
else
clean:
	$(BIN_FIND) . -name \*.o -exec rm -f \{\} \;
	rm -f $(OBJ_FILES) $(OBJ_FILES_TEST)
	rm -rf $(DIST)
endif

%/bin/$(TEST): src/test/run_tests.cc $(OBJ_FILES) $(OBJ_FILES_TEST)
	$(BIN_TEST) -d $(dir $(@)) || mkdir -p $(dir $(@))
	$(CXX) $(CPPFLAGS) -o$(@) $(^) $(LDFLAGS)

%/bin/$(MAIN): src/main/rpmctl.cc $(OBJ_FILES)
	$(BIN_TEST) -d $(dir $(@)) || mkdir -p $(dir $(@))
	$(CXX) $(CPPFLAGS) -o$(@) $(^) $(LDFLAGS)

%.cc : %.hh
	$(BIN_TOUCH) $(@)

$(OBJ_FILES): $(TPL_FILES)

__compile_obj: $(OBJ_FILES)

__build_main: LDFLAGS += -licuio -licuuc -ldb_cxx
__build_main: $(DIST)/bin/$(MAIN)

__build_test: CPPFLAGS += -I/usr/include/unittest++
__build_test: LDFLAGS  += -licuio -licuuc -ldb_cxx -lboost_filesystem -lboost_system -lunittest++
__build_test: __compile_obj $(DIST)/bin/$(TEST)

ifeq ($(BIN_TEST),)
  $(error "test binary not found [define one using BIN_TEST variable]")
endif
ifeq ($(BIN_TOUCH),)
  $(error "touch binary not found [define one using BIN_TOUCH variable]")
endif
ifeq ($(CXX),)
  $(error "c++ compiler not found [define one using CXX variable]")
endif
ifeq ($(BIN_FIND),)
  $(warning "find binary not found [define one using BIN_FIND variable]")
endif

.SUFFIXES: .ht

