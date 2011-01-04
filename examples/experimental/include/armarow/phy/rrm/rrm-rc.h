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
#ifndef __ARMAROW_EXP_RRM_h__
#define __ARMAROW_EXP_RRM_h__
#define ARMAROW_DEBUG_DISABLE

#include <armarow/debug.h>
#include <avr-halib/share/delay.h>
#include <avr-halib/share/delegate.h>

namespace armarow {
    namespace phy {
        struct RrmCfg {
            enum {
                enabledCCA           = false,
                /*!< indicates whether a CCA is performed before transmitting*/
                enabledCRC           = true,
                /*!< indicates whether CRC checking of frames is enabled*/
                enabledAutoAck       = false,
                /*!< indicates whether the automatic acknowledgement is enabled*/
                enabledAddressDecode = false,
                /*!< indicates whether the automatic address decode is enabled*/
                enableConcurrence    = false,
                /*!< indicates whether interrupts can be used concurrently*/
                rxOnIdle             = true
                /*!< indicates whether the TX mode is turned off after a transmit*/
            };
        };

        template <typename HW, typename SPEC, typename CFG = struct RrmCFG>
        class Rrm {
            public:
                /*! \brief  definition of the class type*/
                typedef Rrm<HW,SPEC,CFG> type;
            private:
            public:
                void init() {
                    UseRegmap(rm, HW);
                    rm.reset.ddr  = true;
                    rm.reset.port = false;
                    rm.sleep.ddr  = true;
                    rm.sleep.port = false;
                    SyncRegmap(rm);
                    delay_us( SPEC::timings::TRX_CHIP_RESET_TIME_US );
                    rm.reset.port = true;
                    SyncRegmap(rm);
                    delay_us( SPEC::timings::TRX_CHIP_RESET_TIME_US );
                }
                void testRegister() {
                    typename SPEC::regval_t value;
                    value.value = 0;

                    // test register RR
                    SPEC::reg_man_id_0::read(value);        // read register
                    ::logging::log::emit() << "RR: "
                        << (uint16_t)value.value << ::logging::log::endl;

                    // TODO 2011-01-04 test register WR
                    // test register RWR
                    SPEC::reg_trx_state::read(value);
                    value.reg_trx_state.trx_cmd = 0x08;
                    SPEC::reg_trx_state::write(value);      // write register

                    value.value = 0;
                    SPEC::reg_trx_state::read(value);       // read register
                    ::logging::log::emit() << "RW: "
                        << (uint16_t)value.value << ::logging::log::endl;
                }
                void testSRAM() {
                    uint8_t buffer[128];
                    uint8_t count = 0;
                    uint8_t size = 128;
                    uint8_t* bp = buffer;

                    // write SRAM
                    while (size--) *(bp++)=0x41;
                    size=128;bp=buffer;
                    SPEC::sram::write(SPEC::sram::sram_txfifo, buffer, size);
                    while (size--) *(bp++)=0x00;
                    size=128;bp=buffer;

                    // read SRAM
                    SPEC::sram::read (SPEC::sram::sram_txfifo, buffer, size);
                    while (size--) count += (( *(bp++) == 0x41 ) ? 1 : 0);
                    ::logging::log::emit() << "SRAM: "
                        << '[' << (uint16_t)count << ']'
                        << ::logging::log::endl;
                }
                void testTrxFiFo() {
                    uint8_t buffer[127];
                    uint8_t count = 0;
                    uint8_t size = 127;
                    uint8_t* bp = buffer;

                    // write TRX
                    while (size--) *(bp++)=0x01;
                    size=127;bp=buffer;
                    SPEC::trxfb::write(buffer, size);

                    // read TRX
                    SPEC::trxfb::read(buffer, size);
                    while (size--) count += (( *(bp++) == 0x42 ) ? 1 : 0);
                    bp=buffer;
                    ::logging::log::emit() << "TRXFIFO: "
                        << '[' << (uint16_t)*(bp++) << ']' << (uint16_t)count
                        << ::logging::log::endl;
                }
        };
    }; // end namespace phy
}; // end namespace armarow
#endif  //__ARMAROW_EXP_RRM_h__
