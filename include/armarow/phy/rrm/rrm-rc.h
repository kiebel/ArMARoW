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

        using namespace phy::specification::at86rf230::registers;
		using namespace phy::specification::at86rf230::ram;
		using namespace phy::specification::at86rf230::fifo;

        template <typename HW, typename SPEC, typename CFG = struct RrmCFG>
        class Rrm {
            public:
                /*! \brief  definition of the class type*/
                typedef Rrm<HW,SPEC,CFG> type;
                typedef typename SPEC::RegMap RegMap;
				typedef typename SPEC::SRamRead SRamRead;
				typedef typename SPEC::SRamWrite SRamWrite;
				typedef typename SPEC::FifoRead FifoRead;
				typedef typename SPEC::FifoWrite FifoWrite;
				typedef typename HW::portmap_t portmap_t;
				typedef SPEC spec_t;
				struct mob_t {};
            private:
            public:
                void init() {
                    UseRegmap(rm, portmap_t);
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
                    UseRegMap(rm, RegMap);
                    SyncRegister(rm, MAN_IDRegister);

                    ::logging::log::emit() << "RR: "
                        << (uint16_t)rm.man_id << ::logging::log::endl;

                    rm.trx_cmd=0x08;
                    SyncRegister(rm, TRX_StateRegister);
					delay_us(880);
					SyncRegister(rm, TRX_StatusRegister);

                    ::logging::log::emit() << "RW: "
                        << (uint16_t)rm.trx_cmd << ::logging::log::endl;

					::logging::log::emit() << "RWR: " << (uint16_t)rm.trx_status << ::logging::log::endl;
                }

				void testSRAM() {
					static const uint8_t size=16;
                    uint8_t buffer[size];

					for(uint8_t i=0;i<size;i++)
						buffer[i]=0x41+i;

					{
						UseRegMap(rm, SRamWrite);
						rm.startAddress=0;
						rm.startAccess=1;
						
						for(uint8_t i=0;i<size;i++)
						{
							rm.data=buffer[i];
							if(i==size-1)
								rm.stopAccess=1;
							SyncRegister(rm, RamWriteRegister);
							
						}
					}

					for(uint8_t i=0;i<size;i++)
						buffer[i]=0x0;

					{
						UseRegMap(rm, SRamRead);
						rm.startAddress=0;
						rm.startAccess=1;

						for(uint8_t i=0;i<size;i++)
						{
							if(i==size-1)
								rm.stopAccess=1;
							SyncRegister(rm, RamReadRegister);
							buffer[i]=rm.data;
						}
					}

                    ::logging::log::emit() << "SRAM: "
                        << '[';
					for(uint8_t i=0;i<size;i++)
						::logging::log::emit() << log::hex << (uint16_t)buffer[i];
					
					::logging::log::emit() << ']' << ::logging::log::endl;
                }


                void testTrxFiFo() {
					static const uint8_t size=16;
                    uint8_t buffer[size+1];

					for(uint8_t i=0;i<size;i++)
						buffer[i]=0x41+i;

					{
						UseRegMap(rm, FifoWrite);
						rm.length=size;
						SyncRegister(rm, FifoWriteRegister);
						while(!rm.done)
						{
							rm.data=buffer[rm.index];
							SyncRegister(rm, FifoWriteRegister);
						}
					}
                    //SPEC::trxfb::write(buffer, size);
					for(uint8_t i=0;i<size;i++)
						buffer[i]=0;


                    // read TRX
                    /*{
						UseRegMap(rm, FifoWrite);
						SyncRegister(rm, FifoWriteRegister);
						while(!rm.done)
						{
							SyncRegister(rm, FifoWriteRegister);
							buffer[rm.index]=rm.data;
						}*/

						UseRegMap(rm, SRamRead);
						rm.startAddress=txFifoStart;
						rm.startAccess=1;

						for(uint8_t i=0;i<size+1;i++)
						{
							if(i==size-1)
								rm.stopAccess=1;
							SyncRegister(rm, RamReadRegister);
							buffer[i]=rm.data;
						}

						//SPEC::trxfb::read(buffer, size);
						
                    	::logging::log::emit() << "TRXFIFO[" << (uint16_t)size << "]: " << '[';
						for(uint8_t i=0;i<size;i++)
							::logging::log::emit() << (uint16_t)buffer[i];
						::logging::log::emit() << ']' << ::logging::log::endl;
					}
        };
    }; // end namespace phy
}; // end namespace armarow
#endif  //__ARMAROW_EXP_RRM_h__
