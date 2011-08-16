

#define MAC_LAYER_VERBOSE_OUTPUT false

#include "armarow/mac/mac_csma_ca.h"

#include "util.h"


/* === globals ============================================================== */

struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=28
	};

};



armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;


armarow::MAC::mob_t messageobject;


My_Information a;

#undef LOGGING_DISABLE

//erst notifizieren, und dann receive aufrufen, wo man receive puffer übergibt
void callback_recv() {

 	if(mac.receive(messageobject)!=0){
		
		::logging::log::emit()
        	//<< PROGMEMSTRING("[Content:] ") << messageobject.payload << ::logging::log::endl
		<< PROGMEMSTRING("Node ID: ") << (int) messageobject.header.source_adress
		<< PROGMEMSTRING("msnr: ") << (int) messageobject.header.sequencenumber << ::logging::log::endl
		<< "r ED:" << (int) messageobject.minfo.ed << log::endl  //from repeater
	    	<< "r LQI:" << (int) messageobject.minfo.lqi
        	<< ::logging::log::endl; // << ::logging::log::endl;


		messageobject.get_object_from_payload(a);

		a.print();

		::logging::log::emit()
		<< "gsnr: " << a.global_sequence_number
        	<< ::logging::log::endl; // << ::logging::log::endl;

		::logging::log::emit()
		<< "b ED:" << (int) a.ed << log::endl	//from bubbler
		<< "b LQI:" << (int) a.lqi << log::endl;


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

   //sychron receive test
   



}
