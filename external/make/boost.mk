################################################################################
##
## Copyright (c) 2010 Thomas Kiebel   <kiebel@ivs.cs.uni-magdeburg.de>
##				 2011 Christoph Steup <steup@ivs.cs.uni-magdeburg.de>
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

ifeq ($(BOOSTDIR),)
	BOOSTDIR:=${BASEEXTERNAL}/boost_1_46_1
	BOOST_ECHO="Installing latest Boost"
	BOOST_FILE=${BASEEXTERNAL}/boost_1_46_1.tar.bz2
	BOOST_INSTALL=tar -C ${BASEEXTERNAL} -jxf ${BOOST_FILE}
	BOOST_REMOVE=rm -rf ${BOOSTDIR} ${BOOST_FILE} &
	INCLUDES+=${BOOSTDIR}
else
	USER_BOOSTDIR:=${BOOSTDIR}
	BOOSTDIR:=${BASEEXTERNAL}/include/boost
	BOOST_ECHO="Linking user specified Boost Headers"
	BOOST_INSTALL=mkdir -p ${BASEEXTERNAL}/include && ln -s ${USER_BOOSTDIR}/boost ${BASEEXTERNAL}/include/
	BOOST_REMOVE=rm -rf ${BASEEXTERNAL}/include &
	INCLUDES+=${BASEEXTERNAL}/include
	
endif

EXTERNAL_REMOVES+=${BOOST_REMOVE}
EXTERNAL_TARGETS+=${BOOSTDIR}

${BOOST_FILE}:
	@echo "Downloading latest boost"
	@wget http://downloads.sourceforge.net/project/boost/boost/1.46.1/boost_1_46_1.tar.bz2 -O $@

${BOOSTDIR}: | ${BOOST_FILE}
	@echo ${BOOST_ECHO}
	@${BOOST_INSTALL}
