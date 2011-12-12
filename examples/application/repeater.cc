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
/*! \file   examples/applixation/repeater.cc
 *  \brief  Example implementation that sends a received packet n-times.
 *  \note   The repeater works directly on the physical layer so it does not
 *          regard any MAC protocol.
 */
/* === includes ============================================================= */
#include <platform.h>               // platform dependent software config
#include <radio.h>
#include <avr-halib/share/delay.h>      // delays and timings
#include <avr-halib/avr/clock.h>
#include <avr-halib/share/freq.h>

#include <armarow/armarow.h>            // main ArMARoW include
#include <armarow/debug.h>              // ArMARoW logging and debugging
#include <armarow/phy/phy.h>            // physical layer

using avr_halib::config::Frequency;
using avr_halib::drivers::Clock;

typedef platform::config::RadioDriver<> RadioController;
RadioController::mob_t message;

struct ClockConfig : public platform::avr::clock::Clock1BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint8_t TickValueType;
};

Clock<ClockConfig> periodicTrigger;
RadioController rc;
uint8_t counter;
uint8_t channel=11;

void callback_recv() {
    uint8_t size = ( !counter) ? rc.receive(message) : 0;
    counter = (size != 0) ? 5 : 0;       // set number of repetitions
}

void send(){
    if ( counter ) {
        rc.setStateTRX(armarow::PHY::tx_on);
        rc.send(message);
        log::emit()
            << "Send message " << (int32_t)counter
            << log::endl;
        counter--;
    }
}

/*! \brief  Initializes the physical layer.*/
void init() {
    rc.init();
    rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
    rc.setStateTRX(armarow::PHY::rx_on);
    rc.onReceive.bind<callback_recv>();
    periodicTrigger.registerCallback<send>();
}
/* === main ================================================================= */
int main() {
    sei();                              // enable interrupts
    log::emit()
        << "Starting PHY repeater!"
        << log::endl << log::endl;

    init();                             // initialize radio controller

    Idler::idle();
}
