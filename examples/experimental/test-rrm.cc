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
/*! \file   examples/experimental/test-rrm.cc
 *  \brief  Proof of concept for the idea of remote regmaps.
 */
/* === includes ============================================================= */
#include "platform.h"   // platform dependent software config

#include <armarow/armarow.h>            // ArMARoW main include
#include <armarow/phy/rrm/at86rf230-spec.h>
#include <armarow/phy/rrm/rrm-rc.h>
/* === definitions ========================================================== */
namespace platform {
    class config {
            typedef AT86RF230_Hal halRc_t;              // hardware configuration
        public:
            typedef armarow::phy::Rrm< halRc_t, armarow::phy::specification::At86Rf230<halRc_t > > rc_t;
            typedef rc_t::spec_t spec_t;            // radio controller spec
            typedef rc_t::mob_t  mob_t;             // message object for RC
    };
}
/* === globals ============================================================== */
platform::config::rc_t radio;
/* === main ================================================================= */
int main() {
    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("proof of concept for the idea of remote regmaps")
        << ::logging::log::endl << ::logging::log::endl;

    //---------------------------------------------------------------
    radio.init();
    radio.testRegister();               // test register access
    radio.testSRAM();                   // test SRAM access
    radio.testTrxFiFo();                // test access to TRXFIFO
    //---------------------------------------------------------------
    do {                                // duty cycle
    } while (true);
}
