/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
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
/*! \file   examples/application/bubbler.cc
 *  \brief  Example implementation sending the same message over and over again.
 */
/* === includes ============================================================= */
#include "platform-cfg.h"               // platform dependent software config
#include "avr-halib/share/delay.h"      // delays and timings

#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging
#include "armarow/phy/phy.h"            // physical layer
/* === globals ============================================================== */
platform::config::mob_t message = {10,{'0','1','2','3','4','5','6','7','8','9'}};
platform::config::rc_t  rc;             // radio controller
uint8_t channel = 11;                   // channel number the sniffer checks
/* === functions ============================================================ */
/*! \brief  Initializes the physical layer.*/
void init() {
    rc.init();
    rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
}
/* === main ================================================================= */
int main() {
    uint16_t cnt = 0;

    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting bubbler (repeated send of the same message)!")
        << ::logging::log::endl << ::logging::log::endl;

    init();                             // initialize famouso
    do {                                // duty cycle
        delay_ms(1000);
        rc.setStateTRX(armarow::PHY::TX_ON);
        rc.send(message);
        ::logging::log::emit()
            << PROGMEMSTRING("Sending message ") << (int32_t)cnt++
            << ::logging::log::endl;
    } while (true);
}
