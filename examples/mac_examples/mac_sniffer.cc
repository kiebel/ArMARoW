

#define MAC_LAYER_VERBOSE_OUTPUT false


#include "armarow/mac/mac_csma_ca.h"

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

//erst notifizieren, und dann receive aufrufen, wo man receive puffer übergibt
void callback_recv() {

	avr_halib::locking::GlobalIntLock lock;

 	if(mac.receive(messageobject)!=0){
		
		::logging::log::emit()
        	//<< PROGMEMSTRING("[Content:] ") << messageobject.payload << ::logging::log::endl
		//<< PROGMEMSTRING("Node ID: ") << (int) messageobject.header.source_adress
		//<< PROGMEMSTRING("Message Sequence Number: ") << (int) messageobject.header.sequencenumber
		<< (int) messageobject.header.source_adress << "," <<(int) messageobject.header.sequencenumber
        	<< ::logging::log::endl; // << ::logging::log::endl;
		
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
