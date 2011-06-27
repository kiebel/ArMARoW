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
#ifndef __ARMAROW_EXP_AT86RF230_SPEC_FIFO_h__
#define __ARMAROW_EXP_AT86RF230_SPEC_FIFO_h__

namespace armarow {
    namespace phy {
        namespace specification {
			namespace at86rf230{
			namespace fifo{
					namespace base=avr_halib::regmaps::base;
					using boost::mpl::list;

					struct FifoReadRegister
					{
						enum RegisterParameters
						{
							address=0x00,
							mode=base::read
						};

						uint8_t length;
						uint8_t index;
						uint8_t data;
						uint8_t done : 1;
					};


					struct FifoWriteRegister
					{
						enum RegisterParameters
						{
							address=0x00,
							mode=base::write
						};

						uint8_t length;
						uint8_t index;
						uint8_t data;
						uint8_t done : 1;
					};


					typedef typename list< FifoReadRegister >::type ReadRegisterList;
					typedef typename list< FifoWriteRegister >::type WriteRegisterList;

					template <typename BusDriver>
					class ReadAccess
					{
						private:
							static const uint8_t readOpMask  = 0x20;
						public:

							bool read(uint8_t address, uint8_t* data, uint8_t size)
							{
								BusDriver &bus=BusDriver::getInstance();
								
								FifoReadRegister &reg=*(FifoReadRegister*)data;
								if(reg.done)
								{
									bus.enable();
									bus.write( readOpMask );
									bus.read(reg.length);
									reg.index=0;
									reg.done=0;
								}
								else
								{
									bus.read(reg.data);
									reg.index++;
								}
								if(reg.index==reg.length)
								{
									bus.disable();
									reg.done=1;
								}
								return true;
							}

					};

					template <typename BusDriver>
					class WriteAccess
					{
						private:
							static const uint8_t writeOpMask = 0x60;
						public:

							bool write(uint8_t address, const uint8_t* data, uint8_t size)
							{
								BusDriver &bus=BusDriver::getInstance();
								
								FifoWriteRegister &reg=*(FifoWriteRegister*)data;
								if(reg.done)
								{
									bus.enable();
									bus.write( writeOpMask );
									bus.write( reg.length );
									reg.index=0;
									reg.done=0;
								}
								else
								{
									bus.write(reg.data);
									reg.index++;
								}
								
								if(reg.index==reg.length)
								{
									bus.disable();
									reg.done=1;
								}
								return true;
							}
					};
				}

				
				template<typename BusDriver>
                struct FifoRead :
                    public avr_halib::regmaps::base::RemoteRegMap<
                                            fifo::ReadAccess<BusDriver>,
                                            fifo::ReadRegisterList
                                            >{};

				template<typename BusDriver>
                struct FifoWrite :
                    public avr_halib::regmaps::base::RemoteRegMap<
                                            fifo::WriteAccess<BusDriver>,
                                            fifo::WriteRegisterList
                                            >{};
		}
            namespace rrm {
                template <typename Interface>
                struct TrxFiFo {
                    //TODO CRC uint8_t readRxFifo(uint8_t pSize, uint8_t *pData, uint8_t *pLqi, bool &pCrc) {return 0;}
                    static uint8_t read (uint8_t* pBuffer, uint8_t pSize) {
                        uint8_t count = 0;

                        (Interface::getInstance()).enable();
                        (Interface::getInstance()).write( 0x20 );
                        (Interface::getInstance()).read( count );
                        if ( pSize >= count) pSize = 0;
                        while(pSize--)
                            (Interface::getInstance()).read( *(pBuffer++) );
                        (Interface::getInstance()).disable();
                        //TODO 2011-01-10 read LQI value
                        return ( (pSize >= count) ? count : 0 );
                    }
                    //TODO CRC void writeTxFifo(uint8_t pSize, const uint8_t *pData) {}
                    static bool write(uint8_t* pBuffer, uint8_t pSize) {
                        (Interface::getInstance()).enable();
                        if (pSize > 127) return false; //TODO 2011-01-10 use information about frame size
                        (Interface::getInstance()).write( 0x60  );
                        (Interface::getInstance()).write( pSize );
                        while(pSize--)
                            (Interface::getInstance()).write( *(pBuffer++) );
                        (Interface::getInstance()).disable();
                        return true;
                    }
                };
            }
        }
    }
}

#endif  //__ARMAROW_EXP_AT86RF230_SPEC_FIFO_h__
