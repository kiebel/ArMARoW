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

#undef LOGGING_DISABLE

/* === globals ============================================================== */

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=45
	};

};




armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Disable> mac;



void transmission_completed_callback(){

//::logging::log::emit()
//            << PROGMEMSTRING("Successful send a message...")
//            << ::logging::log::endl;

//secure callback, so it can't be interrupted
avr_halib::locking::GlobalIntLock lock;

::logging::log::emit()
            << PROGMEMSTRING("msg...") << (int) mac.get_result_of_last_send_operation()
            << ::logging::log::endl;

	if(mac.get_result_of_last_send_operation()!=0)
	::logging::log::emit()
            << PROGMEMSTRING("ERROR: couldn't transmit last message...") 
            << ::logging::log::endl;


}


void async_sending_test(armarow::MAC::mob_t msg){

  mac.onSend_Operation_Completed_Delegtate.bind<transmission_completed_callback>();

  int ret;

  bool verbose = false;

  while(1){

	//test for one shot timer
	//mac.send_async(msg);
	//while(1);
     
	   //::logging::log::emit()  << PROGMEMSTRING("Try sending a message...") << ::logging::log::endl;
     {

        ret=mac.send_async(msg);
     
	

	if(verbose){

	avr_halib::locking::GlobalIntLock lock;

	::logging::log::emit()
            << PROGMEMSTRING("ret is: ") << ret
            << ::logging::log::endl;

	}

	}

     //delay_ms(1000);

  }

}

struct My_Information{

	//char data[100];

	int messwert1;
	int messwert2;
	int messwert3;

	My_Information(){

	    messwert1=10;
	    messwert2=20;
	    messwert3=30;


	}

	void print(){

	    ::logging::log::emit() << "messwert1: " << messwert1 << ::logging::log::endl;
	    ::logging::log::emit() << "messwert2: " << messwert2 << ::logging::log::endl;
	    ::logging::log::emit() << "messwert3: " << messwert3 << ::logging::log::endl;

	}
	
} a;

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

	//init message with your data and set destination 
 for(unsigned int i=0;i<sizeof(messagecontent);i++)
	messageobject.payload[i]=messagecontent[i];   //copy data in message payload

	messageobject.size=sizeof(messagecontent);

	//set destination
	messageobject.header.dest_adress = 20; //armarow::MAC::MAC_BROADCAST_ADRESS;
	messageobject.header.dest_pan = 0;

	messageobject.header.controlfield.ackrequest = 0;//1;


	/* //example for usage of object storage functionality
            a.messwert1=40;
	    a.messwert2=50;
	    a.messwert3=60;

	::logging::log::emit() << "Object a: " << ::logging::log::endl;
	a.print();

	messageobject.store_object_in_payload(a);

	My_Information b;

	::logging::log::emit() << "Object b before: " << ::logging::log::endl;
	b.print();

	messageobject.get_object_from_payload(b);

	::logging::log::emit() << "Object b after: " << ::logging::log::endl;
	b.print();

	//end of example
	*/


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

