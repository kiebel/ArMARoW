#pragma once

#include <avr/io.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/avr/regmaps.h>
#include <avr-halib/avr/timer.h>
#include <avr-halib/regmaps/local.h>
#include <avr-halib/share/delay.h>
#include <avr-halib/share/delegate.h>
#include <avr-halib/share/interruptLock.h>
#include <stdlib.h>

#include "platform-cfg.h"  //FIXME what is that include needed for
#include <armarow/armarow.h>
#include <armarow/debug.h>
#include <armarow/phy/phy.h>
#include "MediumAccessLayerEvaluation.h"
#include "MediumAccessLayerMessage.h"

UseInterrupt(SIG_OUTPUT_COMPARE1A);

using avr_halib::drivers::Clock;
using namespace avr_halib::regmaps;

namespace armarow {
namespace mac {
    typedef MessageFrameMAC mob_t;
    typedef Clock<ClockConfig> MAC_Clock; //FIXME use appropriated name
    typedef void* AttributType;
    typedef uint16_t DeviceAddress;

    /*! \brief Configuration of the acknowledgment timeout timer.
     *
     *  Since there is no timer framework yet, this configuration is used to work with periodic timer interrupts from a
     *  one shot timer. The idea is, that the acknolagment timeout is 20 millisecs, so we need a frequency of 50. After
     *  the first timer interupt occures, we stop the timer and reset it, so that it starts from the beginning again
     *  if neccessary.
     */
    struct ClockConfig { //FIXME move to a separate file
        typedef uint16_t TickValueType;
        typedef Frequency<50> TargetFrequency;
        typedef CPUClock TimerFrequency;
        typedef local::Timer1 Timer;
    };

    /*! \brief Defines the interface every Medium Access Layer protocol uses.
     *
     *  \tparam PHYL configuration of the used Physical Layer
     *  \tparam MAC_EVALUATION_CATIVATION_STAT stupid name don't know what it is used for maybe not needed (evaluation framework)
     */
    template<class PHYL, MAC_EVALUATION_ACTIVATION_STATE state>
    struct MediumAccessLayerInterface : public PHYL, MACEvaluation<state> {
        typename PHYL::mob_t message;
        DeviceAddress nodeAddress;

        enum maxwaitingtime { maximal_waiting_time_in_milliseconds = 100 }; //FIXME what does this waiting time wait for?
        uint16_t nav;                   //FIXME what is the purpose of nav?
        uint16_t clocktick_counter;     //FIXME what is the purpose of clocktick_counter ? 
        MAC_Clock clock;                //FIXME what is the purpose of clock? not used for the remainder of the interface!!!

        /*! \brief Invoked after a valid message was received.*/
        Delegate<> onMessageReceive;
        /*! \brief Invoked after a message transmission was completed.*/
        Delegate<> onSendCompleted;

        MediumAccessLayerInterface() : nodeAddress(28) { //FIXME what does that value stand for?
            init();
        }

        /*! \brief Don't know.*/
        void messageReceived() {}

        void init() {
            PHYL::init();
            uint8_t channel = 11; //FIXME what does that value stand for?
            PHYL::setAttribute(armarow::PHY::phyCurrentChannel, &channel);
            PHYL::setStateTRX(armarow::PHY::rx_on);
            MACEvaluation<state>::init();
            srandom(nodeAddress);
            clocktick_counter = 0; //FIXME what does that value stand for?
            nav = 0; //FIXME what does that value stand for?
        }

        void reset() {
            PHYL::reset();
            init();
        }

        /*! \brief Sends a Medium Access Layer message using the underlying Physical Layer.
         *
         *  \param messageObject Medium Access Layer message
         *  \return Don't know.
         */
        int send(MessageFrameMAC messageObject) {
            int randomnumber = rand();
            uint8_t ccaValue;
            uint32_t waitingtime = ( ((uint32_t)randomnumber*maximal_waiting_time_in_milliseconds) / (0x8000)); //FIXME what does 0x8000 stand for?

            messageObject.header.sequencenumber = getGlobalSequenceNumber();
            delay_ms(waitingtime); //FIXME why are we waiting? waiting time seems influenced by the logging!!
            log::emit() << log::endl << log::endl << "Sending MessageFrameMAC... " << log::endl;
            messageObject.print();

            PHYL::setStateTRX(armarow::PHY::rx_on);
            armarow::PHY::State status = PHYL::doCCA(ccaValue);
            //FIXME Most of this IF construct seems to be for logging purposes only, do we need it that way?
            if ( status == armarow::PHY::idle ) {
                log::emit()
                    << PROGMEMSTRING("Medium frei!!!") << log::endl << log::endl;
            } else if (status==armarow::PHY::busy) {
                log::emit()
                    << PROGMEMSTRING("Medium belegt!!!") << log::endl << log::endl;
                return -1; //FIXME do we have named error states for send
            } else if (status==armarow::PHY::trx_off) {
                log::emit()
                    << PROGMEMSTRING("Controller nicht im Receive State!!!") << log::endl
                    << log::endl;
                return -1; //FIXME do we have named error states for send
            } else {
                log::emit()
                    << PROGMEMSTRING("armarow::PHY::State return Value of Clear channel Assessment not in {busy,idle,trx_off}!!!")
                    << log::endl << log::endl;
                return -1; //FIXME do we have named error states for send
            }
            PHYL::setStateTRX(armarow::PHY::tx_on);
            PHYL::send(*messageObject.getPhysicalLayerMessage());
            return 0; //FIXME do we have named error states for send
        }

        int receive(MessageFrameMAC& messageObject) {
            PHYL::setStateTRX(armarow::PHY::rx_on);
            PHYL::receive(message);

            //FIXME not sure if i understand what's happening here, i think the content of message needs to be transformed into messageObject?
            armarow::mac::MessageFrameMAC* messageMAC = armarow::mac::MessageFrameMAC::transformPhysicalLayerMessageIntoMediumAccessLayerMessage(message);

            if ( messageMAC == NULL || messageMAC->header.controlfield.frametype != data ) {
                messageMAC->print();
                return 0;
            }
            messageObject = *messageMAC;
            return message->size; //FIXME do we want to return size of message on Physical or MAC layer?
        }
        private:
            uint8_t getGlobalSequenceNumber() {
                static uint8_t globalSequenceNumber = 0;
                return globalSequenceNumber++;
            }
    };
}
}
