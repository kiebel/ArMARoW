/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas    Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *               2011 Christoph Steup  <steup@ivs.cs.uni-magdeburg.de>
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
/*! \file   examples/applixation/sniffer.cc
 *  \brief  Example implementation of a sniffer on the physical layer.
 */
/* === includes ============================================================= */
#include <platform-cfg.h>               // platform dependent software config
#include <avr-halib/share/delay.h>      // delays and timings

#include <armarow/armarow.h>            // main ArMARoW include
#include <armarow/debug.h>              // ArMARoW logging and debugging
#include <armarow/phy/phy.h>            // physical layer
#include <idler.h>
#include <avr-halib/regmaps/local.h>
#include <avr-halib/avr/clock.h>
/* === globals ============================================================== */
platform::config::mob_t message = {0,{0}};
platform::config::rc_t  rc;             // radio controller
uint8_t channel = 11;                   // channel number

UseInterrupt(SIG_OUTPUT_COMPARE1A);

struct MyClockConfig
{
    typedef uint16_t TickValueType;
    typedef Frequency<1> TargetFrequency;
    typedef CPUClock TimerFrequency;
    typedef avr_halib::regmaps::local::Timer1 Timer;
};

typedef avr_halib::drivers::Clock<MyClockConfig> Clock;

Clock clock;

/* === functions ============================================================ */
/*! \brief  Callback triggered by an interrupt of the radio controller.
 *  \todo   Add Information for LQI and RSSI values.
 */
void callback_recv() {
    /*rc.receive(message);
    log::emit()
        << PROGMEMSTRING("[CHANNEL: ") << (int32_t)channel
        << PROGMEMSTRING(", [DATA: [LENGTH: ") << (int32_t)message.size
        << PROGMEMSTRING("], [CONTENT: \"");

    log::emit()
        << ((uint32_t*)message.payload)[0]
    log::emit()
        << PROGMEMSTRING("\"]]]")
        << log::endl;*/
}
/*! \brief  Initializes the physical layer.*/
void init() {
    rc.init();
    rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
    rc.setStateTRX(armarow::PHY::rx_on);
    rc.onReceive.bind<callback_recv>();
}
/* === main ================================================================= */
int main() {
    sei();                              // enable interrupts
    log::emit()
        << PROGMEMSTRING("Starting EnergyDetector!")
        << log::endl << log::endl;

    init();                             // initialize

    uint8_t energyLevel=0;
    while(true){
        rc.doED(energyLevel);
        Clock::Time t;
        clock.getTime(t);
        if(energyLevel)
            log::emit() << t.ticks << " " << t.microTicks << " " << (uint16_t)energyLevel << log::endl;
    }

    return 0;
}
