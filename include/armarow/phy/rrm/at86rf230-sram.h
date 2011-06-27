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
				namespace ram{
					namespace base=avr_halib::regmaps::base;
					using boost::mpl::list;

					struct RamReadRegister
					{
						enum RegisterParameters
						{
							address=0x00,
							mode=base::read
						};

						uint8_t startAddress;
						uint8_t data;
						union{
							struct{
								uint8_t startAccess : 1;
								uint8_t stopAccess  : 1;
							};
							uint8_t flags;
						};
					};


					struct RamWriteRegister
					{
						enum RegisterParameters
						{
							address=0x00,
							mode=base::write
						};

						uint8_t startAddress;
						uint8_t data;
						union{
							struct{
								uint8_t startAccess : 1;
								uint8_t stopAccess  : 1;
							};
							uint8_t flags;
						};
					};


					typedef typename list< RamReadRegister >::type ReadRegisterList;
					typedef typename list< RamWriteRegister >::type WriteRegisterList;

					template <typename BusDriver>
					class ReadAccess
					{
						private:
							static const uint8_t readOpMask  = 0x00;
						public:

							bool read(uint8_t address, uint8_t* data, uint8_t size)
							{
								BusDriver &bus=BusDriver::getInstance();
								
								RamReadRegister &reg=*(RamReadRegister*)data;
								if(reg.startAccess)
								{
									bus.enable();
									bus.write( readOpMask );
									bus.write( reg.startAddress );
									reg.startAccess=0;
								}

								bus.read(reg.data);
								
								if(reg.stopAccess)
								{
									bus.disable();
									reg.stopAccess=0;
								}
								return true;
							}

					};

					template <typename BusDriver>
					class WriteAccess
					{
						private:
							static const uint8_t writeOpMask = 0x40;
						public:

							bool write(uint8_t address, const uint8_t* data, uint8_t size)
							{
								BusDriver &bus=BusDriver::getInstance();
								
								RamWriteRegister &reg=*(RamWriteRegister*)data;
								if(reg.startAccess)
								{
									bus.enable();
									bus.write( writeOpMask );
									bus.write( reg.startAddress );
									reg.startAccess=0;
								}

								bus.write(reg.data);
								
								if(reg.stopAccess)
								{
									bus.disable();
									reg.stopAccess=0;
								}
								return true;
							}
					};
				    enum SpecialAddress {
                        txFifoStart   = 0x00,
                        rxFifoStart   = 0x80,
                	};
				}

				
				template<typename BusDriver>
                struct SRamRead :
                    public avr_halib::regmaps::base::RemoteRegMap<
                                            ram::ReadAccess<BusDriver>,
                                            ram::ReadRegisterList
                                            >{};

				template<typename BusDriver>
                struct SRamWrite :
                    public avr_halib::regmaps::base::RemoteRegMap<
                                            ram::WriteAccess<BusDriver>,
                                            ram::WriteRegisterList
                                            >{};
            }
        }
    }
}

#endif  //__ARMAROW_EXP_AT86RF230_SPEC_SRAM_h__
