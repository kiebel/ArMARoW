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
#ifndef __ARMAROW_COMMON_SPI_h__
#define __ARMAROW_COMMON_SPI_h__

#include <avr-halib/avr/spi.h>
#include <avr-halib/share/singleton.h>

namespace armarow {
    namespace common {
        namespace helpers {
            /*! \brief SPI interface implementation
             *  \tparam PortMap Register map for spi operation on mcu
             *  \tparam CFG SPI configuration
             *
             *  This class is the basic implementation of a SPI bus driver. It
             *  can be used to communicate via the SPI interface. However this
             *  class is no singleton and therefore not suited to be used
             *  directly.
             *  \todo add interface for 2Byte register (uint16_t)
             */
            template<typename PortMap, typename CFG>
            struct SpiImpl : private SpiMaster< CFG > {
                private:
                    /*! \brief  basic SPI driver for bus operation*/
                    typedef SpiMaster< CFG > Base;
                public:
                    /*! \brief  Constructor initializes SPI and chip select.*/
                    SpiImpl() {
                        UseRegmap(rm, PortMap);
                        rm.cs.ddr  = true;
                        rm.cs.port = true;
                        SyncRegmap(rm);
                    }
                    void enable() {
                        UseRegmap(rm, PortMap);
                        rm.cs.port = false;
                        SyncRegmap(rm);
                    }
                    void disable() {
                        UseRegmap(rm, PortMap);
                        rm.cs.port = true;
                        SyncRegmap(rm);
                    }
                    void write(const uint8_t data) {
                        Base::put( (const char)(data) );
                    }
                    void read(uint8_t &data, const uint8_t addr = 0) {
                        Base::put(addr);
                        while ( !Base::get( (char &)(data) ) );
                    }
            };
        } // end namespace helper

        /*! \brief SPI interface for radio controller access.
         *  \tparam PortMap register map for spi operation on mcu
         *  \tparam CFG SPI configuration for operation on mcu and rc
         *
         *  This class is a singleton implementation of an SPI bus driver.
         *  It can be used to communicate with the specified radio controller.
         */
        template<typename PortMap, typename CFG>
        struct SPI
		{
				typedef avr_halib::object::Singleton< helpers::SpiImpl<PortMap, CFG> > type;
		};
    } // end namespace common
} // end namespace armarow

#endif  // __ARMAROW_COMMON_SPI_h__
