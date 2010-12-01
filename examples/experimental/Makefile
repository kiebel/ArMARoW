################################################################################
##
## Copyright (c) 2010-2011 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
##               2010-2011 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
## All rights reserved.
##
##    Redistribution and use in source and binary forms, with or without
##    modification, are permitted provided that the following conditions
##    are met:
##
##    * Redistributions of source code must retain the above copyright
##      notice, this list of conditions and the following disclaimer.
##
##    * Redistributions in binary form must reproduce the above copyright
##      notice, this list of conditions and the following disclaimer in
##      the documentation and/or other materials provided with the
##      distribution.
##
##    * Neither the name of the copyright holders nor the names of
##      contributors may be used to endorse or promote products derived
##      from this software without specific prior written permission.
##
##
##    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
##    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
##    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
##    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
##    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
##    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
##    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
##    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
##    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##
## $Id$
##
################################################################################
include ../../platform/Makefile

BOOST_DIR  ?= /usr/include/boost
BOOST_LINK  = ./include/boost 

ADDITIONAL_INCLUDE +=  -I$(ARMAROWDIR)/examples/experimental/include -I/usr/include/boost/compatibility/cpp_c_headers
LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)
CXXFLAGS+= -ggdb3 -O0 -UNDEBUG -D__NO_STL__ -DBOOST_NO_STDLIB_CONFIG

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))

.phony: all boost clean

all: boost
	@echo usage: \"make programm-to-build\"

boost:
	@mkdir -p $(dir ${BOOST_LINK})
	@ln -Tfs ${BOOST_DIR} ${BOOST_LINK}

clean:
	@rm -rf $(filtered)
