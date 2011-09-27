

#ifndef __MAC_CSMA_CA__
#define __MAC_CSMA_CA__

#ifndef MAC_LAYER_VERBOSE_OUTPUT
	/*! \def MAC_LAYER_VERBOSE_OUTPUT                                                                                  
          contains a bool value indicating whether verbose output should be enabled or disabled 
	  Since that is a Mac Layer debugging functionality, it is not represented to the user explicitly. (You won't have any benefit enabling this, since the logging uses the UART and thats is very slow.)                                                                                    
       */
	#define MAC_LAYER_VERBOSE_OUTPUT false //false //true
#endif


#ifndef MAC_VERBOSE_ACK_OUTPUT 
	/*! \def MAC_VERBOSE_ACK_OUTPUT                                                                                 
          contains a bool value indicating whether verbose output concerning the acknolagement mechanism should be enabled or disabled 
	  Since that is a Mac Layer debugging functionality, it is not represented to the user explicitly. (You won't have any benefit enabling this, since the logging uses the UART and thats is very slow.)                                                                                    
       */
	#define MAC_VERBOSE_ACK_OUTPUT false //false //true
#endif


#ifndef ENABLE_FILTERING_OF_DUPLICATES
	/*! \def ENABLE_FILTERING_OF_DUPLICATES                                                                                 
          This enables an experimental feature for duplicate filtering. If you really need it, enable it. Otherwise just keep it false.                                                                                   
       */
	#define ENABLE_FILTERING_OF_DUPLICATES false
#endif


#include "mac.h"
#include "atmega1281_timer3_regmap.h"
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>
//#include <boost/type_traits.hpp>

//#define LOGGING_DISABLE
#undef LOGGING_DISABLE

UseInterrupt(SIG_OUTPUT_COMPARE3A);

//ArMARoW/external/avr-halib/experimental/include/avr-halib/share$
//TODO: GlobalIntlock globaler interrupt lock, zum schützen der Interruptservice routinen voreinander -> einbauen

namespace armarow{

	namespace MAC{

	typedef MAC_Message mob_t;

		/*! \brief ERROR Message neccessary for STATIC ASSERT ERROR MESSAGE to report invalid Configuration at compile time*/
		class INVALID_MAC_CONFIGURATION__TYPE_DOESNT_INHERIT_FROM_CLASS__MAC_CONFIGURATION;

		typedef void* AttributType;
		typedef uint16_t DeviceAddress; 
		

/*! \brief MAC_Configuration: the abstraction of a MAC Configuration, all user defined configurations have to inherit from this class
  This class contains all parameters of the Mac Layer that are configurable.
  This class doesn't need memory at run time, since it exits only at compile time
 */
		struct MAC_Configuration{

			enum {

				channel=11,
				mac_adress_of_node=28, //Node ID	
				pan_id=0,
				ack_request=1,
				minimal_backoff_exponend=2,  //means, we wait 2^0=1 ms until we send a message 
				maximal_backoff_exponend=7,
				maximal_number_of_retransmissions=3,
				promiscuous_mode=0, //if 1 turns out the message filter and accepts all messages 
				acknolagement_timeout_duration_in_ms=100 //timeout event occures after the specified time is exeeded, it will then attempt a retransmission

			};


		};

		enum MAC_Special_Adresses{MAC_BROADCAST_ADRESS=255};

/*! \brief MAC_CSMA_CA: the implementation of the Mac protocol CSMA/CA
  This class inherits from MAC_Base, because all Mac protocols have to have the same interface. (so it inherits the Physical Layer as well)
  The first template parameter is a Configuration class, that inherits from the MAC_Configuration class. It contains all MAC_Layer relevant configuration parameters.
  The desired Physical Layer is the second template parameter, and MAC_Base will inherit from it. 
  The third template parameter indicates the usage of the evaluation features of the Mac protocol. In the MAC namespace there are to constants, Enables and Disable. If you don't want to use the evaluation features (e.g. measuring bandwith), then you should turn this feature of with the Disable parameter. Otherwise you should pass the Enable parameter. 
 */
		template<class MAC_Config,class Radiocontroller,MAC_EVALUATION_ACTIVATION_STATE Mac_Evaluation_activation_state>
		struct MAC_CSMA_CA : public MAC_Base<Radiocontroller,Mac_Evaluation_activation_state>{

			/*!
			    \brief For ease of use and debugging purposes, we make a Typedef for the Mac protocol, so that it can be accessed as MAC_LAYER.
			*/
			typedef MAC_CSMA_CA<MAC_Config,Radiocontroller,Mac_Evaluation_activation_state> MAC_LAYER;

			protected:
				// CLOCK 
				MAC_Clock acknolagement_timeout_timer; //not a one shot timer, we abuse the clock for that ;-)

				ExactEggTimer<Timer3> one_shot_timer;				
				
				MAC_Message receive_buffer;
				typename Radiocontroller::mob_t physical_layer_receive_buffer;
				/*! we don't want to deliver the same message twice, so we need a flag for that and we declare it as volatile, so that the compiler doesn't do anything fishy to it (optimization)*/
				volatile bool has_message_ready_for_delivery;
				/*! bit we need for timer interrupt routine, to decide if there is a message to send (asynchron message delivery)*/
				volatile bool has_message_to_send;
				/*! the send async method will copy the user data in this internal buffer*/
				MAC_Message send_buffer;

				/*!
			          \brief The abstraction of all needed variables to implement a simple Message Filter. We will only remember the last received message to save memory.
				*/
				struct Message_Filter{

					uint8_t sequence_number_of_last_received_message; //= msg.header.sequencenumber;
					uint8_t source_id_of_last_received_message; //= msg.header.dest_adress;
					uint8_t source_panid_of_last_received_message; //= msg.header.dest_pan;

					Message_Filter(){
						//init variables for last message
						sequence_number_of_last_received_message=0;
						source_id_of_last_received_message=255; //Broadcast adress
						source_panid_of_last_received_message=255; 
						
					}


				} message_filter;

				/*!
				  \brief The abstraction of all variables and program logic to implement an acknolagement mechanism in the Mac protocol.
				*/
				struct Acknolagement_Handler{

					enum ACK_ERROR_CODE {success,TIMEOUT,MEDIUM_BUSY};

				//if a maximal waiting time is exceeded, then waits for ack will be set to false and an error is reported
				//received_ack_for_last_transmitted_message is set only if an ack was received, if the timeout occur beforhand, it will still be false and indicates an error -> retransmission, or if number of retransmission exceeds a limit, than report an error

					//volatile bool waits_for_ack; //if false, drop ack immediatly

					//volatile bool received_ack_for_last_transmitted_message;

					//volatile bool initialized_ack_mechanism; //we need that, so we know at the beginning of the programm, if we have an error situation or are in setup state


					volatile struct {

						bool waits_for_ack : 1; //if false, drop ack immediatly

						bool received_ack_for_last_transmitted_message : 1;

						bool initialized_ack_mechanism : 1; //we need that, so we know at the beginning of the programm, if we have an error situation or are in setup state
						bool timeout_occured : 1;

						bool ack_to_send : 1;
					};


				/*!
				  \brief Implements the functionality of the IEEE backoff Timing computation. Neccessary variables and program logic are encapsulated here.
				  The Idea of this class is the structuring of feature related variables and constants in a sub class for better readability and maintainability. (Reducing code scattering.)
				
				*/
				struct Backoff_Timing{

				uint8_t current_backoff_exponend;

				//ieee maximal backoff exponend
				//static const uint8_t maximal_backoff_exponend = 7; //TODO: add IEEE number here -> its a pain finding it in the standard, so we just take this value, since 2^7= 128 ms, that is quite a long backoff time

				//static const uint8_t minimal_backoff_exponend = MAC_Config::minimal_backoff_exponend;

				//uint8_t number_of_backoffs;  //TODO: not really used until now, remove if unneccessary

				//static const uint8_t maximum_number_of_backoffs = 10; //TODO: add IEEE number here


				uint8_t maximal_number_of_retransmissions;

				uint8_t current_number_of_retransmissions;

					Backoff_Timing(){

						//init variables
						this->reset();
						maximal_number_of_retransmissions=MAC_Config::maximal_number_of_retransmissions; //3;

					}

					//idea: if(is_in_boundaries()) oneshottimer.start(get_random_backoff_time_in_ms()); else drop_message();

					//has the exponend grown above the allowed limit?
					/*
					bool is_in_boundaries(){
						if(this->current_backoff_exponend<=maximal_backoff_exponend){
							return true;
						}else{
							return false;
						}
					}
					*/

					bool number_of_backoffs_has_exeeded(){
						if(current_backoff_exponend>MAC_Config::maximal_backoff_exponend){ //number off backoffs exeeded?							
							return true;
						}else{
							return false;
						}
					}


					
					/*!
					  \brief This method is intended for the one shot timer, to get the waiting time easier.
					*/
					uint16_t get_random_backoff_time_in_ms(){
						

						//compute ieee compatible maximal backoff waiting time, that will grow exponentionally with number of backoffs
						uint32_t current_maximal_backofftime = 2^(this->current_backoff_exponend);
						this->current_backoff_exponend++;

						//get random number
						uint32_t randomnumber = rand();

						//0x8000 = RAND_MAX+1 -> Optimization, so that we can do a shift instead of a division
				     		uint32_t random_waitingtime = ( ((uint32_t)randomnumber * current_maximal_backofftime) / (0x8000)); 


						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit()
						<< "random waiting time in ms: " << (int) random_waitingtime 
						<< ::logging::log::endl;

						//return random waiting time as uint16_t, so that the one shot timer can just use it
						return (uint16_t)random_waitingtime;

					}

					
					/*!
					  \brief Method for easily reseting the counters, intended to use in the send function, when you just accepted a new message for transmission.
					*/
					void reset(){
						//set to default value
						current_backoff_exponend=MAC_Config::minimal_backoff_exponend; //0;

						//number_of_backoffs=0;

						current_number_of_retransmissions=0;

					}
				} backoff_timing;

					MAC_Message acknolagement_buffer; //for sending acknolagements
					uint8_t sequence_number_of_last_transmitted_message;
					uint8_t destination_id_of_last_transmitted_message;
					uint8_t destination_panid_of_last_transmitted_message;
					uint8_t timeout_counter_in_ms;
					uint8_t timeout_duration_in_ms;
					ACK_ERROR_CODE result_of_last_send_operation_errorcode;

					Acknolagement_Handler(){//(volatile bool& a_has_message_to_send) : has_message_to_send(a_has_message_to_send){
						reset();

						timeout_duration_in_ms=MAC_Config::acknolagement_timeout_duration_in_ms;//100;
						//maximal_number_of_retransmissions=3;
						initialized_ack_mechanism=false;
						result_of_last_send_operation_errorcode=success;

					}
					/*!
					  \brief resets variables to default value
					*/
					void reset(){


						received_ack_for_last_transmitted_message=false;
						waits_for_ack=false;
						//current_number_of_retransmissions=0;
						timeout_counter_in_ms=0;
						timeout_occured=false;

						ack_to_send=false;

					}
					/*!
					  \brief This is an intern debugging method. 
					*/
					void print(){
						::logging::log::emit() << "received_ack_for_last_transmitted_message: " << (int)received_ack_for_last_transmitted_message << ::logging::log::endl;
						::logging::log::emit() << "waits_for_ack: " << (int)waits_for_ack << ::logging::log::endl;
						::logging::log::emit() << "current_number_of_retransmissions: " << (int)backoff_timing.current_number_of_retransmissions << ::logging::log::endl;
						::logging::log::emit() << "timeout_counter_in_ms: " << (int)timeout_counter_in_ms << ::logging::log::endl;
						::logging::log::emit() << "timeout_counter_in_ms: " << (int) timeout_occured << ::logging::log::endl;
					}

					/*!
					  \brief If an ACK message is received, this method decides, whether its the ACK frame we are waiting for or not.
					  If yes, we set the corrosponding bits, if not we just ignore the ACK.
					  \param ack a reference to the received acknolagement message
					  \param has_message_to_send a reference to a needed bit of the MAC Layer
					  \param onSend_Operation_Completed_Delegtate the reference to a Delegate, that contains the callback function we want to call, to notify the user upon completion
					  \param mac_layer a reference to the Mac Layer, because we need some of its functionality
					*/
					void handle_received_ACK(MAC_Message& ack,volatile bool& has_message_to_send,Delegate<>& onSend_Operation_Completed_Delegtate,MAC_LAYER& mac_layer){
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "ack was received, validating..." << ::logging::log::endl;
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit()
						<< "last message sequence number: " << (int) sequence_number_of_last_transmitted_message << ::logging::log::endl
						<< "ack sequence number: " << (int) ack.header.sequencenumber << ::logging::log::endl;
						if (sequence_number_of_last_transmitted_message == ack.header.sequencenumber)
						//if(send_buffer.header.sequencenumber == ack.header.sequencenumber)
						//&&  destination_id_of_last_transmitted_message == ack.header.dest_adress
						//&& destination_panid_of_last_transmitted_message == ack.header.dest_pan)
						{
							mac_layer.reset_acknolagement_timer();
							//this is the ACK for the last transmitted message
							received_ack_for_last_transmitted_message=true;
							//mechanism has to be initialized again by calling "init_waiting_mechanism_for_ACK_for_MAC_Message"
							initialized_ack_mechanism=false;
							waits_for_ack=false;
							has_message_to_send=false;

							result_of_last_send_operation_errorcode=success;							

							//if(MAC_LAYER_VERBOSE_OUTPUT) 

							if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "received ACK for message " << (int) sequence_number_of_last_transmitted_message << ::logging::log::endl;
							if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "waiting time in ms: " << (int) timeout_counter_in_ms << " current timeout duration: " << (int) timeout_duration_in_ms << ::logging::log::endl;
							if(!onSend_Operation_Completed_Delegtate.isEmpty()) onSend_Operation_Completed_Delegtate();
						}
					}
					/*!
					  \brief initializes the waiting mechanism to wait for an acknolagement message
					  \param msg the transmitted Data message we want an acknolagement message for
					  \param mac_layer a reference to the Mac Layer, because we need some functionality of it
					  \return an error code
					*/
					ACK_ERROR_CODE init_waiting_mechanism_for_ACK_for_MAC_Message(MAC_Message& msg,MAC_LAYER& mac_layer){

						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "wait for ACK..." << ::logging::log::endl;

						sequence_number_of_last_transmitted_message = msg.header.sequencenumber;
						destination_id_of_last_transmitted_message = msg.header.dest_adress;
						destination_panid_of_last_transmitted_message = msg.header.dest_pan;

						reset();
						//we wait for an ack, so we set the bit
						waits_for_ack=true;
						initialized_ack_mechanism=true;
						mac_layer.acknolagement_timeout_timer.start(); //start ack timeout Timer 

						return success;

					}

				} acknolagement_handler;
					/*!
					  \brief automatically evaluates all relevant information from the mac message, to avoid errors, and then creates equivalent ACK message, which the sender is expecting
					  \param msg a valid MAC message (from IEEE type Data)
					  \return an errorcode, \see Acknolagement_Handler for possible errorcodes
					*/
					int send_ACK_for_MAC_Message(MAC_Message& msg){
						/* Acknolagement_Handler::ACK_ERROR_CODE
						MAC_Message ack_message = msg;

						ack_message.setPayloadNULL();
						ack_message.size=0;						
						*/				

						//TODO:FIXME:TEST for delayed ACK, if that gets to its destination, remove LATER!!!!
						//delay_ms(1);

						if(msg.header.controlfield.ackrequest==0) return Acknolagement_Handler::success;

						if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "sending ACK..." << ::logging::log::endl;

						//if the sender doesnt want an ACK, he won't get one
						//if(msg.header.controlfield.ackrequest==0) return Acknolagement_Handler::success;

						/*
	//MAC_Message(IEEE_Frametype msgtyp, DeviceAddress source_adress, DeviceAddress dest_adress, char* pointer_to_databuffer, uint8_t size_of_databuffer)
						//sende es wieder dorthin, wo es her kam
						MAC_Message ack_message(Acknowledgment,  //frametype is ACK
							MAC_Config::mac_adress_of_node, //source adress of ACK, take it from Config
							msg.header.source_adress, //dest adress, is source adress of data message
							(char*)"ACK",4);//(char*) 0,0);   //ACK doesn't contain data, all relevant information are stored in the header
						*/

						acknolagement_handler.acknolagement_buffer.header.sequencenumber=msg.header.sequencenumber; //the ACK is for this data message, so we need the same sequence numbers
						acknolagement_handler.acknolagement_buffer.header.dest_pan = msg.header.source_pan; //destination_panid_of_last_transmitted_message;
						acknolagement_handler.acknolagement_buffer.header.dest_adress = msg.header.source_adress;
						acknolagement_handler.acknolagement_buffer.header.source_adress = MAC_Config::mac_adress_of_node;
						acknolagement_handler.acknolagement_buffer.header.source_pan=MAC_Config::pan_id;
						acknolagement_handler.acknolagement_buffer.header.controlfield.frametype = Acknowledgment;
						acknolagement_handler.acknolagement_buffer.size=0; //we have no payload, so set size to zero
						acknolagement_handler.acknolagement_buffer.header.controlfield.ackrequest=0;

						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "=====> start ACK msg..." << ::logging::log::endl << ::logging::log::endl;
						if(MAC_LAYER_VERBOSE_OUTPUT) acknolagement_handler.acknolagement_buffer.print();
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "=====> end ACK msg..." << ::logging::log::endl << ::logging::log::endl;
						//set bit, that ack is to send, so the send function knows, that there is a valid Ack 
						//frame waiting for transmission in the acknolagement_buffer
						acknolagement_handler.ack_to_send=true;
						//copy build message into mac layer ack buffer
						//acknolagement_handler.acknolagement_buffer=ack_message;
						send_async_intern();

						return Acknolagement_Handler::success;

					}



			public:
				/*! \brief This delegate contains function, that is called if a valid MAC_Message was received. In the function you bind to this Delegate, you should use receive(MAC_Message& msg) to get the received message.*/
				Delegate<> onMessageReceiveDelegate;
				/*! \brief This delegate contains a functionpointer to a function, that is called if a send operation is completed. To get the errorcode  use get_result_of_last_send_operation() in the function to bind to this delegate*/
				Delegate<> onSend_Operation_Completed_Delegtate;

				/*!
				  \brief Constructor of the Mac Layer. Initializes the protocol and makes a compile time verification, whether the configuration is correct or not. 
				  If the configuration class (first template parameter) doesn't inherit from MAC_Configuration, then it causes an compile time error, notifying the user about the error. 
				*/
				MAC_CSMA_CA() : send_buffer(receive_buffer){   // : channel(11), mac_adress(0){


				//compile time verification, whether MAC_Configuration is the baseclass of the parameter MAC_Config, just to be shure we get a valid configuration
				 static const bool k=boost::is_base_of<MAC_Configuration,MAC_Config>::value;

				 ARMAROW_STATIC_ASSERT_ERROR(k,INVALID_MAC_CONFIGURATION__TYPE_DOESNT_INHERIT_FROM_CLASS__MAC_CONFIGURATION,(MAC_Config));

			
					this->channel=MAC_Config::channel; //11;
					this->mac_adress_of_node=MAC_Config::mac_adress_of_node;      //28;          //this parameter can be configured 

					init();	

		
				}


				//=============================================================================================================================
				//============== Start Interrupt Service Routines =============================================================================
				//=============================================================================================================================

					/*!
					  \brief Run to completion Task of the Mac protocol, handles the timout event if an acknolagement message wasn't received in time.
					  If a timeout event occures, then this task will be activated from the acknolagement timeout clock. Then it will determine, whether a retransmission is attempted or not. If the maximal number of retransmissions is already exceeded, then the onSend_Operation_Completed_Delegtate is called to notify the user about the failed transmission. Otherwise, we will just retry transmitting the message.   
					*/
					void callback_wait_for_ack_timeout(){

						
						if(acknolagement_handler.waits_for_ack){
					
								//start of ISR, secure with lock
								avr_halib::locking::GlobalIntLock lock;

								acknolagement_handler.backoff_timing.current_number_of_retransmissions++;
							
								acknolagement_handler.waits_for_ack=false;  //timeout event, delete bit, so the busy wait in the sender function will end and 
								acknolagement_handler.timeout_counter_in_ms=0;

								//this is the ACK for the last transmitted message
								acknolagement_handler.received_ack_for_last_transmitted_message=false;

								//mechanism has to be initialized again by calling "init_waiting_mechanism_for_ACK_for_MAC_Message"
								acknolagement_handler.initialized_ack_mechanism=false;
			
							
								acknolagement_handler.timeout_occured=true;
																				

								if(acknolagement_handler.backoff_timing.current_number_of_retransmissions<= acknolagement_handler.backoff_timing.maximal_number_of_retransmissions){ 

									//if(MAC_LAYER_VERBOSE_OUTPUT) 
										if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "retry transmitting... attempt number " 
										<< (int) acknolagement_handler.backoff_timing.current_number_of_retransmissions << ::logging::log::endl;


									//reset backoff exponent, so that the mximal waiting time is not growing from retransmission to retransmission
									acknolagement_handler.backoff_timing.current_backoff_exponend=MAC_Config::minimal_backoff_exponend;

									

									send_async_intern();
									return;
									//pointer_to_async_sending_ISR
									
									//result_of_last_send_operation_errorcode=TIMEOUT;

								}else{
									acknolagement_handler.result_of_last_send_operation_errorcode=Acknolagement_Handler::TIMEOUT;
									//number of retries exceeded boundaries
									has_message_to_send=false;
									if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "TIMEOUT..." << ::logging::log::endl;

									if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT)::logging::log::emit() << "number of retries has exeeded..." << ::logging::log::endl;

									 if(!onSend_Operation_Completed_Delegtate.isEmpty()) onSend_Operation_Completed_Delegtate();

								}								

							
							}



					}



				
 /*! callback_receive_message 
					
      \brief Run to completion Task of the Mac protocol, is executed by the Physical Layer, if a new message is received.
      The Task will first try to decode the Physical Layer Message. If the decoding fails the message is dropped. Otherwise the received Mac Message is passed to a Filter. 
	If the message is adressed at us, we keep it, otherwise we drop it. 
	If we received a Data Message, we send an acknolagement message if requested (send ack bit in the Mac Header controlfield) and call the onMessageReceiveDelegate to notify the user about the received message.

  */
				void callback_receive_message(){

				avr_halib::locking::GlobalIntLock lock;

				{ //critial section start

                                 //avr_halib::locking::GlobalIntLock lock;


				if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "entered receive message interupt" << ::logging::log::endl;

					

					Radiocontroller::receive(physical_layer_receive_buffer);

					has_message_ready_for_delivery=true;
				

					armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(physical_layer_receive_buffer);

					if(mac_msg == (armarow::MAC::MAC_Message*) 0 ) {
	
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;

						has_message_ready_for_delivery=false; //message is somehow invalid 
						return;

					}

					receive_buffer = *mac_msg;

					//===========================================================
					//================= MESSAGE FILTERING START =================


					if(MAC_Config::promiscuous_mode==0){

					if(receive_buffer.header.dest_adress==MAC_Config::mac_adress_of_node  						  
					   || receive_buffer.header.dest_adress==MAC_BROADCAST_ADRESS){
					//TODO: && mac_message.header.dest_pan==MAC_Config::pan_id
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "accepted message..." << ::logging::log::endl;

					}else{
						//message is not for us, so we drop it
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "dropped message...(" 
						<< (int) receive_buffer.header.controlfield.frametype << ","  //type
						<< (int) receive_buffer.header.sequencenumber << ","   //sequence number
						<< (int) receive_buffer.header.source_adress << ")" << ::logging::log::endl; //sender id

						has_message_ready_for_delivery=false; //we are not interested in messages, that are not for us

						//::logging::log::emit() << "dropped message..." << ::logging::log::endl;
						//receive_buffer.print();
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
						return;

					}

					//at this point, we can be sure, that the received message is for us

					if(ENABLE_FILTERING_OF_DUPLICATES){

					//did we already receive the message (only data) -> we determine this by looking at the sequence number
					//we don'T want to filte out double Acks
					if(receive_buffer.header.controlfield.frametype==Data 
						&& message_filter.sequence_number_of_last_received_message == receive_buffer.header.sequencenumber
						&& message_filter.source_id_of_last_received_message == receive_buffer.header.source_adress
						&& message_filter.source_panid_of_last_received_message == receive_buffer.header.source_pan
					){

						::logging::log::emit() << "filtered out duplicate message...(" 
						<< (int) message_filter.sequence_number_of_last_received_message << ","
						<< (int) message_filter.source_id_of_last_received_message << ","
						<< (int) message_filter.source_panid_of_last_received_message << ")"
						<< ::logging::log::endl;

		
						has_message_ready_for_delivery=false;
						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
						return;

					}

					//init message filter with the header data of the current message				
					message_filter.sequence_number_of_last_received_message=receive_buffer.header.sequencenumber;
					message_filter.source_id_of_last_received_message = receive_buffer.header.source_adress;
					message_filter.source_panid_of_last_received_message = receive_buffer.header.source_pan;


					} //end enable feature duplicate filtering

					} //end if promiscuous mode

					//================= MESSAGE FILTERING END ==================
					//===========================================================



					if(receive_buffer.header.controlfield.frametype!=Data) {

						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "===> got meta msg..." << ::logging::log::endl << ::logging::log::endl;

						if(MAC_LAYER_VERBOSE_OUTPUT) receive_buffer.print(); //just for debug purposes

						if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "===> end meta msg..." << ::logging::log::endl << ::logging::log::endl;

						has_message_ready_for_delivery=false;  //the application is only interested in application data, special packages have to be filtered out
					


						//TODO: set bit that sended message was acknolaged
						if(receive_buffer.header.controlfield.frametype==Acknowledgment){

							//set bit to 1
							acknolagement_handler.handle_received_ACK(receive_buffer,has_message_to_send,onSend_Operation_Completed_Delegtate,*this); //this bit has to be set to false if neccessary, so we have to provide it via reference


						}


						return;
					}


					//if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "=> start data message:" << ::logging::log::endl;
					if(MAC_LAYER_VERBOSE_OUTPUT) receive_buffer.print();
					//if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "=> end data message:" << ::logging::log::endl;

					//at this point, we know that we have received a data frame, so we have to send an ack
					//TODO: send ACK
					//acknolagement_handler.

					send_ACK_for_MAC_Message(receive_buffer);


					//evaluation.received_bytes_in_last_second+=receive_buffer.size;

					//intern evaluation feature for measurement of bandwith
					this->add_number_of_received_bytes(receive_buffer.size);


					//mac_msg->print();

					has_message_ready_for_delivery=true;

					if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "leaving receive message interrupt, calling delegate" << ::logging::log::endl;

					if(!onMessageReceiveDelegate.isEmpty()) onMessageReceiveDelegate();

					} //critial section end


					//receive_buffer.print(); 
					//if we reach this instruction, everything went well and we can call a user defined interrupt service routine
					
				if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
					

				}

				/*!
				  \brief Run to completion Task of the Mac protocol, tries to send the message that is stored in the send_buffer buffer. Note, that tasks are not intended to be called directly by the user.
				  Some general behaviour of this function:
				  Acknolagement messages have priority over data messages. 
				  The backoff timing behaviour is different for Ack and Data messages. 
				  Data messages have the IEEE backoff timing implementation. 
				  Ack messages have a constant backoff time, because they are time critical. 
				  If the medium is busy, then a oneshot timer is started, that calls this Task again after the specified timing behaviour.
				*/
				void send_async_intern(){

				    uint8_t ccaValue=0;
				    armarow::PHY::State status;
				    MAC_Message message_to_send=send_buffer;

				    { //critial section start

					//it can be called per interrupt, so we secure it
					avr_halib::locking::GlobalIntLock lock;

					if(acknolagement_handler.ack_to_send==true){
						message_to_send=acknolagement_handler.acknolagement_buffer;
					}
					if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "enter send ISR" << ::logging::log::endl;
					one_shot_timer.stop();
					if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "do CCA" << ::logging::log::endl;
					status=Radiocontroller::doCCA(ccaValue);
					
					//if(MAC_LAYER_VERBOSE_OUTPUT) 
					::logging::log::emit() << "finished CCA" << ::logging::log::endl;

					if(status==armarow::PHY::success && ccaValue)
					{
					

					//if(MAC_LAYER_VERBOSE_OUTPUT) 
					//::logging::log::emit() << "transmit message..." << ::logging::log::endl;
					if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "sending..." << ::logging::log::endl;	
					if(MAC_LAYER_VERBOSE_OUTPUT) message_to_send.print();
	
					//we want to send (tranceiver on)
					Radiocontroller::setStateTRX(armarow::PHY::tx_on);

					//send
					Radiocontroller::send(*message_to_send.getPhysical_Layer_Message());

					//after sending we need to change in the Transive mode again, so that we get received messages per interrupt
					Radiocontroller::setStateTRX(armarow::PHY::rx_on);

									

					//we have transmitted our ack, we can now return to normal operation, where we send the data messages
					if(acknolagement_handler.ack_to_send==true){
						acknolagement_handler.ack_to_send=false;
						//send ISR have to call itself again, to deliver the data message in the buffer, if any
						if(has_message_to_send) one_shot_timer.start(1);
						return;
					  }
					if(message_to_send.header.controlfield.ackrequest==1){
					//init variables of acknolagement_handler, so that it waits for the ACK message for the transmitted message (ONLY that one)
					acknolagement_handler.init_waiting_mechanism_for_ACK_for_MAC_Message(message_to_send,*this);
					}else{

						acknolagement_handler.result_of_last_send_operation_errorcode = Acknolagement_Handler::success;

						has_message_to_send=false;
						//consider calling this delegate outside of the critical section
						 if(!onSend_Operation_Completed_Delegtate.isEmpty()) onSend_Operation_Completed_Delegtate();

					}

					}else{
						//for one shot timer test
						one_shot_timer.stop();

						if(MAC_LAYER_VERBOSE_OUTPUT)  ::logging::log::emit()
           				 					<< PROGMEMSTRING("Medium busy...")
            									<< ::logging::log::endl;


						//we couldn't transmit our ack, so we have to compute a random waiting time and try again later
						//since we don't want to influence the backoff timing of data messages, we use our own method 
						if(acknolagement_handler.ack_to_send==true){

							int randomnumber = rand();
												//the 20 is the maximal waiting time for an ACK
							uint32_t waitingtime = (((uint32_t)randomnumber * 20) / (0x8000)); //0x8000 = RAND_MAX+1 -> Optimization, so that we can do a shift instead of a division

							//one shot timer neu stellen, dieser ruft diese Funktion nach einer zufälligen Zeit
							//erneut auf, solange bis Nachricht erfolgreich versendet wurde
							one_shot_timer.start((uint16_t)waitingtime);

						return;

						}

						if(acknolagement_handler.backoff_timing.number_of_backoffs_has_exeeded()){

							has_message_to_send=false;
							acknolagement_handler.result_of_last_send_operation_errorcode=Acknolagement_Handler::MEDIUM_BUSY;

							if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "number of backoffs has exeeded..." << ::logging::log::endl;		
							acknolagement_handler.backoff_timing.reset();
							if(!onSend_Operation_Completed_Delegtate.isEmpty()) onSend_Operation_Completed_Delegtate();
						}else{
							//start one shot timer
							one_shot_timer.start(acknolagement_handler.backoff_timing.get_random_backoff_time_in_ms());
						}
					}
					if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "leave send ISR" << ::logging::log::endl;
				    } //critial section end
				} //end send_async_intern

				//=============================================================================================================================
				//============== END Interrupt Service Routines ===============================================================================
				//=============================================================================================================================

				/*!
				  \brief resets the acknolagement timer, so that it starts counting from zero again, if you call the start function
				  It is aconvinience function that can be replaced if a timerframework is available.
				*/
				void reset_acknolagement_timer(){

					acknolagement_timeout_timer.stop();
					acknolagement_timeout_timer.setCounter(0); //reset clock

				}
				/*!
				  \brief Initialises all relevant data needed for correct operation of the protocol.
				  It binds special functions of the Mac Layer to delegates of the Physical Layer and Clocks to implement the event triggered system.
				*/
				int init(){

					//mac specific callback for received messages 
					this->onReceive.template bind<MAC_CSMA_CA, &MAC_CSMA_CA::callback_receive_message>(this);
					//function for one shot timer
					this->one_shot_timer.onTimerDelegate.template bind<MAC_CSMA_CA, &MAC_CSMA_CA::send_async_intern>(this);
					//Basic initialization
					MAC_Base<Radiocontroller,Mac_Evaluation_activation_state>::init();
					//typeof *this = MAC_CSMA_CA
					acknolagement_timeout_timer.registerCallback<typeof *this, &MAC_CSMA_CA::callback_wait_for_ack_timeout>(*this);
					reset_acknolagement_timer();//it doesn't count anymore and if you start it again, it begins from the beginning
					//set the configured channel, use Physical Layer method
					this->Radiocontroller::setAttribute(armarow::PHY::phyCurrentChannel,&this->channel); 
					has_message_ready_for_delivery=false;
					has_message_to_send=false;
					int tmp = 2; //carrier sense only
					this->Radiocontroller::setAttribute(armarow::PHY::phyCCAMode,&tmp); //carrier sense only  // with energy above threshold
					return 0;
				}
				/*!
				  \brief This method resets the Physical Layer and reinitializes the Mac Layer. It is intended to use in fatal error situations, where normal operation of the protocol is no longer possible.
				*/
				int reset(){
					//here we need to call the radio controller directly, because a MAC_Base::init() wouldn't consider the extensions from this class
					Radiocontroller::reset();
					init();
					return 0;
				}
				/*!
				  \brief This Method is intended to be called in a user defined function, that is bound to the onSend_Operation_Completed_Delegtate, to get the errorcode for the last send operation.
				  \return errorcode describing the result of the last send operation. \see Acknolagement_Handler for possible errorcodes.
				*/
				int get_result_of_last_send_operation(){
					return acknolagement_handler.result_of_last_send_operation_errorcode;
				}
				/*!
				  \brief Writes the current value from the specified attribute into the second parameter value.
				  \param attributes indicates the attribute that should be read
				  \param value contains the value of the specified attribute after execution of this function
				*/
				void get_MAC_Attribut(typename MAC_Base<Radiocontroller,Mac_Evaluation_activation_state>::mac_attributes attributes,  AttributType* value){
					
				}
				/*!
				  \brief Writes the content of the second parameter in the variable of the specified attribute.
				  \param attributes indicates the attribute that should be written
				  \param value contains the value that should be assigned to the specified attribute
				*/
				void set_MAC_Attribut(typename MAC_Base<Radiocontroller,Mac_Evaluation_activation_state>::mac_attributes attributes,  AttributType value){

				}
				/*!
				  \brief This funtion is intended to be called, if the user wants to transmit data.
				  It will send the message asynchron to the main application. It first initializes the neccessary data and then tries to send the message. Sequence number, mac source_adress and mac source_pan are set according to the MAC_Configuration, passed as template argument to the Main class. If it cannot transmit the message in the first attempt, it will try again later asynchron to the main program. (Task will be executed again after some time due to a one shot timer.)
				  \param mac_message a reference to the message that has to be transmitted
				*/		
				int send_async(MAC_Message& mac_message){

				   avr_halib::locking::GlobalIntLock lock;
				   if(!has_message_to_send){

					has_message_to_send=true;
					//init message header
					mac_message.header.sequencenumber=this->get_global_sequence_number();
					//optional: source adress pan und message type dürfen vom nutzen nicht geändert werden -> private machen und MAC Layer als friend deklarieren
					mac_message.header.source_adress=MAC_Config::mac_adress_of_node;
					mac_message.header.source_pan=MAC_Config::pan_id;
					acknolagement_handler.backoff_timing.reset(); //backoff timing includes retransmission related variables. Since we start a new transmission, they have to be set to default values
					//copy message into send message buffer
					send_buffer=mac_message;
					//sends the message that we copied in the send_buffer
					send_async_intern();
					return 0;
				   }else{
					return -1; //the Last Message we wanted to transmit wasn't send yet
				   }

				}
				/*!
				  \brief Just an alternativ name for send_async, since we don't need a synchron send method.
				*/
				int send(MAC_Message mac_message){

					//call asynchron send routine
					return this->send_async(mac_message);


				}

				/*!

				  \brief receive blocks until a message is received (or it returns an already received, but not delivered message (delivered with respect to the application))
				  Use this function only, if you want to explicitly wait for a received message. (You can do more meaningful things than busy wait you know, so you should consider binding a message handler on the onMessageReceive Delegate of the Mac Layer. That way, you receive asynchron and that is usually what you want. In this case receive returns immediatly, since the callback is only called if an undelivered message is ready and receive returns the last undelivered message, so it will not block.)
				*/
				int receive(MAC_Message& mac_message){

					mac_message.setPayloadNULL();

					Radiocontroller::setStateTRX(armarow::PHY::rx_on);
					//Radiocontroller::receive_blocking(message);


					//TODO: replace busy wait with something like sleep that wakes up if an interupt occures to avoid energy waste

					while(has_message_ready_for_delivery==false){

					}
					//if we get here, an interrupt occoured in the meantime, and we can deliver a message
					//the first thing we do is setting the value false again and then returning the message
					has_message_ready_for_delivery=false;
					mac_message = receive_buffer;
					return mac_message.size;
				}
			}; //end class CSMA/CA
		} //end namespace mac
	} //end namespace armarow
#endif

