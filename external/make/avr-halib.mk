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
#                             AVR - EXTERNALS
# -----------------------------------------------------------------------------
ifeq ($(AVR_HALIBDIR),)
	AVR_HALIBDIR:=${BASEEXTERNAL}/avr_halib
	AVR_ECHO="Checking out latest AVR-halib"
	AVR_FETCH=svn co https://svn-eos.cs.uni-magdeburg.de/repos/Projects/AVR/halib/trunk ${AVR_HALIBDIR} --ignore-externals -q
	AVR_CONFIGURE=sed -e s?.*BOOST_DIR=?BOOST_DIR=$(abspath ${BOOSTDIR})?g -e s?.*LOGGING_DIR=?LOGGING_DIR=$(abspath ${LOGGINGDIR})?g ${AVR_HALIBDIR}/config.mk > temp && mv temp ${AVR_HALIBDIR}/config.mk
	AVR_CLEAN=${MAKE} -C ${AVR_HALIBDIR} clean &
	AVR_REMOVE=rm -rf ${AVR_HALIBDIR} &
endif

HALIB=${AVR_HALIBDIR}
CHIP=${MCU}

-include ${AVR_HALIBDIR}/config.mk
-include ${AVR_HALIBDIR}/make/config.mk

EXTERNAL_DEPS    += ${AVR_HALIBDIR}/lib/libavr-halib-${MCU}.a
EXTERNAL_TARGETS += ${AVR_HALIBDIR}
EXTERNAL_CLEANS  += ${AVR_CLEAN}
EXTERNAL_REMOVES += ${AVR_REMOVE}

${AVR_HALIBDIR}:
	@echo ${AVR_ECHO}
	@${AVR_FETCH}
	@${AVR_CONFIGURE}

${AVR_HALIBDIR}/config.mk: | ${AVR_HALIBDIR}

${AVR_HALIBDIR}/make/config.mk: | ${AVR_HALIBDIR}

${AVR_HALIBDIR}/lib/libavr-halib-${MCU}.a:
	@echo "Building needed avr-halib for $(MCU)"
	@make -C $(AVR_HALIBDIR) CHIP=${MCU}
