
#define MAC_LAYER_VERBOSE_OUTPUT false

#include "armarow/mac/mac_csma_ca.h"

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


  //FIXME: TODO: Test

 //while(1); //do nothing

 delay_ms(500);

  while(1){


     {

        ret=mac.send_async(msg);
     
	if(ret==0){
		//delay_ms(1000);
	}
	

	if(verbose){

	avr_halib::locking::GlobalIntLock lock;

	::logging::log::emit()
            << PROGMEMSTRING("ret is: ") << ret
            << ::logging::log::endl;

	}

	}

     //delay_ms(100);

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

	messageobject.header.controlfield.ackrequest = 1;//1;


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


}

