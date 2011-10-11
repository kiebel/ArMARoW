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
/*! \brief  Unit test for the physical layers of ArMARoW.
 *  \file   test/unittest-phy.cc
 *
 *  This file is part of the ArMARoW library. See copyright for details for using.
 */
/* === includes ============================================================= */
#include "platform-cfg.h"               // platform dependent software config
#include "avr-halib/share/delay.h"      // delays and timings

#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging
#include "armarow/phy/phy.h"            // physical layer
/* === globals ============================================================== */
bool rcIRQ = false;
platform::config::mob_t des  = {0,{0}};
platform::config::mob_t src  = {10,{'0','1','2','3','4','5','6','7','8','9'}};
platform::config::rc_t  rc;
/* === functions ============================================================ */
/*! \brief  Callback triggered by an interrupt of the radio controller.*/
void callback_recv() {
    uint8_t size = rc.receive(des);
    assert(size == des.size);
    rcIRQ = true;
}
void setup() { rc.init(); }
/*! \brief  Unit case testing the radio controller attributes.*/
void case_attribute() {
    setup();
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Case[attribute] " << ::logging::log::endl;

    uint8_t param = 0;
    uint8_t value = 0;

    // read_only attributes -------------------------------
    assert(rc.setAttribute(armarow::PHY::phyMaxFrameDuration, &value) == armarow::PHY::read_only);
    assert(rc.setAttribute(armarow::PHY::phySHRDuration, &value)      == armarow::PHY::read_only);
    assert(rc.setAttribute(armarow::PHY::phySymbolsPerOctet, &value)  == armarow::PHY::read_only);

    // channel & channel support --------------------------
    //FIXME Dynamic channel testing configured by the specfile!!!
    for (uint8_t channel = 0; channel <= 26; channel++) {
        if ( (channel >= platform::config::spec_t::Channel::minChannel) &&
                (channel <= platform::config::spec_t::Channel::maxChannel) ) {
            assert(rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel) == armarow::PHY::success);
            assert(rc.getAttribute(armarow::PHY::phyCurrentChannel, &value)   == armarow::PHY::success);
            assert(channel == value);
        } else {
            assert(rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel) == armarow::PHY::invalid_parameter);
        }
    }
    uint32_t channels = 0;
    assert(rc.setAttribute(armarow::PHY::phyChannelsSupported, &channels) == armarow::PHY::read_only);
    assert(rc.getAttribute(armarow::PHY::phyChannelsSupported, &channels) == armarow::PHY::success);
    assert(channels == platform::config::spec_t::Channel::supportedChannel());

    // transmit power -------------------------------------
    for (uint8_t index = 0; index <= 16; index++) {
        if ( index == 16) {
            assert(rc.setAttribute(armarow::PHY::phyTransmitPower, &index) == armarow::PHY::invalid_parameter);
        } else {
            assert(rc.setAttribute(armarow::PHY::phyTransmitPower, &index) == armarow::PHY::success);
            assert(rc.getAttribute(armarow::PHY::phyTransmitPower, &value) == armarow::PHY::success);
            assert(index == value);
        }
    }

    // cca mode -------------------------------------------
    param = armarow::PHY::threshold;
    value = 0;
    assert(rc.setAttribute(armarow::PHY::phyTransmitPower, &param) == armarow::PHY::success);
    assert(rc.getAttribute(armarow::PHY::phyTransmitPower, &value) == armarow::PHY::success);
    assert(param == value);
    param = armarow::PHY::carrierSense;
    value = 0;
    assert(rc.setAttribute(armarow::PHY::phyTransmitPower, &param) == armarow::PHY::success);
    assert(rc.getAttribute(armarow::PHY::phyTransmitPower, &value) == armarow::PHY::success);
    assert(param == value);
    param = armarow::PHY::carrierSenseThreshold;
    value = 0;
    assert(rc.setAttribute(armarow::PHY::phyTransmitPower, &param) == armarow::PHY::success);
    assert(rc.getAttribute(armarow::PHY::phyTransmitPower, &value) == armarow::PHY::success);
    assert(param == value);

    // page -----------------------------------------------
    //TODO  if chip supports more pages do something to test
}
/*! \brief  Unit case testing the methods for carrier sense.*/
void case_medium() {
    //FIXME How can CCA and ED be tested continously?
    setup();
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Case[reset] " << ::logging::log::endl;

    armarow::PHY::State cstate;
    uint8_t value;

    cstate = rc.doCCA();                      //TODO
    cstate = rc.doED(value);                  //TODO
}
/*! \brief  Unit case testing the radio controller receive.*/
void case_recv() {
    setup();
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Case[receive] " << ::logging::log::endl;

    uint8_t channel = 11;

    assert(rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel) == armarow::PHY::success);
    //FIXME test if recv works without correct state
    rc.setStateTRX(armarow::PHY::rx_on);

    // receive blocking
    uint8_t size = rc.receive_blocking(des);
    assert(size == des.size);
    assert(des.size == src.size);
    for (uint8_t index = 0; index < des.size; index++) {
        assert(des.payload[index] == (src.payload[index]));
    }

    // receive non blocking (interrupt triggered)
    rc.onReceive.bind<callback_recv>();
/*    while( !rcIRQ ) { delay_ms(500); }    // interrupt triggered receive
    assert(des.size == src.size);
    for (uint8_t index = 0; index < des.size; index++) {
        assert(des.payload[index] == (src.payload[index]));
    } */
}
/*! \brief  Unit case testing the reset of the radio controller.*/
void case_reset() {
    setup();
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Case[reset] " << ::logging::log::endl;

    rc.reset();
    assert(rc.getStateTRX() == armarow::PHY::trx_off);
}
/*! \brief  Unit case testing the radio controller send.*/
void case_send() {
    setup();
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Case[send] " << ::logging::log::endl;

    uint8_t channel = 11;

    assert(rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel) == armarow::PHY::success);

    // use of states --------------------------------------
    assert(rc.send_blocking(src) ==  armarow::PHY::trx_off);
    assert(rc.send(src) ==  armarow::PHY::trx_off);
    // use of sizes ---------------------------------------
    src.size = 0x00;
    rc.setStateTRX(armarow::PHY::tx_on);
    assert(rc.send_blocking(src) == armarow::PHY::success);
    rc.setStateTRX(armarow::PHY::tx_on);
    assert(rc.send(src) == armarow::PHY::success);
    src.size = 0x80;
    rc.setStateTRX(armarow::PHY::tx_on);
    assert(rc.send_blocking(src) == armarow::PHY::invalid_parameter);
    rc.setStateTRX(armarow::PHY::tx_on);
	assert(rc.send(src) == armarow::PHY::invalid_parameter);
    // send message ----------------------------------------
    src.size = 10;
    rc.setStateTRX(armarow::PHY::tx_on);
    assert(rc.send_blocking(src) == armarow::PHY::success);
    rc.setStateTRX(armarow::PHY::tx_on);
    assert(rc.send(src) == armarow::PHY::success);
    // necessary to avoid abording transmission, by the reset of the next
    // testsuit
    delay_ms(1000);
}
/*! \brief  Unit case testing the getter/setter of the radio states. */
void case_state() {
    setup();
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Case[state] " << ::logging::log::endl;

    assert(rc.getStateTRX() == armarow::PHY::trx_off);
    rc.setStateTRX(armarow::PHY::rx_on);
    assert(rc.getStateTRX() == armarow::PHY::rx_on);
    rc.setStateTRX(armarow::PHY::trx_off);
    assert(rc.getStateTRX() == armarow::PHY::trx_off);
    rc.setStateTRX(armarow::PHY::tx_on);
    assert(rc.getStateTRX() == armarow::PHY::tx_on);
    rc.setStateTRX(armarow::PHY::force_trx_off);
    assert(rc.getStateTRX() == armarow::PHY::trx_off);
}
/* === main ================================================================= */
int main() {
    sei();                    // enable interrupts

    ::logging::log::emit() << "UnitTest Suite[PHY]"  << ::logging::log::endl;
    //---------------------------------------------------------------
    case_state();             // testsuite states
    case_reset();             // testsuite reset
    case_attribute();         // testsuite attributes
    case_medium();            // testsuite medium access
    case_send();              // testsuite sending
    case_recv();              // testsuite receiving
    //---------------------------------------------------------------
    ::logging::log::emit() << "UnitTest Suite[PHY]!" << ::logging::log::endl;
    while(1);
}
