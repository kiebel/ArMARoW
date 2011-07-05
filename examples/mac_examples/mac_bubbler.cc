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

#include "armarow/mac/mac_csma_ca.h"

/* === globals ============================================================== */
platform::config::mob_t message = {10,{'0','1','2','3','4','5','6','7','8','9'}};

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

enum {
channel=1,
mac_adress_of_node=38
};

};

#undef LOGGING_DISABLE

//armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t> mac;
armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;



void success_transmission_callback(){

//::logging::log::emit()
//            << PROGMEMSTRING("Successful send a message...")
//            << ::logging::log::endl;

//secure callback, so it can't be interrupted
avr_halib::locking::GlobalIntLock lock;

::logging::log::emit()
            << PROGMEMSTRING("msg...")
            << ::logging::log::endl;

}


void async_sending_test(armarow::MAC::mob_t msg){

  mac.onMessage_Successfull_Transmitted_Delegate.bind<success_transmission_callback>();

  int ret;

  while(1){

	//test for one shot timer
	//mac.send_async(msg);
	//while(1);
     
	   //::logging::log::emit()  << PROGMEMSTRING("Try sending a message...") << ::logging::log::endl;
     {

        
   
        ret=mac.send_async(msg);
     
	avr_halib::locking::GlobalIntLock lock;

	::logging::log::emit()
            << PROGMEMSTRING("ret is: ") << ret
            << ::logging::log::endl;
	}

     //delay_ms(1);

  }

}


//armarow::MAC::MAC_CSMA_CA mac;


/* === main ================================================================= */
int main() {
    uint16_t cnt = 0;

    armarow::MAC::mob_t messageobject;


   //const char messagecontent[] = "MAC LAYER TEST";  //15 bytes
   //const char messagecontent[] = "MAC LAYER TEST with much larger messages, increasing efficency"; //63 bytes
   const char messagecontent[] = "MAC LAYER TEST with larger messages, increasing efficency, using the maximum message size, to test for collisions"; //114 bytes


	    ::logging::log::emit()
            << PROGMEMSTRING("Sending the following message ") << (int32_t)cnt++
            << ::logging::log::endl << ::logging::log::endl;

 for(unsigned int i=0;i<sizeof(message);i++)
	    messageobject.payload[i]=messagecontent[i];   //copy data in message payload

	messageobject.size=sizeof(messagecontent);


    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting bubbler (repeated send of the same message)!")
        << ::logging::log::endl << ::logging::log::endl;


    async_sending_test(messageobject);

    //main loop
    do {

		::logging::log::emit()
        	//<< PROGMEMSTRING("[Content:] ") << messageobject.payload << ::logging::log::endl
		<< PROGMEMSTRING("Node ID: ") << (int) messageobject.header.source_adress
		<< PROGMEMSTRING("Message Sequence Number: ") << (int) messageobject.header.sequencenumber
        	<< ::logging::log::endl << ::logging::log::endl;


	

	if(mac.send(messageobject)<0){

			 ::logging::log::emit()
           << PROGMEMSTRING("couldn't transmit message because medium was busy...") 
           << ::logging::log::endl << ::logging::log::endl;

	}

	//delay_ms(1000);

    } while (true);
}

