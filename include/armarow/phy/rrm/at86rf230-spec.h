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
#ifndef __ARMAROW_EXP_AT86RF230_SPEC_h__
#define __ARMAROW_EXP_AT86RF230_SPEC_h__

#include <armarow/phy/rrm/at86rf230-register.h>
#include <armarow/phy/rrm/at86rf230-sram.h>
#include <armarow/phy/rrm/at86rf230-trxfifo.h>
#include <armarow/common/spi.h>

namespace armarow {
    namespace phy {
        namespace specification {
            template < typename HW >
            struct At86Rf230 {
				typedef typename armarow::common::SPI<typename HW::portmap_t, SpiCfg>::type SPI;
                /*! \brief  register definition for the AT86RF230
                 *          radio controller */
                typedef at86rf230::RegMap<SPI> RegMap;
				typedef at86rf230::SRamRead<SPI> SRamRead;
				typedef at86rf230::SRamWrite<SPI> SRamWrite;
				typedef at86rf230::FifoRead<SPI> FifoRead;
				typedef at86rf230::FifoWrite<SPI> FifoWrite;

                /*! \brief  TRX framebuffer definition for the AT86RF230
                 *          radio controller */
                typedef typename rrm::TrxFiFo<SPI> trxfb;

                struct timings {
                    enum {
                        TRX_RESET_TIME_US      = 6,
                        /*!< duration while reset=low is asserted*/
                        TRX_INIT_TIME_US       = 510,
                        /*!< duration for transceiver to reach TRX_OFF for the
                         *   first time*/
                        TRX_CCA_TIME_US        = 140,
                        /*!< duration of a CCA measurement*/
                        TRX_CHIP_RESET_TIME_US = 625
                        /*!< duration for transceiver reset*/
                    };
                };
            };
        }; // end namespace specification
    }; // end namespace phy
}; // end namespace armarow

#endif  //__ARMAROW_EXP_AT86RF230_SPEC_h__