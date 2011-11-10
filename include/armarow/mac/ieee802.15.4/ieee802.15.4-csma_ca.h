#pragma once

#ifndef MAC_LAYER_VERBOSE_OUTPUT
	/*! \def MAC_LAYER_VERBOSE_OUTPUT contains a bool value indicating whether verbose output should be enabled or disabled Since that is a Mac Layer debugging functionality, it is not represented to the user explicitly. (You won't have any benefit enabling this, since the logging uses the UART and thats is very slow.)*/
	#define MAC_LAYER_VERBOSE_OUTPUT false
#endif
#ifndef MAC_VERBOSE_ACK_OUTPUT 
	/*! \def MAC_VERBOSE_ACK_OUTPUT contains a bool value indicating whether verbose output concerning the acknolagement mechanism should be enabled or disabled Since that is a Mac Layer debugging functionality, it is not represented to the user explicitly. (You won't have any benefit enabling this, since the logging uses the UART and thats is very slow.)*/
	#define MAC_VERBOSE_ACK_OUTPUT false
#endif
#ifndef ENABLE_FILTERING_OF_DUPLICATES
	/*! \def ENABLE_FILTERING_OF_DUPLICATES This enables an experimental feature for duplicate filtering. If you really need it, enable it. Otherwise just keep it false.*/
	#define ENABLE_FILTERING_OF_DUPLICATES false
#endif

#include "mac.h"
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

UseInterrupt(SIG_OUTPUT_COMPARE3A);

namespace armarow {
namespace mac {
    typedef void* AttributType;
    typedef uint16_t DeviceAddress; 
	typedef MAC_Message mob_t;

    /*! \brief ERROR Message neccessary for STATIC ASSERT ERROR MESSAGE to report invalid Configuration at compile time*/
    class INVALID_MAC_CONFIGURATION__TYPE_DOESNT_INHERIT_FROM_CLASS__MAC_CONFIGURATION;

		
    /*! \brief MAC_Configuration: the abstraction of a MAC Configuration, all user defined configurations have to inherit from this class This class contains all parameters of the Mac Layer that are configurable. This class doesn't need memory at run time, since it exits only at compile time */
    struct MAC_Configuration {
        enum {
            channel                              = 11,
            mac_adress_of_node                   = 28,
            pan_id                               = 0,
            ack_request                          = 1,
            minimal_backoff_exponend             = 2,
            maximal_backoff_exponend             = 7,
            maximal_number_of_retransmissions    = 3,
            promiscuous_mode                     = 0,
            acknolagement_timeout_duration_in_ms = 100
        };
    };

    enum MAC_Special_Adresses{ MAC_BROADCAST_ADRESS = 255 };

    /*! \brief MAC_CSMA_CA: the implementation of the Mac protocol CSMA/CA
     *  This class inherits from MAC_Base, because all Mac protocols have to have the same interface. (so it inherits the Physical Layer as well)
     *  The first template parameter is a Configuration class, that inherits from the MAC_Configuration class. It contains all MAC_Layer relevant configuration parameters.
     *  The desired Physical Layer is the second template parameter, and MAC_Base will inherit from it. 
     *  The third template parameter indicates the usage of the evaluation features of the Mac protocol. In the MAC namespace there are to constants, Enables and Disable. If you don't want to use the evaluation features (e.g. measuring bandwith), then you should turn this feature of with the Disable parameter. Otherwise you should pass the Enable parameter.*/
    template<class MAC_Config,class Radiocontroller,MAC_EVALUATION_ACTIVATION_STATE Mac_Evaluation_activation_state>
    struct MAC_CSMA_CA : public MAC_Base<Radiocontroller,Mac_Evaluation_activation_state> {
            /*! \brief For ease of use and debugging purposes, we make a Typedef for the Mac protocol, so that it can be accessed as MAC_LAYER.*/
            typedef MAC_CSMA_CA<MAC_Config,Radiocontroller,Mac_Evaluation_activation_state> MAC_LAYER;
        protected:
            MAC_Clock acknolagement_timeout_timer;
            ExactEggTimer<Timer3> one_shot_timer;				
            MAC_Message receive_buffer;
            typename Radiocontroller::mob_t physical_layer_receive_buffer;
            /*! we don't want to deliver the same message twice, so we need a flag for that and we declare it as volatile, so that the compiler doesn't do anything fishy to it (optimization)*/
            volatile bool has_message_ready_for_delivery;
            /*! bit we need for timer interrupt routine, to decide if there is a message to send (asynchron message delivery)*/
            volatile bool has_message_to_send;
            /*! the send async method will copy the user data in this internal buffer*/
            MAC_Message send_buffer;

            /*! \brief The abstraction of all needed variables to implement a simple Message Filter. We will only remember the last received message to save memory.*/
            struct Message_Filter {
                uint8_t sequence_number_of_last_received_message;
                uint8_t source_id_of_last_received_message;
                uint8_t source_panid_of_last_received_message;

                Message_Filter() {
                    sequence_number_of_last_received_message = 0;
                    source_id_of_last_received_message       = 255;
                    source_panid_of_last_received_message    = 255; 
                }
            } message_filter;

            /*! If a maximal waiting time is exceeded, then waits for ack will be set to false and an error is reported The variable received_ack_for_last_transmitted_message is set only if an ack was received, if the timeout occur beforhand, it will still be false and indicates an error -> retransmission, or if number of retransmission exceeds a limit, than report an error. \brief The abstraction of all variables and program logic to implement an acknolagement mechanism in the Mac protocol.*/
            struct Acknolagement_Handler {
                enum ACK_ERROR_CODE { success, TIMEOUT, MEDIUM_BUSY };
                volatile struct {
                    bool waits_for_ack                             : 1;
                    bool received_ack_for_last_transmitted_message : 1;
                    bool initialized_ack_mechanism                 : 1;
                    bool timeout_occured                           : 1;
                    bool ack_to_send                               : 1;
                };
				/*! \brief Implements the functionality of the IEEE backoff Timing computation. Neccessary variables and program logic are encapsulated here. The Idea of this class is the structuring of feature related variables and constants in a sub class for better readability and maintainability. (Reducing code scattering.) */
				struct Backoff_Timing {
                    uint8_t current_backoff_exponend;
                    uint8_t maximal_number_of_retransmissions;
                    uint8_t current_number_of_retransmissions;

					Backoff_Timing(){
						//init variables
						this->reset();
						maximal_number_of_retransmissions=MAC_Config::maximal_number_of_retransmissions; //3;
					}
					/*!
					  \brief This method returns true, if we had more backoffs than specified in the configuration and false otherwise
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
            /*! \brief automatically evaluates all relevant information from the mac message, to avoid errors, and then creates equivalent ACK message, which the sender is expecting
             *
             *  \param msg a valid MAC message (from IEEE type Data)
             *  \return an errorcode, \see Acknolagement_Handler for possible errorcodes
			 */
            int send_ACK_for_MAC_Message(MAC_Message& msg) {
                if ( msg.header.controlfield.ackrequest == 0 )
                    return Acknolagement_Handler::success;
                if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT )
                    ::logging::log::emit() << "sending ACK..." << ::logging::log::endl;

                acknolagement_handler.acknolagement_buffer.header.sequencenumber = msg.header.sequencenumber;
                acknolagement_handler.acknolagement_buffer.header.dest_pan       = msg.header.source_pan;
                acknolagement_handler.acknolagement_buffer.header.dest_adress    = msg.header.source_adress;
                acknolagement_handler.acknolagement_buffer.header.source_adress  = MAC_Config::mac_adress_of_node;
                acknolagement_handler.acknolagement_buffer.header.source_pan     = MAC_Config::pan_id;
                acknolagement_handler.acknolagement_buffer.header.controlfield.frametype = Acknowledgment;
                acknolagement_handler.acknolagement_buffer.size = 0;
                acknolagement_handler.acknolagement_buffer.header.controlfield.ackrequest = 0;

                if ( MAC_LAYER_VERBOSE_OUTPUT )
                    ::logging::log::emit() << "=====> start ACK msg..." << ::logging::log::endl << ::logging::log::endl;
                if ( MAC_LAYER_VERBOSE_OUTPUT )
                    acknolagement_handler.acknolagement_buffer.print();
                if ( MAC_LAYER_VERBOSE_OUTPUT )
                    ::logging::log::emit() << "=====> end ACK msg..." << ::logging::log::endl << ::logging::log::endl;
                acknolagement_handler.ack_to_send = true;
                send_async_intern();

                return Acknolagement_Handler::success;
            }
        public:
            /*! \brief Constructor of the Mac Layer. Initializes the protocol and makes a compile time verification, whether the configuration is correct or not. If the configuration class (first template parameter) doesn't inherit from MAC_Configuration, then it causes an compile time error, notifying the user about the error.*/
            MAC_CSMA_CA() : send_buffer(receive_buffer) {
                static const bool k = boost::is_base_of<MAC_Configuration,MAC_Config>::value;
                ARMAROW_STATIC_ASSERT_ERROR(k,INVALID_MAC_CONFIGURATION__TYPE_DOESNT_INHERIT_FROM_CLASS__MAC_CONFIGURATION,(MAC_Config));
                this->mac_adress_of_node=MAC_Config::mac_adress_of_node;
                init();	
            }

            //=============================================================================================================================
            //============== Start Interrupt Service Routines =============================================================================
            //=============================================================================================================================

            /*! \brief Run to completion Task of the Mac protocol, handles the timout event if an acknolagement message wasn't received in time. If a timeout event occures, then this task will be activated from the acknolagement timeout clock. Then it will determine, whether a retransmission is attempted or not. If the maximal number of retransmissions is already exceeded, then the onSend_Operation_Completed_Delegtate is called to notify the user about the failed transmission. Otherwise, we will just retry transmitting the message.*/
            void callback_wait_for_ack_timeout() {
                if ( acknolagement_handler.waits_for_ack ) {
                    avr_halib::locking::GlobalIntLock lock;

                    acknolagement_handler.backoff_timing.current_number_of_retransmissions++;
                    acknolagement_handler.waits_for_ack = false;
                    acknolagement_handler.timeout_counter_in_ms = 0;
                    acknolagement_handler.received_ack_for_last_transmitted_message = false;
                    acknolagement_handler.initialized_ack_mechanism = false;
                    acknolagement_handler.timeout_occured = true;
                    if ( acknolagement_handler.backoff_timing.current_number_of_retransmissions <= acknolagement_handler.backoff_timing.maximal_number_of_retransmissions ) { 
                        if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT )
                            ::logging::log::emit() << "retry transmitting... attempt number " << (int) acknolagement_handler.backoff_timing.current_number_of_retransmissions << ::logging::log::endl;
                            acknolagement_handler.backoff_timing.current_backoff_exponend = MAC_Config::minimal_backoff_exponend;
                            send_async_intern();
                            return;
                    } else {
                        acknolagement_handler.result_of_last_send_operation_errorcode = Acknolagement_Handler::TIMEOUT;
                        has_message_to_send = false;
                        if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT )
                            ::logging::log::emit() << "TIMEOUT..." << ::logging::log::endl;
                        if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT )
                            ::logging::log::emit() << "number of retries has exeeded..." << ::logging::log::endl;
                        if ( !this->onSend_Operation_Completed_Delegtate.isEmpty() )
                            this->onSend_Operation_Completed_Delegtate();
                    }	
                }
            }

            /*! callback_receive_message \brief Run to completion Task of the Mac protocol, is executed by the Physical Layer, if a new message is received. The Task will first try to decode the Physical Layer Message. If the decoding fails the message is dropped. Otherwise the received Mac Message is passed to a Filter. If the message is adressed at us, we keep it, otherwise we drop it. If we received a Data Message, we send an acknolagement message if requested (send ack bit in the Mac Header controlfield) and call the onMessageReceiveDelegate to notify the user about the received message.*/
            void callback_receive_message() {
				avr_halib::locking::GlobalIntLock lock;
				{
                    if ( MAC_LAYER_VERBOSE_OUTPUT )
                        ::logging::log::emit() << "entered receive message interupt" << ::logging::log::endl;
                    Radiocontroller::receive(physical_layer_receive_buffer);
					has_message_ready_for_delivery = true;
					armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(physical_layer_receive_buffer);
					if ( mac_msg == (armarow::MAC::MAC_Message*)0 ) {
						if ( MAC_LAYER_VERBOSE_OUTPUT )
                            ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
						has_message_ready_for_delivery = false;
						return;
					}
					receive_buffer = *mac_msg;
					//================= MESSAGE FILTERING START =================
					if ( MAC_Config::promiscuous_mode == 0 ) {
                        if ( receive_buffer.header.dest_adress == MAC_Config::mac_adress_of_node || receive_buffer.header.dest_adress == MAC_BROADCAST_ADRESS ) {
                            if ( MAC_LAYER_VERBOSE_OUTPUT )
                                ::logging::log::emit() << "accepted message..." << ::logging::log::endl;
                        } else {
                            if ( MAC_LAYER_VERBOSE_OUTPUT)
                                ::logging::log::emit() << "dropped message...(" << (int) receive_buffer.header.controlfield.frametype << "," << (int) receive_buffer.header.sequencenumber << "," << (int) receive_buffer.header.source_adress << ")" << ::logging::log::endl;
                                has_message_ready_for_delivery = false;
						    if ( MAC_LAYER_VERBOSE_OUTPUT )
                                ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
                            return;
                        }
                        if ( ENABLE_FILTERING_OF_DUPLICATES ) {
                            if ( receive_buffer.header.controlfield.frametype == Data && 
                                 message_filter.sequence_number_of_last_received_message == receive_buffer.header.sequencenumber &&
                                 message_filter.source_id_of_last_received_message == receive_buffer.header.source_adress &&
                                 message_filter.source_panid_of_last_received_message == receive_buffer.header.source_pan ) {
                                ::logging::log::emit() << "filtered out duplicate message...(" << (int) message_filter.sequence_number_of_last_received_message << "," << (int) message_filter.source_id_of_last_received_message << "," << (int) message_filter.source_panid_of_last_received_message << ")" << ::logging::log::endl;
                                has_message_ready_for_delivery=false;
                                if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
                                return;
                            }
                            message_filter.sequence_number_of_last_received_message = receive_buffer.header.sequencenumber;
                            message_filter.source_id_of_last_received_message       = receive_buffer.header.source_adress;
                            message_filter.source_panid_of_last_received_message    = receive_buffer.header.source_pan;
                        }
					}
					//================= MESSAGE FILTERING END ==================
					if(receive_buffer.header.controlfield.frametype!=Data) {
						if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "===> got meta msg..." << ::logging::log::endl << ::logging::log::endl;
						if ( MAC_LAYER_VERBOSE_OUTPUT ) receive_buffer.print();
						if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "===> end meta msg..." << ::logging::log::endl << ::logging::log::endl;
						has_message_ready_for_delivery = false;
						if ( receive_buffer.header.controlfield.frametype == Acknowledgment ) {
							acknolagement_handler.handle_received_ACK(receive_buffer,has_message_to_send, this->onSend_Operation_Completed_Delegtate, *this);
						}
						return;
					}
					if ( MAC_LAYER_VERBOSE_OUTPUT ) receive_buffer.print();
					send_ACK_for_MAC_Message(receive_buffer);
					this->add_number_of_received_bytes(receive_buffer.size);
					has_message_ready_for_delivery = true;
					if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "leaving receive message interrupt, calling delegate" << ::logging::log::endl;
					if ( !this->onMessageReceiveDelegate.isEmpty() ) this->onMessageReceiveDelegate();
                }
				if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "leave receive ISR" << ::logging::log::endl;
            }
			
            /*! \brief Run to completion Task of the Mac protocol, tries to send the message that is stored in the send_buffer buffer. Note, that tasks are not intended to be called directly by the user. Some general behaviour of this function: Acknolagement messages have priority over data messages. The backoff timing behaviour is different for Ack and Data messages. Data messages have the IEEE backoff timing implementation. Ack messages have a constant backoff time, because they are time critical. If the medium is busy, then a oneshot timer is started, that calls this Task again after the specified timing behaviour. */
            void send_async_intern() {
                uint8_t ccaValue = 0;
                armarow::PHY::State status;
                MAC_Message message_to_send = send_buffer;
                {
					avr_halib::locking::GlobalIntLock lock;

					if ( acknolagement_handler.ack_to_send == true ) {
                        message_to_send=acknolagement_handler.acknolagement_buffer;
					}
					if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "enter send ISR" << ::logging::log::endl;
					one_shot_timer.stop();
					if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "do CCA" << ::logging::log::endl;
					status = Radiocontroller::doCCA(ccaValue);
					::logging::log::emit() << "finished CCA" << ::logging::log::endl;
					if ( status == armarow::PHY::success && ccaValue ) {
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "sending..." << ::logging::log::endl;	
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) message_to_send.print();
                        Radiocontroller::setStateTRX(armarow::PHY::tx_on);
                        Radiocontroller::send(*message_to_send.getPhysical_Layer_Message());
                        Radiocontroller::setStateTRX(armarow::PHY::rx_on);
                        
                        if ( acknolagement_handler.ack_to_send == true ) {
                            acknolagement_handler.ack_to_send = false;
                            if ( has_message_to_send ) one_shot_timer.start(1);
                            return;
                          }
                        if ( message_to_send.header.controlfield.ackrequest == 1 ) {
                            acknolagement_handler.init_waiting_mechanism_for_ACK_for_MAC_Message(message_to_send,*this);
                        } else {
                            acknolagement_handler.result_of_last_send_operation_errorcode = Acknolagement_Handler::success;
                            has_message_to_send = false;
                            if ( !this->onSend_Operation_Completed_Delegtate.isEmpty() ) this->onSend_Operation_Completed_Delegtate();
                        }
					} else {
						one_shot_timer.stop();
						if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << PROGMEMSTRING("Medium busy...") << ::logging::log::endl;
						if ( acknolagement_handler.ack_to_send == true ) {
							int randomnumber = rand();
							uint32_t waitingtime = (((uint32_t)randomnumber * 20) / (0x8000));
							one_shot_timer.start((uint16_t)waitingtime);
                            return;
						}
						if ( acknolagement_handler.backoff_timing.number_of_backoffs_has_exeeded() ) {
							has_message_to_send = false;
							acknolagement_handler.result_of_last_send_operation_errorcode = Acknolagement_Handler::MEDIUM_BUSY;
							if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "number of backoffs has exeeded..." << ::logging::log::endl;		
							acknolagement_handler.backoff_timing.reset();
							if ( !this->onSend_Operation_Completed_Delegtate.isEmpty() ) this->onSend_Operation_Completed_Delegtate();
						} else {
							one_shot_timer.start(acknolagement_handler.backoff_timing.get_random_backoff_time_in_ms());
						}
					}
					if ( MAC_LAYER_VERBOSE_OUTPUT ) ::logging::log::emit() << "leave send ISR" << ::logging::log::endl;
                }
            }
            //============== END Interrupt Service Routines ===============================================================================
            /*! \brief resets the acknolagement timer, so that it starts counting from zero again, if you call the start function It is aconvinience function that can be replaced if a timerframework is available.*/
            void reset_acknolagement_timer() {
                acknolagement_timeout_timer.stop();
                acknolagement_timeout_timer.setCounter(0);
            }
            /*! \brief Initialises all relevant data needed for correct operation of the protocol. It binds special functions of the Mac Layer to delegates of the Physical Layer and Clocks to implement the event triggered system.*/
            int init() {
                this->onReceive.template bind<MAC_CSMA_CA, &MAC_CSMA_CA::callback_receive_message>(this);
                this->one_shot_timer.onTimerDelegate.template bind<MAC_CSMA_CA, &MAC_CSMA_CA::send_async_intern>(this);
                MAC_Base<Radiocontroller,Mac_Evaluation_activation_state>::init();
                acknolagement_timeout_timer.registerCallback<typeof *this, &MAC_CSMA_CA::callback_wait_for_ack_timeout>(*this);
                reset_acknolagement_timer();
                uint8_t channel = MAC_Config::channel;
                this->Radiocontroller::setAttribute(armarow::PHY::phyCurrentChannel,&channel); 
                has_message_ready_for_delivery=false;
                has_message_to_send=false;
                int tmp = 2;
                this->Radiocontroller::setAttribute(armarow::PHY::phyCCAMode,&tmp);
                return 0;
            }
            /*! \brief This method resets the Physical Layer and reinitializes the Mac Layer. It is intended to use in fatal error situations, where normal operation of the protocol is no longer possible.*/
            int reset() {
                Radiocontroller::reset();
                init();
                return 0; //FIXME
            }
            /*! \brief This Method is intended to be called in a user defined function, that is bound to the onSend_Operation_Completed_Delegtate, to get the errorcode for the last send operation.
             *  \return errorcode describing the result of the last send operation. \see Acknolagement_Handler for possible errorcodes.
             */
            int get_result_of_last_send_operation() {
                return acknolagement_handler.result_of_last_send_operation_errorcode;
            }
            /*! \brief This funtion is intended to be called, if the user wants to transmit data. It will send the message asynchron to the main application. It first initializes the neccessary data and then tries to send the message. Sequence number, mac source_adress and mac source_pan are set according to the MAC_Configuration, passed as template argument to the Main class. If it cannot transmit the message in the first attempt, it will try again later asynchron to the main program. (Task will be executed again after some time due to a one shot timer.)
             *  \param mac_message a reference to the message that has to be transmitted
             */		
            int send(MAC_Message& mac_message) {
                avr_halib::locking::GlobalIntLock lock;
                if ( !has_message_to_send ) {
                    has_message_to_send = true;
					mac_message.header.sequencenumber = this->get_global_sequence_number();
					mac_message.header.source_adress  = MAC_Config::mac_adress_of_node;
					mac_message.header.source_pan     = MAC_Config::pan_id;
					acknolagement_handler.backoff_timing.reset();
					send_buffer = mac_message;
					send_async_intern();
					return 0;
                } else {
					return -1;
                }
            }

            /*! \brief receive blocks until a message is received (or it returns an already received, but not delivered message (delivered with respect to the application)) Use this function only, if you want to explicitly wait for a received message. (You can do more meaningful things than busy wait you know, so you should consider binding a message handler on the onMessageReceive Delegate of the Mac Layer. That way, you receive asynchron and that is usually what you want. In this case receive returns immediatly, since the callback is only called if an undelivered message is ready and receive returns the last undelivered message, so it will not block.)
             */
            int receive(MAC_Message& mac_message) {
                mac_message.setPayloadNULL();
                Radiocontroller::setStateTRX(armarow::PHY::rx_on);
                while ( has_message_ready_for_delivery == false );
                has_message_ready_for_delivery = false;
                mac_message = receive_buffer;
                return mac_message.size;
            }
    };
}
}
