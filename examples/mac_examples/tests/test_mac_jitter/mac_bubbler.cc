
#include "armarow/mac/mac_csma_ca.h"

#include "armarow/mac/util.h"

#undef LOGGING_DISABLE

/* === globals ============================================================== */

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=45
	};

};




armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;



void transmission_completed_callback(){


avr_halib::locking::GlobalIntLock lock;

::logging::log::emit()
            //<< PROGMEMSTRING("msg...") 
	    << "," << (int) mac.get_result_of_last_send_operation()
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

	MessageLossMeasurement<110> msg_measurement_object;

	msg_measurement_object.global_sequence_number=0;


  while(1){
	msg.store_object_in_payload(msg_measurement_object);
	
     {

	//msg.store_object_in_payload(msg_measurement_object);

	ret=mac.send(msg);

	if(ret==0){
	   avr_halib::locking::GlobalIntLock lock;
	   //::logging::log::emit()
           // << msg_measurement_object.global_sequence_number
           // << ::logging::log::endl;
	   //	msg_measurement_object.global_sequence_number++;
	   delay_ms(50);
	}


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

	messageobject.header.controlfield.ackrequest = 0;

    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting bubbler (repeated send of the same message)!")
        << ::logging::log::endl << ::logging::log::endl;


    async_sending_test(messageobject);

}

