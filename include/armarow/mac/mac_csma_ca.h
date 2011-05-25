

#ifndef __MAC_CSMA_CA__
#define __MAC_CSMA_CA__


#include "mac.h"


namespace armarow{

	namespace MAC{

	typedef MAC_Message mob_t;

		// CLOCK
	/*
	struct ClockConfig
	{
		typedef uint16_t TickValueType;
		typedef Frequency<1> TargetFrequency;
		typedef Frequency<32768> TimerFrequency; //CPUClock
		typedef local::Timer2 Timer;
	};

		typedef Clock<ClockConfig> MAC_Clock;
	*/	


		typedef void* AttributType;
		typedef uint16_t DeviceAddress; 
		



		//template <uint8_t channel>
		class MAC_CSMA_CA : public MAC_Base{

		

			protected:


				//enum maxwaitingtime{maximal_waiting_time_in_milliseconds=100};
				//enum mac_attributes{TA,C,S};


				//uint16_t nav; //network allocation vector -> Zeitdauer, die das Medium voraussichtlich belegt sein wird


				// CLOCK 
				MAC_Clock clock;
				
				//since we can either send orreceive, but not both at the same time, we just need one buffer
				MAC_Message send_receive_buffer;
				platform::config::rc_t::mob_t physical_layer_receive_buffer;

				//we don't want to deliver the same message twice, so we need a flag for that
				volatile bool has_message_ready_for_delivery; //and we declare it as volatile, so that the compiler doesn't do anything fishy to it (optimization)

				


				


			public:

				Delegate<> onMessageReceiveDelegate;


				MAC_CSMA_CA(){   // : channel(11), mac_adress(0){
					channel=11;
					mac_adress_of_node=28;          //this parameter can be configured 

					//maximal_waiting_time_in_milliseconds=100;
					init();	

				}




				/*receiver Thread, if the mac protocol needs an asyncron receive routine*/
				void callback_receive_message(){


					platform::config::rc_t::receive(physical_layer_receive_buffer);

					has_message_ready_for_delivery=true;
				

					armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(physical_layer_receive_buffer);

					if(mac_msg == (armarow::MAC::MAC_Message*) 0 ) {

						::logging::log::emit() << "has_message_ready_for_delivery=false" << ::logging::log::endl;

						has_message_ready_for_delivery=false; //message is somehow invalid 
						return;

					}

					send_receive_buffer = *mac_msg;
					if(send_receive_buffer.header.controlfield.frametype!=DATA) {
						send_receive_buffer.print(); //just for debug purposes
						has_message_ready_for_delivery=false;  //the application is only interested in application data, special packages have to be filtered out
						::logging::log::emit() << "has_message_ready_for_delivery=false" << ::logging::log::endl;
						return;
					}

					has_message_ready_for_delivery=true;
					//send_receive_buffer.print(); 
					//if we reach this instruction, everything went well and we can call a user defined interrupt service routine
					if(!onMessageReceiveDelegate.isEmpty()) onMessageReceiveDelegate();

				}

				void callback_periodic_timer_activation_event(){

					
					//decrement network allocation vector, because we "wait" for nav == 0, if we want to send a message
					//if(nav>0) nav--;

					/* CLOCK
					MAC_Clock::Time t;
					clock.getTime(t);
					::logging::log::emit() << "Tick: " << t.ticks << ", " << t.microTicks << ::logging::log::endl;
					*/


/*
						int randomnumber = rand();
					
					//delay_ms((randomnumber*maximal_waiting_time_in_milliseconds)/RAND_MAX);	
				//led.toggle();


					::logging::log::emit() << "es ist eine weitere Sekunde vergangen... Random Number: " << randomnumber << " normalized Random Number: " << ( ((uint32_t)randomnumber*maximal_waiting_time_in_milliseconds) / (0x8000)) << " MAX RANDOM NUMBER: " << RAND_MAX << ::logging::log::endl;				
*/

					led.toggle();



				}


				int init(){
					//message={0,{0}};

		
					//Basic initialization
					MAC_Base::init();


					/*
					platform::config::rc_t::init();
					platform::config::rc_t::setAttribute(armarow::PHY::phyCurrentChannel, &channel);
					platform::config::rc_t::setStateTRX(armarow::PHY::RX_ON);
					
					//platform::config::rc_t::onReceive.bind<callback_receive_message>();

					//registerCallback<T, &T::onConversionComplete>(t);

					//sets the seed value for the pseudo random numbers used for a random waiting time for medium access controll
					srandom(mac_adress_of_node);
					*/
					
					//typeof *this = MAC_CSMA_CA
					clock.registerCallback<typeof *this, &MAC_CSMA_CA::callback_periodic_timer_activation_event>(*this);
					setDelegateMethod(platform::config::rc_t::onReceive,MAC_CSMA_CA,MAC_CSMA_CA::callback_receive_message,*this);

					has_message_ready_for_delivery=false;


					// Set a method as timer event handler
					//setDelegateMethod(b.timer.onTimerDelegate, Blinker, Blinker::onTimer1, b);

					//setDelegateMethod(clock.timer.onTimerDelegate, MAC_Base, MAC_Base::callback_periodic_timer_activation_event, *this);


					return 0;
				}

				int reset(){

					//here we need to call the radio controller directly, because a MAC_Base::init() wouldn't consider the extensions from this class
					platform::config::rc_t::reset();
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

				int send(MAC_Message mac_message){

				        mac_message.header.sequencenumber=get_global_sequence_number();

					//random waiting time (from 0 to 100 ms) -> should be adjusted for real usage
					int randomnumber = rand();
					uint32_t waitingtime = ( ((uint32_t)randomnumber*maximal_waiting_time_in_milliseconds) / (0x8000)); //0x8000 = RAND_MAX+1 -> Optimization, so that we can do a shift instead of a division

					delay_ms(waitingtime);	
			

					::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					<< "Sending MAC_Message... " << ::logging::log::endl;
					mac_message.print();


					//for a Clear Channel assessment we need to change into Receive State
					platform::config::rc_t::setStateTRX(armarow::PHY::RX_ON);

					armarow::PHY::State status=platform::config::rc_t::doCCA();

					if(status==armarow::PHY::IDLE){

						::logging::log::emit()
							<< PROGMEMSTRING("Medium frei!!!")
							<< ::logging::log::endl << ::logging::log::endl;						
						

					}else if (status==armarow::PHY::BUSY){

						::logging::log::emit()
							<< PROGMEMSTRING("Medium belegt!!!")
							<< ::logging::log::endl << ::logging::log::endl;
					
							return -1;

					}else if (status==armarow::PHY::TRX_OFF){
						
						
						::logging::log::emit()
							<< PROGMEMSTRING("Controller nicht im Receive State!!!")
							<< ::logging::log::endl << ::logging::log::endl;	
							return -1;
					}else{

						::logging::log::emit()
							<< PROGMEMSTRING("armarow::PHY::State return Value of Clear channel Assessment not in {BUSY,IDLE,TRX_OFF}!!!")
							<< ::logging::log::endl << ::logging::log::endl;

					return -1;	
					}


				




					//we want to send (tranceiver on)
					platform::config::rc_t::setStateTRX(armarow::PHY::TX_ON);

					platform::config::rc_t::send(*mac_message.getPhysical_Layer_Message());

					//after sending we need to change in the Transive mode again, so that we get received messages per interrupt
					platform::config::rc_t::setStateTRX(armarow::PHY::RX_ON);

					//platform::config::rc_t::setStateTRX(armarow::PHY::TX_OFF);
					//::logging::log::emit() << ::logging::log::endl << ::logging::log::endl 
					//<< "End of SEND Methode reached" <<::logging::log::endl;
					return 0;
				}


				/*

					receive blocks until a message is received (or it returns an alredy received, but not delivered message (delivered with respect to the application))
				*/

				int receive(MAC_Message& mac_message){


					mac_message.setPayloadNULL();


					platform::config::rc_t::setStateTRX(armarow::PHY::RX_ON);
					//platform::config::rc_t::receive_blocking(message);


					//TODO: replace busy wait with something like sleep that wakes up if an interupt occures to avoid energy waste
					while(has_message_ready_for_delivery==false){

						//if(has_message_ready_for_delivery) 
						//::logging::log::emit() << "has_message_ready_for_delivery=" << (int) has_message_ready_for_delivery << ::logging::log::endl;


						//delay_ms(1);

					}

					//if we get here, an interrupt occoured in the meantime, and we can deliver a message
					//the first thing we do is setting the value false again and returning then the message

					has_message_ready_for_delivery=false;


					//platform::config::rc_t::receive(message);

					//armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(message);
					mac_message = send_receive_buffer;
						/*::logging::log::emit()
							<< PROGMEMSTRING("leaving receive method...")
							<< ::logging::log::endl << ::logging::log::endl;
						*/
					return mac_message.size;
				}





			};


		} //end namespace mac

	} //end namespace armarow



#endif


