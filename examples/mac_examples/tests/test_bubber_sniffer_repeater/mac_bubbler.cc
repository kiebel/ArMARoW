
#define MAC_LAYER_VERBOSE_OUTPUT false

#include "armarow/mac/mac_csma_ca.h"

#include "util.h"


#undef LOGGING_DISABLE


/* === globals ============================================================== */

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=45
	};

};




armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Disable> mac;

My_Information a;

armarow::MAC::mob_t messageobject;


void transmission_completed_callback(){

//secure callback, so it can't be interrupted
avr_halib::locking::GlobalIntLock lock;

::logging::log::emit()
            << PROGMEMSTRING("messageobject...") << (int) mac.get_result_of_last_send_operation()
            << ::logging::log::endl;

	if(mac.get_result_of_last_send_operation()!=0)
	::logging::log::emit()
            << PROGMEMSTRING("ERROR: couldn't transmit last message...") 
            << ::logging::log::endl;


}


void async_sending_test(){

  mac.onSend_Operation_Completed_Delegtate.bind<transmission_completed_callback>();

  int ret;

  bool verbose = true;

  //armarow::MAC::mob_t messageobject=messageobject;

  while(1){

	{

        //messageobject.header

	messageobject.store_object_in_payload(a);

        ret=mac.send_async(messageobject);
     
     	//we send the previous message header with this message 
	a.header=messageobject.header;

	
	a.messwert1=123;



	if(verbose && ret==0){

	avr_halib::locking::GlobalIntLock lock;

	::logging::log::emit()
	    << (int) a.global_sequence_number 
            //<< PROGMEMSTRING("ret is: ") << ret
            << ::logging::log::endl;

	a.global_sequence_number++;

	}

	}

     //delay_ms(200);

  }

}



/* === main ================================================================= */
int main() {

	    ::logging::log::emit()
            << PROGMEMSTRING("Sending the following message: 123")
            << ::logging::log::endl << ::logging::log::endl;


	//set destination
	messageobject.header.dest_adress = 12; //adress of repeater //armarow::MAC::MAC_BROADCAST_ADRESS;
	messageobject.header.dest_pan = 0;

	messageobject.header.controlfield.ackrequest = 1; //0; //1;



	a.messwert1=123;


	messageobject.store_object_in_payload(a);

	

    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting bubbler (repeated send of the same message)!")
        << ::logging::log::endl << ::logging::log::endl;


    async_sending_test();

}

