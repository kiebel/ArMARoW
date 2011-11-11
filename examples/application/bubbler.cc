/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *				      Christoph Steup <steup@ivs.cs.uni-magdeburg.de>
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
#include <platform-cfg.h>               // platform dependent software config

#include <armarow/armarow.h>            // main ArMARoW include
#include <armarow/debug.h>              // ArMARoW logging and debugging
#include <armarow/phy/phy.h>            // physical layer
#include <idler.h>
/* === globals ============================================================== */
platform::config::mob_t message;
platform::config::rc_t  rc;             // radio controller
uint8_t channel = 11;                   // channel number the sniffer checks
TimeTriggeredEventSource eventSource;
/* === functions ============================================================ */
/*! \brief  Initializes the physical layer.*/
void send(){
    rc.setStateTRX(armarow::PHY::tx_on);
    rc.send(message);
    ::logging::log::emit() << PROGMEMSTRING("Sending message ") 
        << ((uint32_t*)message.payload)[0]++ << ::logging::log::endl;
}

void init() {
    message.size=sizeof(uint32_t);
    ((uint32_t*)message.payload)[0]=0;
    rc.init();
    rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
}
/* === main ================================================================= */
int main() {
    eventSource.registerCallback<send>();
    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting bubbler (repeated send of the same message)!")
        << ::logging::log::endl << ::logging::log::endl;
    init();                            // initialize

    Idler::idle();

    return 0;
}
