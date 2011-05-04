################################################################################
##
## Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
##               2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
#TODO PLATFORM = default
PLATFORM = ICradioStick
################################################################################
#ADDITIONAL_INCLUDE += -DBOOST_NO_STDLIB_CONFIG -D__NO_STL__ -I../../external/avr-halib/externals/include -I../../external/avr-halib/externals/include/boost/compatibility/cpp_c_headers/
include ../../platform/Makefile

LDFLAGS= $(LIB) $(ADDITIONAL_LIBS)
CXXFLAGS+= -ggdb3 -Os -UNDEBUG

filtered=$(filter-out %.cc %.h %.py %Makefile,$(shell find . -maxdepth 1 -type f))


CXXFLAGS+=-fno-keep-inline-functions -ffunction-sections -fshort-enums #-DLOGGING_DISABLE  -DBOOST_NO_STDLIB_CONFIG -D__NO_STL__ -I../../external/avr-halib/externals/include -I../../external/avr-halib/externals/include/boost/compatibility/cpp_c_headers/
LDFLAGS+= -Wl,--gc-sections #-Wl,--section-start=.text=0x1FC00
#Locations
#INCLUDES=../../include/ ../../platform/ ../../platform/default/
INCLUDE+= -I../../external/avr-halib/externals/include/boost/type_traits
LDPATHS=

#BIN=.
#/bin
#BUILD=.
#/build
#SRC=.
#INC=.
#HALIB:=../../external/avr-halib/
#PROGRAMMER=jtag2
#PORT=usb

#----------END USER_CONFIG --------



#include ${HALIB}/config.mk
#include ${HALIB}/rules/general.mk
#include ${HALIB}/rules/example.mk



.phony: all clean

all:
	@echo usage: \"make programm-to-build\"

my: 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CXXOPTIONS) main.cc t_mac.cc s_mac.cc -o test

#lol:
	#${LD} ${LDFLAGS} main.o mac.o -o $@
	#ld LDFLAGS main.o mac.o
#clean:
#	@rm -rf $(filtered)
