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
 *  \brief  Example implementation of a sniffer on the mac layer.
 */
/* === includes ============================================================= */

#include "armarow/mac/mac_csma_ca.h"

#include "armarow/mac/util.h"

/* === globals ============================================================== */

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=20
	};

};



armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;


armarow::MAC::mob_t messageobject;

uint8_t channel = 11;                   // channel number
/* === functions ============================================================ */
/*! \brief  Callback triggered by an interrupt of the radio controller.
 *  \todo   Add Information for LQI and RSSI values.
 */

#undef LOGGING_DISABLE


MessageLossMeasurement<100> msg_measurement_object;

uint32_t global_sequence_number=0;

//erst notifizieren, und dann receive aufrufen, wo man receive puffer übergibt
void callback_recv() {

 	if(mac.receive(messageobject)!=0){
		
		messageobject.get_object_from_payload(msg_measurement_object);
		
		while(global_sequence_number<msg_measurement_object.global_sequence_number){
			::logging::log::emit()
        	//<< PROGMEMSTRING("[Content:] ") << messageobject.payload << ::logging::log::endl
		<< PROGMEMSTRING("lost message: ") << (int) global_sequence_number++
        	<< ::logging::log::endl; // << ::logging::log::endl;
		}		

		::logging::log::emit()
        	//<< PROGMEMSTRING("[Content:] ") << messageobject.payload << ::logging::log::endl
		<< PROGMEMSTRING("gsnr: ") << (int) msg_measurement_object.global_sequence_number
        	<< ::logging::log::endl; // << ::logging::log::endl;

		global_sequence_number++;
	}else{

		::logging::log::emit()
        	<< PROGMEMSTRING("Failed receiving message!") 
        	<< ::logging::log::endl << ::logging::log::endl;

	}

}
/*! \brief  Initializes the physical layer.*/
void test_asynchron_receive() {

    mac.onMessageReceiveDelegate.bind<callback_recv>();
    while(1);
}
/* === main ================================================================= */
int main() {

    

    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting sniffer!")
        << ::logging::log::endl << ::logging::log::endl;

    test_asynchron_receive();    //aus bzw. einkommentieren für aynchronen/synchronen test                       



}
