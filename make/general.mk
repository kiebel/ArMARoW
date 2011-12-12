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
# if ARMAROWDIR isn't defined, the root directory is automatically determind
ifeq ($(ARMAROWDIR),)
ARMAROWCWD := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
ARMAROWDIR := $(patsubst %/platform/,%,$(ARMAROWCWD))
endif

ifneq ($(shell test -f $(ARMAROWDIR)/make/rules.mk && echo success), success)
$(info )
$(info The ARMAROWDIR you gave to the make-system is wrong. The make-system could )
$(info not find all needed files. In general the make-system is able to determine )
$(info its root directory automatically. Thus, you have three options:            )
$(info    - First, you don't define ARMAROWDIR and let the make-system do the job.)
$(info    - Second, you define the ARMAROWDIR environment variable.               )
$(info    - Third, you start make as follows "make ARMAROWDIR="root-dir".         )
$(info )
$(error root directory is given wrong)
endif

include $(ARMAROWDIR)/config.mk
# -----------------------------------------------------------------------------

BASEDOC      = $(ARMAROWDIR)/doc
BASEEXTERNAL = $(ARMAROWDIR)/external
BASEINC      = $(ARMAROWDIR)/include
EXAMPLES	 = ${ARMAROWDIR}/examples

ARMAROW_DEBUG ?= -g -DNDEBUG

INCLUDES     += $(ARMAROWDIR)/platform/$(PLATFORM) $(ARMAROWDIR)/include
CFLAGS       += -fno-strict-aliasing
CXXFLAGS     += -fno-strict-aliasing

# -----------------------------------------------------------------------------
ifeq (${AVR_HALIBDIR},)
	include ${ARMAROWDIR}/external/make/boost.mk
	include ${ARMAROWDIR}/external/make/logging.mk	
endif
include ${ARMAROWDIR}/platform/${PLATFORM}/config.mk
include ${ARMAROWDIR}/make/${ARCH}/config.mk


CFLAGS   += $(addprefix -I,${INCLUDES})
CXXFLAGS += $(addprefix -I,${INCLUDES})
LDFLAGS  += $(addprefix -L,${LDPATHS})
LDFLAGS  += $(addprefix -l,${LIBS})
