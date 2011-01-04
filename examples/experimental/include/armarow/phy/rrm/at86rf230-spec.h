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

namespace armarow {
    namespace phy {
        namespace specification {
            template < typename HW, typename SPI >
            struct At86Rf230 {
                /*! \brief  register address data type*/
                typedef typename at86rf230::Register::address regadd_t;
                /*! \brief  register value data type*/
                typedef typename at86rf230::Register::mapping regval_t;

                typedef typename at86rf230::Register::template Access<SPI,at86rf230::Register::reg_trx_status> reg_trx_status;
                typedef typename at86rf230::Register::template Access<SPI,at86rf230::Register::reg_trx_state>  reg_trx_state;
                typedef typename at86rf230::Register::template Access<SPI,at86rf230::Register::reg_man_id_0>   reg_man_id_0;

                /*! \brief  definition of SRAM access */
                typedef typename at86rf230::Sram<SPI> sram;
                /*! \brief  definition of TRXFIFO access */
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
