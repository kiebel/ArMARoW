

#include "armarow/mac/mac_csma_ca.h"

#include "../util.h"



struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=20
	};

};



armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;


armarow::MAC::mob_t messageobject;



#undef LOGGING_DISABLE


MessageLossMeasurement<110> msg_measurement_object;

uint32_t global_sequence_number=0;

//erst notifizieren, und dann receive aufrufen, wo man receive puffer übergibt
void callback_recv() {

 	if(mac.receive(messageobject)!=0){
		
		messageobject.get_object_from_payload(msg_measurement_object);
		/*		
		while(global_sequence_number<msg_measurement_object.global_sequence_number){
			::logging::log::emit()
			<< PROGMEMSTRING("lost message: ") << (int) global_sequence_number++
        		<< ::logging::log::endl; 
		}		

		::logging::log::emit()
    
		<< PROGMEMSTRING("gsnr: ") << (int) msg_measurement_object.global_sequence_number
        	<< ::logging::log::endl;

		global_sequence_number++;
		*/
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
