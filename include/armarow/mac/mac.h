#pragma once

#include <avr/io.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/avr/regmaps.h>
#include <avr-halib/avr/timer.h>
#include <avr-halib/ext/button.h>
#include <avr-halib/ext/led.h>
#include <avr-halib/portmaps/icstick_portmap.h>
#include <avr-halib/regmaps/local.h>
#include <avr-halib/share/delay.h>
#include <avr-halib/share/delegate.h>
#include <avr-halib/share/interruptLock.h>
#include <stdlib.h>

#include "../common.h" //FIXME
#include "platform-cfg.h" //FIXME 
#include "armarow/armarow.h"
#include "armarow/debug.h"
#include "armarow/phy/phy.h"
#include "mac_evaluation.h"
#include "mac_message.h"

UseInterrupt(SIG_OUTPUT_COMPARE1A);

void* operator new (size_t , void* buffer) { return buffer; } //FIXME

using avr_halib::drivers::Clock;
using namespace avr_halib::regmaps;

namespace armarow {
namespace mac {
	typedef MAC_Message mob_t;
    typedef Clock<ClockConfig> MAC_Clock;
    typedef void* AttributType;
    typedef uint16_t DeviceAddress; 

    /*! \brief The configuration of the acknolagement timeout timer. It is actually a little hack to use a class intended for periodic timer interrupts as one shot timer, but since we don't have a timer framework, we use this little trick. The idea is, that the acknolagment timeout is 20 millisecs, so we need a frequency of 50. After the first timer interupt occures, we stop the timer and reset it, so that it starts from the beginning again if neccessary.*/
	struct ClockConfig {
		typedef uint16_t TickValueType;
		typedef Frequency<50> TargetFrequency;
		typedef CPUClock TimerFrequency;
		typedef local::Timer1 Timer;
	};

    /*! \brief This is the interface class of every Mac Protocol. It should provide all neccessary methods that a Mac protocol should have. there are two template parameters. The first one is the Physical Layer. The second one is the activation status of the evaluation feature. This class then inherit from the Physical Layer and the Mac_Evaluation class(Second template parameter of this class is passed as a template parameter of Mac_Evaluation.)*/
    template<class Radiocontroller,MAC_EVALUATION_ACTIVATION_STATE state>
    struct MAC_Base : public Radiocontroller,Mac_Evaluation<state> {
        enum maxwaitingtime { maximal_waiting_time_in_milliseconds = 100 };
        typename Radiocontroller::mob_t message;
        uint16_t nav;
        uint16_t clocktick_counter; 
        MAC_Clock clock;
        DeviceAddress mac_adress_of_node;

        /*! \brief This delegate contains function, that is called if a valid MAC_Message was received. In the function you bind to this Delegate, you should use receive(MAC_Message& msg) to get the received message.*/
        Delegate<> onMessageReceiveDelegate;
        /*! \brief This delegate contains a functionpointer to a function, that is called if a send operation is completed. To get the errorcode  use get_result_of_last_send_operation() in the function to bind to this delegate*/
        Delegate<> onSend_Operation_Completed_Delegtate;

        MAC_Base() {
            mac_adress_of_node = 28;
            init();	
        }

        /*! receiver Thread, if the mac protocol needs an asyncron receive routine*/
        void callback_receive_message() {}
        uint8_t get_global_sequence_number() {
            static uint8_t global_sequencenumber = 0;
            return global_sequencenumber++;
        }

        int init() {
            Radiocontroller::init();
            uint8_t channel = 11;
            Radiocontroller::setAttribute(armarow::PHY::phyCurrentChannel, &channel);
            Radiocontroller::setStateTRX(armarow::PHY::rx_on);
            Mac_Evaluation<state>::init();
            srandom(mac_adress_of_node);
            clocktick_counter = 0;
            nav = 0;
            return 0;
        }

        int reset() {
            Radiocontroller::reset();
            init();
            return 0;
        }

        int send(MAC_Message mac_message) {
            mac_message.header.sequencenumber = get_global_sequence_number();
            int randomnumber = rand();
            uint32_t waitingtime = ( ((uint32_t)randomnumber*maximal_waiting_time_in_milliseconds) / (0x8000));
            delay_ms(waitingtime);	
            ::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << "Sending MAC_Message... " << ::logging::log::endl;
            mac_message.print();

            Radiocontroller::setStateTRX(armarow::PHY::rx_on);
            uint8_t ccaValue;
            armarow::PHY::State status=Radiocontroller::doCCA(ccaValue);
            if ( status == armarow::PHY::idle ) {
                ::logging::log::emit() << PROGMEMSTRING("Medium frei!!!") << ::logging::log::endl << ::logging::log::endl;						
            } else if (status==armarow::PHY::busy) {
                ::logging::log::emit() << PROGMEMSTRING("Medium belegt!!!") << ::logging::log::endl << ::logging::log::endl;
                return -1;
            } else if (status==armarow::PHY::trx_off) {
                ::logging::log::emit() << PROGMEMSTRING("Controller nicht im Receive State!!!") << ::logging::log::endl << ::logging::log::endl;	
                return -1;
            } else {
                ::logging::log::emit() << PROGMEMSTRING("armarow::PHY::State return Value of Clear channel Assessment not in {busy,idle,trx_off}!!!") << ::logging::log::endl << ::logging::log::endl;
                return -1;	
            }
            Radiocontroller::setStateTRX(armarow::PHY::tx_on);
            Radiocontroller::send(*mac_message.getPhysical_Layer_Message());
            return 0;
        }

        int receive(MAC_Message& mac_message) {
            mac_message.setPayloadNULL();
            Radiocontroller::setStateTRX(armarow::PHY::rx_on);
            Radiocontroller::receive(message);
            armarow::MAC::MAC_Message* mac_msg = armarow::MAC::MAC_Message::create_MAC_Message_from_Physical_Message(message);
            if ( mac_msg == (armarow::MAC::MAC_Message*)0 ) return 0;
            mac_message = *mac_msg;
            if ( mac_message.header.controlfield.frametype != Data ) {
                mac_message.print();
                return 0;
            }
            return mac_msg->size;
        }
    };
}
}
