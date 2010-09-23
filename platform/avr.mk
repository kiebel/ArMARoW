################################################################################
##
## Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
##               2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
##               2010 Christoph Steup <christoph.steup@student.ovgu.de>
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
# -----------------------------------------------------------------------------
#                             CONFIGURATION
# -----------------------------------------------------------------------------
#HALIBDIR   = # configure location for avr halib library
HAEXTDIR   = $(ARMAROWDIR)/external/avr-halib
HALIBDIR  ?= $(HAEXTDIR)
ifeq ($(HALIBDIR), $(HAEXTDIR))
	ADDITIONAL_DIR += $(HALIBDIR)
endif
# -----------------------------------------------------------------------------
AR      = avr-ar
ARFLAGS = ru
AS      = avr-as
AVRDUDE = avrdude
CC      = avr-gcc
CXX     = avr-g++
LD      = avr-ld
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
PMGEN   = $(HALIBDIR)/tools/portmapgen/avr-halib-pmg
RANLIB  = avr-ranlib
SIZE    = avr-size

AVRDUDE_PROGRAMMER ?= avr911
AVRDUDE_PORT       ?= /dev/ttyUSB0
AVRDUDE_FLAGS       = -v -P $(AVRDUDE_PORT) -u -c $(AVRDUDE_PROGRAMMER) -p $(MCU)
# -----------------------------------------------------------------------------
MCU                ?= at90can128
CPU_CLOCK          ?= 16000000
LIBAVR              = $(LIBDIR)/libavr-halib-$(MCU).a

ADDITIONAL_CFLAGS  += -DF_CPU=${CPU_CLOCK}ULL -DAVR -mmcu=$(MCU) -fno-threadsafe-statics
