

#ifndef __MAC__
#define __MAC__


//#include "attributes.h"
//#include <avr/stdlib.h>
#include <avr/io.h>

//#define size_t int

/* === amarow includes ============================================================= */
#include "platform-cfg.h"               // platform dependent software config



#include "avr-halib/share/delay.h"      // delays and timings
#include "avr-halib/share/delegate.h"   //Delegator for user defined interrupt service routines
#include "avr-halib/share/interruptLock.h"  //Global Lock


#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging
#include "armarow/phy/phy.h"            // physical layer

/* CLOCK */

#include <avr-halib/regmaps/local.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/portmaps/icstick_portmap.h>
#include <avr-halib/ext/led.h>
#include <avr-halib/ext/button.h>

//#include <avr-halib/avr/newTimer.h>
#include <avr-halib/avr/timer.h>
#include <avr-halib/avr/regmaps.h> //ale regmaps aus non experimental Zweig

//AVR includes
#include <stdlib.h>

#include "../common.h"
#include "mac_evaluation.h"

//typedef avr_halib::power::Morpheus<MorpheusSyncList> Morpheus;

//activates clock interrupt
UseInterrupt(SIG_OUTPUT_COMPARE1A);
//UseInterrupt(SIG_OUTPUT_COMPARE2A);

using avr_halib::drivers::Clock;

using namespace avr_halib::regmaps;
/**/

/*globales placement new*/
void* operator new (size_t , void* buffer){

return buffer;

} 


#define LOGGING_DISABLE


#include "mac_message.h"


namespace armarow{
	/*! \brief This namespace contains all Mac Layer specific classes.*/
	namespace MAC{

	typedef MAC_Message mob_t;

/*!
  \brief The configuration of the acknolagement timeout timer.
  It is actually a little hack to use a class intended for periodic timer interrupts as one shot timer, but since we don't have a timer framework, we use this little trick. The idea is, that the acknolagment timeout is 20 millisecs, so we need a frequency of 50. After the first timer interupt occures, we stop the timer and reset it, so that it starts from the beginning again if neccessary.
*/
	struct ClockConfig
	{
		typedef uint16_t TickValueType;
		typedef Frequency<50> TargetFrequency; //every 20 ms one timer interrupt for acknolagement timeout (1000 times per second is 1 ms, we want every 20 ms an interrupt (if the timer is running) and then we have 1/0.020s = 50 times per second)
		typedef CPUClock TimerFrequency; //CPUClock
		typedef local::Timer1 Timer;
	};

		typedef Clock<ClockConfig> MAC_Clock;
		typedef void* AttributType;
		typedef uint16_t DeviceAddress; 
		

		//template<class Radiocontroller>
		//class MAC_Base : public Radiocontroller {
		//TODO: implement template based inheritence mechanism


		//template <uint8_t channel>
		//class MAC_Base : public Radiocontroller {
/*!
 \brief This is the interface class of every Mac Protocol. It should provide all neccessary methods that a Mac protocol should have.
 there are two template parameters. The first one is the Physical Layer. The second one is the activation status of the evaluation feature. This class then inherit from the Physical Layer and the Mac_Evaluation class(Second template parameter of this class is passed as a template parameter of Mac_Evaluation.)
*/
		template<class Radiocontroller,MAC_EVALUATION_ACTIVATION_STATE state>
		struct MAC_Base : public Radiocontroller,Mac_Evaluation<state>{

				enum maxwaitingtime{maximal_waiting_time_in_milliseconds=100};

				typename Radiocontroller::mob_t message; //= {0,{0}};				
				//Radiocontroller  rc;
				uint8_t channel;                   // channel number the node is sending and receiving data
				uint16_t nav; //network allocation vector -> Zeitdauer, die das Medium voraussichtlich belegt sein wird

				uint16_t clocktick_counter; 

				// CLOCK 
				MAC_Clock clock;

				DeviceAddress mac_adress_of_node;

				enum mac_attributes{TA,C,S};




			//public:




				MAC_Base(){   // : channel(11), mac_adress(0){
					channel=11;
					mac_adress_of_node=28;          //this parameter can be configured 

					//maximal_waiting_time_in_milliseconds=100;
					init();	

				}

				/*! receiver Thread, if the mac protocol needs an asyncron receive routine*/
				void callback_receive_message(){



				}

				uint8_t get_global_sequence_number(){

        				static uint8_t global_sequencenumber=0; //will overflow all 256 MAC Frames

					return global_sequencenumber++;

				}

				int init(){
					//message={0,{0}};
					Radiocontroller::init();
					Radiocontroller::setAttribute(armarow::PHY::phyCurrentChannel, &channel);
					Radiocontroller::setStateTRX(armarow::PHY::rx_on);
					
					//init valuation framework
					Mac_Evaluation<state>::init();
					

					//sets the seed value for the pseudo random numbers used for a random waiting time for medium access controll
					srandom(mac_adress_of_node);
					clocktick_counter=0;
					
					nav=0;


					return 0;
				}

				int reset(){

					Radiocontroller::reset();
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


				int send_async(MAC_Message& mac_message){
					return 0;
				}

				int send(MAC_Message mac_message){

				        mac_message.header.sequencenumber=get_global_sequence_number();

					//random waiting time (from 0 to 100 ms) -> should be adjusted for real usage
					int randomnumber = rand();
					uint32_t waitingtime = ( ((uint32_t)randomnumber*maximal_waiting_time_in_milliseconds) / (0x8000)); //0x8000 = RAND_MAX+1 -> Optimization, so that we can do a shift instead of a division

					delay_ms(waitingtime);	
			

					::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "Sending MAC_Message... " << ::logging::log::endl;
					mac_message.print();


					//for a Clear Channel assesment we need to change into Receive State
					Radiocontroller::setStateTRX(armarow::PHY::rx_on);

					uint8_t ccaValue;
					armarow::PHY::State status=Radiocontroller::doCCA(ccaValue);

					if(status==armarow::PHY::idle){

						::logging::log::emit()
							<< PROGMEMSTRING("Medium frei!!!")
							<< ::logging::log::endl << ::logging::log::endl;						
						

					}else if (status==armarow::PHY::busy){

						::logging::log::emit()
							<< PROGMEMSTRING("Medium belegt!!!")
							<< ::logging::log::endl << ::logging::log::endl;
					
							return -1;

					}else if (status==armarow::PHY::trx_off){
						
						
						::logging::log::emit()
							<< PROGMEMSTRING("Controller nicht im Receive State!!!")
							<< ::logging::log::endl << ::logging::log::endl;	
							return -1;
					}else{

						::logging::log::emit()
							<< PROGMEMSTRING("armarow::PHY::State return Value of Clear channel Assessment not in {busy,idle,trx_off}!!!")
							<< ::logging::log::endl << ::logging::log::endl;

					return -1;	
					}

					//we want to send (tranceiver on)
					Radiocontroller::setStateTRX(armarow::PHY::tx_on);

					Radiocontroller::send(*mac_message.getPhysical_Layer_Message());

					//Radiocontroller::setStateTRX(armarow::PHY::TX_OFF);
					//::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					//<< "End of SEND Methode reached" <<::logging::log::endl;
					return 0;
				}


				int receive(MAC_Message& mac_message){

					mac_message.setPayloadNULL();


					Radiocontroller::setStateTRX(armarow::PHY::rx_on);
					//Radiocontroller::receive_blocking(message);


					Radiocontroller::receive(message);

					armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(message);

					if(mac_msg == (armarow::MAC::MAC_Message*) 0 ) return 0;

					mac_message = *mac_msg;
					if(mac_message.header.controlfield.frametype!=Data) {
						mac_message.print(); //just for debug purposes
						return 0;            //the application is only interested in application data, special packages have to be filtered out
					}

					return mac_msg->size;
				}


			};

		} //end namespace mac

	} //end namespace armarow



#endif


