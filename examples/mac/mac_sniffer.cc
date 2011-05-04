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
/*! \file   examples/applixation/sniffer.cc
 *  \brief  Example implementation of a sniffer on the physical layer.
 */
/* === includes ============================================================= */
/*#include "platform-cfg.h"               // platform dependent software config
#include "avr-halib/share/delay.h"      // delays and timings

#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging
#include "armarow/phy/phy.h"            // physical layer
*/


/*
void* operator new (size_t , void* buffer){

return buffer;

} 

#include "mac_message.h"
*/

#include "mac.h"

/* === globals ============================================================== */
platform::config::mob_t message = {0,{0}};
//platform::config::rc_t  rc;             // radio controller
armarow::MAC::MAC_Base mac;
uint8_t channel = 11;                   // channel number
/* === functions ============================================================ */
/*! \brief  Callback triggered by an interrupt of the radio controller.
 *  \todo   Add Information for LQI and RSSI values.
 */
void callback_recv() {
    //rc.receive(message);
    ::logging::log::emit()
        << PROGMEMSTRING("[CHANNEL: ") << (int32_t)channel
        << PROGMEMSTRING(", [DATA: [LENGTH: ") << (int32_t)message.size
        << PROGMEMSTRING("], [CONTENT: \"");

/*
    for (uint8_t index = 0; index < message.size; index++) {
        char aChar = (char)message.payload[index];
        //if ((aChar >= '!') && (aChar >= '~')) ::logging::log::emit() << aChar;
	::logging::log::emit() << aChar;
    }
*/
    ::logging::log::emit()
        << PROGMEMSTRING("\"]]]")
        << ::logging::log::endl;

//	armarow::MAC::MAC_Message mac_msg(message);

	armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(message);

	mac_msg->print();
	//mac_msg->hexdump();

	::logging::log::emit() << "msg: " << (void*)mac_msg << " mac_header: " << (void*)&mac_msg->header << " mac_payload data: " << (void*)&mac_msg->payload.data << ::logging::log::endl;

}
/*! \brief  Initializes the physical layer.*/
void init() {
    //rc.init();
    //rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
    //rc.setStateTRX(armarow::PHY::RX_ON);
    //rc.onReceive.bind<callback_recv>();
}
/* === main ================================================================= */
int main() {

    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting sniffer!")
        << ::logging::log::endl << ::logging::log::endl;

    init();                             // initialize famouso

    size_t buffersize=sizeof(platform::config::mob_t);
    char buffer[sizeof(platform::config::mob_t)];
    //char buffer[10];
	
    do {                                // duty cycle
        //delay_ms(1000);
    //::logging::log::emit()
    //    << PROGMEMSTRING("Starting sniffer!")
    //    << ::logging::log::endl << ::logging::log::endl;

		int numberofreceivedbytes = mac.receive(buffer,buffersize);

		 ::logging::log::emit()
        << PROGMEMSTRING("Number of Received bytes: ") << numberofreceivedbytes
        << ::logging::log::endl << ::logging::log::endl;

		 ::logging::log::emit()
        << PROGMEMSTRING("[Content:] ") << buffer
        << ::logging::log::endl << ::logging::log::endl;
		 

    } while (true);
}
