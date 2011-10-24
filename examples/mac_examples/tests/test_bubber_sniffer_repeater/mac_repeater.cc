
#define MAC_LAYER_VERBOSE_OUTPUT false

#include "armarow/mac/mac_csma_ca.h"

#include "util.h"

#undef LOGGING_DISABLE


/* === globals ============================================================== */

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=12
	};

};




armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;

My_Information a;

armarow::MAC::mob_t messageobject;

void callback_recv() {

	int ret=0;

	if(mac.receive(messageobject)!=0){
		
		::logging::log::emit()
		//<< PROGMEMSTRING("[Content:] ") << messageobject.payload << ::logging::log::endl
		<< PROGMEMSTRING("Node ID: ") << (int) messageobject.header.source_adress << ::logging::log::endl
		<< PROGMEMSTRING("msnr: ") << (int) messageobject.header.sequencenumber << ::logging::log::endl
		<< "ED:" << (int) messageobject.minfo.ed << log::endl
		<< "LQI:" << (int) messageobject.minfo.lqi
		<< ::logging::log::endl; // << ::logging::log::endl;


		messageobject.get_object_from_payload(a);

		a.print();

		::logging::log::emit()
		<< "gsnr: " << a.global_sequence_number
        	<< ::logging::log::endl; // << ::logging::log::endl;


		//set destination
		messageobject.header.dest_adress = 28; //adress of repeater //armarow::MAC::MAC_BROADCAST_ADRESS;
		messageobject.header.dest_pan = 0;

		a.messwert1=32;

		//store in payload
		a.ed=messageobject.minfo.ed;
		a.lqi=messageobject.minfo.lqi;

		a.header=messageobject.header;

		messageobject.store_object_in_payload(a);


		ret=mac.send(messageobject);

		//we send the previous message header with this message 
		a.header=messageobject.header;


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

/*
void async_sending_test(){

  mac.onSend_Operation_Completed_Delegtate.bind<transmission_completed_callback>();

  int ret;

  bool verbose = false;

  //armarow::MAC::mob_t messageobject=messageobject;

  while(1){

	{

        //messageobject.header

	messageobject.store_object_in_payload(a);

        ret=mac.send(messageobject);
     
     	//we send the previous message header with this message 
	a.header=messageobject.header;

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
*/


/* === main ================================================================= */
int main() {

	    ::logging::log::emit()
            << PROGMEMSTRING("Repeater: sending different value then that of the bubbler")
            << ::logging::log::endl << ::logging::log::endl;


	//set destination
	messageobject.header.dest_adress = 20; //armarow::MAC::MAC_BROADCAST_ADRESS;
	messageobject.header.dest_pan = 0;

	messageobject.header.controlfield.ackrequest = 0;



	a.messwert1=32;


	messageobject.store_object_in_payload(a);

	

    sei();                              // enable interrupts
   

    //async_sending_test();

    mac.onMessageReceiveDelegate.bind<callback_recv>();
    while(1);


}

