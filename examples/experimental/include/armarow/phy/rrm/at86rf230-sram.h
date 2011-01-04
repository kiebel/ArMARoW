/*******************************************************************************
 *
 * Copyright (c) 2010-2011 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *               2010-2011 Christoph Steup <christoph.steup@st.ovgu.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/
#ifndef __ARMAROW_EXP_AT86RF230_SPEC_SRAM_h__
#define __ARMAROW_EXP_AT86RF230_SPEC_SRAM_h__

namespace armarow {
    namespace phy {
        namespace specification {
            namespace at86rf230 {
                template <typename Interface>
                struct Sram {
                    enum address {
                        sram_txfifo   = 0x00,
                        sram_rxfifo   = 0x80,
                    };
                    static void read (
                            uint16_t pAddress,
                            uint8_t* pBuffer,
                            uint8_t pSize)
                    {
                        (Interface::getInstance()).enable();
                        (Interface::getInstance()).write( 0x00 );
                        (Interface::getInstance()).write( pAddress );
                        while(pSize--)
                            (Interface::getInstance()).read( *(pBuffer++) );
                        (Interface::getInstance()).disable();
                    }
                    static void write(
                            uint16_t pAddress,
                            uint8_t* pBuffer,
                            uint8_t pSize)
                    {
                        (Interface::getInstance()).enable();
                        (Interface::getInstance()).write( 0x40 );
                        (Interface::getInstance()).write( pAddress );
                        while(pSize--)
                            (Interface::getInstance()).write( *(pBuffer++) );
                        (Interface::getInstance()).disable();
                    }
                };
            }
        }
    }
}

#endif  //__ARMAROW_EXP_AT86RF230_SPEC_SRAM_h__
