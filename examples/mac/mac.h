

#ifndef __MAC__
#define __MAC__


//#include "attributes.h"
//#include <avr/stdlib.h>
#include <avr/io.h>

//#define size_t int

/* === amarow includes ============================================================= */
#include "platform-cfg.h"               // platform dependent software config



#include "avr-halib/share/delay.h"      // delays and timings

#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging
#include "armarow/phy/phy.h"            // physical layer

/* CLOCK */
//#include <avr-halib/regmaps/local.h>
//#include <avr-halib/avr/clock.h>
//#include <avr-halib/avr/sleep.h>

#include <avr-halib/regmaps/local.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/portmaps/icstick_portmap.h>
#include <avr-halib/ext/led.h>
#include <avr-halib/ext/button.h>



//activates clock interrupt
UseInterrupt(SIG_OUTPUT_COMPARE1A);

using avr_halib::drivers::Clock;

using namespace avr_halib::regmaps;
/**/

/*globales placement new*/
void* operator new (size_t , void* buffer){

return buffer;

} 


#include "mac_message.h"


namespace armarow{

	namespace MAC{

		/* CLOCK
		struct ClockConfig
		{
			typedef uint16_t TickValueType;
			typedef Frequency<1000> TargetFrequency; //create an interrupt 1000 times a second
			typedef CPUClock TimerFrequency;
			typedef local::Timer1 Timer;
		};

		typedef Clock<ClockConfig> MAC_Clock;
		*/


		typedef void* AttributType;
		typedef uint8_t DeviceAddress; 
		


		//template <uint8_t channel>
		class MAC_Base{

			//T mac_protocoll;

			protected:

				platform::config::mob_t message; //= {0,{0}};
				platform::config::rc_t  rc;
				uint8_t channel;                   // channel number the node is sending and receiving data
				uint16_t nav; //network allocation vector -> Zeitdauer, die das Medium voraussichtlich belegt sein wird
				/* CLOCK 
				MAC_Clock clock;
				*/

				DeviceAddress mac_adress;

				//enum MessageType{RTS,CTS,DATA,ACK};

				//TODO: put real message sizes here
				//enum MessageSize{RTS_SIZE=14,CTS_SIZE=14,DATA_SIZE=126,ACK_SIZE=14};

				void sendControlMessage(MessageType msgtype, DeviceAddress receiver){


					/*
					struct MAC_Message* msg = (struct MAC_Message*) message.payload;

					if(msgtype==RTS){

						message.size=RTS_SIZE;
						msg->messagetyp=RTS; //RTS

						msg->messagetype.rts.sender_mac_adress=mac_adress;
						msg->messagetype.rts.receiver_mac_adress=receiver;

					}else if(msgtype==CTS){

						message.size=CTS_SIZE;
						msg->messagetyp=CTS; //RTS

						msg->messagetype.cts.sender_mac_adress=mac_adress;
						msg->messagetype.cts.receiver_mac_adress=receiver;

					}else if(msgtype==ACK){

						message.size=ACK_SIZE;
						msg->messagetyp=ACK; //RTS

						msg->messagetype.ack.sender_mac_adress=mac_adress;
						msg->messagetype.ack.receiver_mac_adress=receiver;

					}
					*/

					/*

					rc.setStateTRX(armarow::PHY::TX_ON);

					armarow::PHY::State status;


					for(int i=0;i<3;i++){
						if((status=rc.send(message))==armarow::PHY::SUCCESS) break;

					}

					if(armarow::PHY::SUCCESS != status){
						::logging::log::emit()
							<< PROGMEMSTRING("tried to send message 3 times and always failed!!!")
							<< ::logging::log::endl << ::logging::log::endl;
					}

					rc.setStateTRX(armarow::PHY::SUCCESS);

					*/

				}

				void sendDATA(DeviceAddress receiver,void* buffer,size_t buffersize){


				}




				enum mac_attributes{TA,C,S};

				


			public:




				MAC_Base(){   // : channel(11), mac_adress(0){
					channel=11;
					init();	

				}




				/*receiver Thread, if the mac protocol needs an asyncron receive routine*/
				static void callback_receive_message(){



				}


				void callback_periodic_timer_activation_event(){

					//MAC* mythis;// = MAC.pointer_auf_sich_selbst;
					//decrement network allocation vector, because we "wait" for nav == 0, if we want to send a message
					//if(nav>0) nav--;

					/* CLOCK
					MAC_Clock::Time t;
					clock.getTime(t);
					::logging::log::emit() << "Tick: " << t.ticks << ", " << t.microTicks << ::logging::log::endl;
					*/

				}


				int init(){
					//message={0,{0}};
					rc.init();
					rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
					rc.setStateTRX(armarow::PHY::RX_ON);
					rc.onReceive.bind<callback_receive_message>();

					//registerCallback<T, &T::onConversionComplete>(t);

					// Set a method as timer event handler
					//setDelegateMethod(b.timer.onTimerDelegate, Blinker, Blinker::onTimer1, b);
					//setDelegateMethod(clock.onTimerDelegate, MAC_Message, MAC_Message::callback_periodic_timer_activation_event, *this);


					return 0;
				}

				int reset(){

					rc.reset();
					init();

					return 0;
				}



				//void set_MAC_Attribut(MAC_Attribut attribute, MAC_Attribute_Value value);  
				//void set_MAC_Attribut(mac_attributes attributes, MAC_Attribute_Value<int> value);
				//template <class AttributType>

				void get_MAC_Attribut(mac_attributes attributes,  AttributType value){

				}

				void set_MAC_Attribut(mac_attributes attributes,  AttributType value){

				}


				int send(char* buffer,size_t buffer_size){

				

			DeviceAddress sender=25;
			DeviceAddress receiver=38;
					
			//int size=buffersize;
			unsigned int offset=0;

			unsigned int buffersize = buffer_size;

			//while(buffersize>0){
			while(offset<buffer_size){


		//int counterlimit=min(buffersize,MAX_NUMBER_OF_DATABYTES);

			uint8_t counterlimit;   //MAX_NUMBER_OF_DATABYTES <= 255 !!!!!!!!!!!!
			if(buffersize<MAX_NUMBER_OF_DATABYTES) counterlimit=(uint8_t)buffersize; else counterlimit=MAX_NUMBER_OF_DATABYTES;


	//for(int i=0;i<counterlimit;i++){
					//create new Message object 
					MAC_Message mac_message1(armarow::MAC::DATA,sender,receiver,&buffer[offset],counterlimit);

					::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "Sending MAC_Message... " << ::logging::log::endl;
					mac_message1.print();

					
					//wait a random time
					delay_ms(1000); //TODO: make random time!!!


					//for a Clear Channel assesment we need to change into Receive State
					rc.setStateTRX(armarow::PHY::RX_ON);

					armarow::PHY::State status=rc.doCCA();

					if(status==armarow::PHY::IDLE){

						::logging::log::emit()
							<< PROGMEMSTRING("Medium frei!!!")
							<< ::logging::log::endl << ::logging::log::endl;						
						

					}else if (status==armarow::PHY::BUSY){

						::logging::log::emit()
							<< PROGMEMSTRING("Medium belegt!!!")
							<< ::logging::log::endl << ::logging::log::endl;
					
					}else if (status==armarow::PHY::TRX_OFF){
						
						
						::logging::log::emit()
							<< PROGMEMSTRING("Controller nicht im Receive State!!!")
							<< ::logging::log::endl << ::logging::log::endl;	
	
					}else{

						::logging::log::emit()
							<< PROGMEMSTRING("armarow::PHY::State return Value of Clear channel Assessment not in {BUSY,IDLE,TRX_OFF}!!!")
							<< ::logging::log::endl << ::logging::log::endl;	
					}


					//we want to send (tranceiver on)
					rc.setStateTRX(armarow::PHY::TX_ON);

					rc.send(*mac_message1.getPhysical_Layer_Message());


/*
					for(int i=0;i<3;i++){
						if((status=rc.send(*mac_message1.getPhysical_Layer_Message()))==armarow::PHY::SUCCESS) break;

					}

					if(armarow::PHY::SUCCESS != status){
						::logging::log::emit()
							<< PROGMEMSTRING("tried to send message 3 times and always failed!!!")
							<< ::logging::log::endl << ::logging::log::endl;
					}
*/
					

				offset += counterlimit;
				if(buffersize>MAX_NUMBER_OF_DATABYTES) buffersize -= MAX_NUMBER_OF_DATABYTES; else buffersize=0;

					::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "[DEBUG] Counterlimit: " << counterlimit << " Offset: " << offset << " buffersize: " << buffersize <<::logging::log::endl;

				}

					//rc.setStateTRX(armarow::PHY::TX_OFF);
					::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "End of SEND Methode reached" <<::logging::log::endl;
					return offset;
				}

				/*
				 * \brief receive message
				 * 
				 * returns number of readed bytes, returns 0 no more bytes where received or an error occured
				 * exmaple: while(numberofreceivedbytes=receive(buffer,sizeof(buffer))>0){ ... }
				 */
				int receive(char* buffer,size_t buffersize){


					if(buffer==(char*)0){

						::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "[ERROR] in MAC::MAC_Base::receive(): the pointer to the buffer for received data is Null!!!" << buffersize <<::logging::log::endl;

					}					

					if(buffersize<MAX_NUMBER_OF_DATABYTES){

						::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "the size of the databuffer you specified is to little! Minimal Value: " << MAX_NUMBER_OF_DATABYTES << " your Value: " << buffersize <<::logging::log::endl;

					}

					//set the receive buffer to zero
					for(unsigned int i=0;i<buffersize;i++){

						buffer[i]='\0';

					}


					rc.setStateTRX(armarow::PHY::RX_ON);
					rc.receive_blocking(message);

					armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(message);

					if(!mac_msg) return 0;

					//mac_msg->print();

					//size_t numberofbytestocopy=0;
					//if(buffersize>mac_msg->size) numberofbytestocopy=mac_msg->size

					//copy received data in User Application buffer
					for(unsigned int i=0;i<mac_msg->size;i++){

						buffer[i]=mac_msg->payload[i];

					}


					return mac_msg->size;
				}

				
				

				};


		} //end namespace mac

	} //end namespace armarow



#endif


