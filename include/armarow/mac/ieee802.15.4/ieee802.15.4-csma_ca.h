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

#include <boost/static_assert.hpp>
#include <armarow/mac/MediumAccessLayer.h>
#include <armarow/mac/MediumAccessLayerAddress.h>
#include <armarow/mac/MediumAccessLayerConfiguration.h>

#include "ieee802.15.4-acknowledgement.h"
#include "ieee802.15.4-MessageFilter.h"


UseInterrupt(SIG_OUTPUT_COMPARE3A);

namespace armarow {
namespace mac {
    typedef void* AttributType;
    typedef uint16_t DeviceAddress; //FIXME should be provided by "armarow/mac/MediumAccessLayer.h"
    typedef MessageFrameMAC mob_t;

    /*! \brief  Implementation of the IEEE 802.15.4 Medium Access Layer protocol using non beacon and CSMA/CA.
     *
     *  \tparam MACCFG configuration for the Medium Access Layer
     *  \tparam PHYL Physical Layer implementation used for this layer
     *  \tparam MAC_EVALUATION_ACTIVATION_STATE indicates whether the evaluation is activated or not
     */
    template<class MACCFG, class PHYL, MAC_EVALUATION_ACTIVATION_STATE EvaluationState> struct MacCsmaCa:
        public MediumAccessLayerInterface<PHYL, EvaluationState>
    {
        typedef MacCsmaCa<MACCFG, PHYL, EvaluationState> layer;
    protected:
        ExactEggTimer<Timer3> one_shot_timer;

        MessageFrameMAC bufferRECV;
            MessageFrameMAC bufferSEND;
            typename PHYL::mob_t bufferPhyRECV; //FIXME why do we need a both bufferRECV and bufferPhyRECV?

            /*! \brief Flag that indicates there is a message ready to be delivered.*/
            volatile bool messageWaitingFlag; //FIXME can we use a bitfield here?
            /*! \brief Flag that indicates there is a message ready to be delivered.*/
            volatile bool messageReadyFlag;   //FIXME can we use a bitfield here?
            //FIXME datafield for AcknowledgementHandler handlerACK and MessageFilter messageFLT

            ieee802_15_4::AcknowledgementHandler handlerACK;
            ieee802_15_4::MessageFilter messageFLT;

            /*! \brief  Acknowledges a given message.
             *
             *  \param messageObjec a valid message (IEEE data message)
             *  \return an errorcode (\see AcknowledgementHandler::ErrorCode)
             */
            int acknowledgeMessage(MessageFrameMAC& messageObject) {
                if ( messageObject.header.controlfield.ackrequest == 0 ) {
                    return handlerACK.success; //FIXME is it realy a success if we do nothing
                }

                handlerACK.bufferACK.header.sequencenumber = messageObject.header.sequencenumber;
                handlerACK.bufferACK.header.dest_pan       = messageObject.header.source_pan;
                handlerACK.bufferACK.header.dest_adress    = messageObject.header.source_adress;
                handlerACK.bufferACK.header.source_adress  = MACCFG::nodeAddress;
                handlerACK.bufferACK.header.source_pan     = MACCFG::PANId;
                handlerACK.bufferACK.header.controlfield.frametype = acknowledgment;//FIXME scope mit angeben
                handlerACK.bufferACK.size = 0;
                handlerACK.bufferACK.header.controlfield.ackrequest = 0;

                if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT ) {
                    log::emit()
                        << "sending ACK..." << log::endl
                        << "====> start ACK msg..." << log::endl << log::endl;
                    handlerACK.bufferACK.print();
                    log::emit()
                        << "====> end ACK msg..." << log::endl << log::endl;
                }

                handlerACK.handlerReady = true;
                sendMessage();

                return handlerACK.success;
            }

            /*! \brief Run to completion Task of the Mac protocol, tries to send the message that is stored in the bufferSEND buffer. Note, that tasks are not intended to be called directly by the user. Some general behaviour of this function: Acknolagement messages have priority over data messages. The backoff timing behaviour is different for Ack and Data messages. Data messages have the IEEE backoff timing implementation. Ack messages have a constant backoff time, because they are time critical. If the medium is busy, then a oneshot timer is started, that calls this Task again after the specified timing behaviour. */
            void sendMessage() {
                uint8_t ccaValue = 0;
                armarow::PHY::State status;
                MessageFrameMAC messageObject = bufferSEND; //FIXME yet another copy

                {
                    avr_halib::locking::GlobalIntLock lock;

                    if ( handlerACK.handlerWaiting == true ) {
                        messageObject = handlerACK.bufferACK; //FIXME yet another copy
                    }
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "enter send ISR" << log::endl;

                    one_shot_timer.stop(); //FIXME whats the purpose of this timer?

                    if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "do CCA" << log::endl;

                    status = PHYL::doCCA(ccaValue);

                    log::emit() << "finished CCA" << log::endl;
                    if ( status == armarow::PHY::success && ccaValue ) {
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "sending..." << log::endl;
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) messageObject.print();

                        PHYL::setStateTRX( armarow::PHY::tx_on );
                        PHYL::send( *messageObject.getPhysicalLayerMessage() );
                        PHYL::setStateTRX( armarow::PHY::rx_on );

                        if ( handlerACK.handlerWaiting == true ) {
                            handlerACK.handlerWaiting = false;
                            if ( messageReadyFlag ) one_shot_timer.start(1);
                            return;
                        }
                        if ( messageObject.header.controlfield.ackrequest == 1 ) {
                            //FIXME handlerACK.init_waiting_mechanism_for_ACK_for_MAC_Message(messageObject, *this);
                        } else {
                            handlerACK.lastErrorCode = handlerACK.success;
                            messageReadyFlag = false;
                            if ( !this->onSendCompleted.isEmpty() ) onSendCompleted();
                        }
                    } else {
                        one_shot_timer.stop();
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << PROGMEMSTRING("Medium busy...") << log::endl;
                        if ( handlerACK.handlerWaiting == true ) {
                            uint32_t waitingtime = (((uint32_t)rand() * 20) / (0x8000)); //FIXME should be a method
                            one_shot_timer.start((uint16_t)waitingtime);
                            return;
                        }
                        if ( handlerACK.backoff_timing.exceededBackofCount() ) {
                            messageReadyFlag = false;
                            handlerACK.lastErrorCode = handlerACK.medium_busy;
                            if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "number of backoffs has exeeded..." << log::endl;
                            handlerACK.backoff_timing.reset();
                            if ( !onSendCompleted.isEmpty() ) onSendCompleted();
                        } else {
                            one_shot_timer.start(handlerACK.backoff_timing.getBackoffTimeMS());
                        }
                    }
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "leave send ISR" << log::endl;
                }
            }
 

        public:
        using MediumAccessLayerInterface<PHYL, EvaluationState>::onSendCompleted;
        using MediumAccessLayerInterface<PHYL, EvaluationState>::onMessageReceive;
        using MediumAccessLayerInterface<PHYL, EvaluationState>::onReceive;
        using MediumAccessLayerInterface<PHYL, EvaluationState>::getGlobalSequenceNumber;
            MacCsmaCa() {
                init();
            }

            //============== Start Interrupt Service Routines =============================================================================
            /*! \brief  Run to completion Task of the Mac protocol, handles the timout event if an acknolagement message wasn't received in time. If a timeout event occures, then this task will be activated from the acknolagement timeout clock. Then it will determine, whether a retransmission is attempted or not. If the maximal number of retransmissions is already exceeded, then the onSend_Operation_Completed_Delegtate is called to notify the user about the failed transmission. Otherwise, we will just retry transmitting the message.*/
            void callback_wait_for_ack_timeout() {
                if ( handlerACK.handlerReady ) {
                    avr_halib::locking::GlobalIntLock lock;

                    handlerACK.backoff_timing.RetransmissionCount++;
                    handlerACK.handlerWaiting     = false;
                    handlerACK.timeout_counter_in_ms = 0;
                    handlerACK.receivedACK        = false;
                    handlerACK.handlerInitialized = false;
                    handlerACK.receivedTimeout    = true;

                    if ( handlerACK.backoff_timing.RetransmissionCount <= handlerACK.backoff_timing.MaxRetransmissionCount ) {
                        if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT ) {
                            log::emit()
                                << "retry transmitting... attempt number "
                                << (int) handlerACK.backoff_timing.RetransmissionCount
                                << log::endl;
                        }
                        handlerACK.backoff_timing.BackoffExponend = MACCFG::MinBackoffExponend;
                        sendMessage();
                    } else {
                        handlerACK.lastErrorCode = handlerACK.timeout;
                        messageReadyFlag = false;
                        if ( MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT ) {
                            log::emit()
                                << "TIMEOUT..." << log::endl
                                << "number of retries has exeeded..." << log::endl;
                        }
                        if ( !onSendCompleted.isEmpty() ) onSendCompleted();
                    }
                }
            }

            /*! callback_receive_message \brief Run to completion Task of the Mac protocol, is executed by the Physical Layer, if a new message is received. The Task will first try to decode the Physical Layer Message. If the decoding fails the message is dropped. Otherwise the received Mac Message is passed to a Filter. If the message is adressed at us, we keep it, otherwise we drop it. If we received a Data Message, we send an acknolagement message if requested (send ack bit in the Mac Header controlfield) and call the onMessageReceiveDelegate to notify the user about the received message.*/
            void receiveMessage() {
                avr_halib::locking::GlobalIntLock lock;

                if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "entered receive message interupt" << log::endl;

                PHYL::receive(bufferPhyRECV);
                messageWaitingFlag = true;
                MessageFrameMAC* messageObject = MessageFrameMAC::transformPhysicalLayerMessageIntoMediumAccessLayerMessage(bufferPhyRECV);
                if ( messageObject == NULL ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "leave receive ISR" << log::endl;
                    messageWaitingFlag = false;
                    return;
                }
                bufferRECV = *messageObject; //TODO message in bufferRECV has been copied three times at this point!!!
                //================= MESSAGE FILTERING START =================
                //FIXME move filtering related functionality into MessageFilter
                if ( MACCFG::modePromiscuous == 0 ) {
                    if ( messageFLT.isBroadcast(bufferRECV) || messageFLT.isDestination(bufferRECV) ) {
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "accepted message..." << log::endl;
                    } else {
                        if ( MAC_LAYER_VERBOSE_OUTPUT) {
                            log::emit() << "dropped message...("
                                << (int) bufferRECV.header.controlfield.frametype << ","
                                << (int) bufferRECV.header.sequencenumber << ","
                                << (int) bufferRECV.header.source_adress << ")" << log::endl
                                << "leave receive ISR" << log::endl;
                        }
                        messageWaitingFlag = false;
                        return;
                    }
                    if ( ENABLE_FILTERING_OF_DUPLICATES ) { //FIXME maybe this could be a template feature of MessageFilter
                        if ( messageFLT.isDuplicate(bufferRECV) ) {
                            if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                                log::emit()
                                    << "filtered out duplicate message...(" << (int) messageFLT.lastSequenceNumber
                                    << "," << (int) messageFLT.lastSourceID << ","
                                    << (int) messageFLT.lastSourcePANId << ")" << log::endl
                                    << "leave receive ISR" << log::endl;
                            }

                            messageWaitingFlag = false;
                            return;
                        }
                        messageFLT.setFilteringMessage(bufferRECV);
                    }
                    //================= MESSAGE FILTERING END ==================

                    if(bufferRECV.header.controlfield.frametype != data) {
                        if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                            log::emit() << "===> got meta msg..." << log::endl;
                            bufferRECV.print();
                            log::emit() << "===> end meta msg..." << log::endl;
                        }
                        messageWaitingFlag = false;
                        if ( bufferRECV.header.controlfield.frametype == acknowledgment ) {
                            handlerACK.receivedACK(bufferRECV, messageReadyFlag, onSendCompleted, *this);
                        }
                        return;
                    }
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        bufferRECV.print();
                    }

                    acknowledgeMessage(bufferRECV);

                    //FIXME evaluation framework this->add_number_of_received_bytes(bufferRECV.size);

                    messageWaitingFlag = true;

                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        log::emit()
                            << "leaving receive message interrupt, calling delegate" << log::endl;
                    }
                    if ( !onMessageReceive.isEmpty() ) onMessageReceive();
                }
                if ( MAC_LAYER_VERBOSE_OUTPUT ) log::emit() << "leave receive ISR" << log::endl;
            }
           //============== END Interrupt Service Routines ===============================================================================

           /*! \brief  ReInitializes the Medium Access Layer protocol.*/
            void init() {
                uint8_t channel    = MACCFG::channel;
                uint8_t phyCCAMode = 2; //FIXME use correct enumeration value

                onReceive.template bind<layer, &MacCsmaCa::receiveMessage>(this);
                one_shot_timer.onTimerDelegate.template bind<layer, &MacCsmaCa::sendMessage>(this);
                handlerACK.timer.registerCallback<typeof *this, &MacCsmaCa::callback_wait_for_ack_timeout>(*this);

                MediumAccessLayerInterface<PHYL, EvaluationState>::init();
                PHYL::setAttribute(armarow::PHY::phyCurrentChannel, &channel); 
                PHYL::setAttribute(armarow::PHY::phyCCAMode, &phyCCAMode);

                messageWaitingFlag = false;
                messageReadyFlag   = false;

                handlerACK.timeoutDuration    = MACCFG::timeoutDurationACK;
                handlerACK.resetTimeout();
            }

            /*! \brief Reinitializes both Physical and Medium Access Layer.*/
            void reset() {
                PHYL::reset();
                init();
            }

            /*! \brief  Gets the error code of the last send operation.
             *  \return the result of the last send operation (\see AcknowledgementHandler::ErrorCode)
             */
            int getLastSendResult() {
                return handlerACK.lastErrorCode;
            }

            /*! \brief  Transmitts a Medium Access Controll message asynchronously.
             *  \note   If the message could not be transmitted, transmission is periodically tryed.
             *  \param  messageObject reference of the actual message
             */
            int send(MessageFrameMAC& messageObject) {
                avr_halib::locking::GlobalIntLock lock;
                if ( !messageReadyFlag ) {
                    messageReadyFlag = true;
                    messageObject.header.sequencenumber = getGlobalSequenceNumber();
                    messageObject.header.source_adress  = MACCFG::nodeAdress;
                    messageObject.header.source_pan     = MACCFG::PANId;
                    handlerACK.backoff_timing.reset();
                    bufferSEND = messageObject; //FIXME why does the message needs to be copied here?
                    sendMessage();
                    return 0;
                }
                return -1;
            }

            /*! \brief Receives a message synchronously.
             *  \note  Returns an already received if this message is new or waits until a new message arrives.
             */
            int receive(MessageFrameMAC& messageObject) {
                PHYL::setStateTRX(armarow::PHY::rx_on);
                while ( !messageWaitingFlag );
                messageWaitingFlag = false;
                messageObject = bufferRECV; //TODO the message has been copied at least twice if not more at this point
                return messageObject.size;
            }

    };
}
}
