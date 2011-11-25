#include "ieee802.15.4-BackoffTimingImpl.h"
namespace armarow {
namespace mac {
    namespace ieee802_15_4 {

        /*! \brief  Encapsulates variables and program logic of an acknowledgement mechanism.
         *
         *  After sending a message the acknowledgement mechanism is initialized. If an expected acknowledgement
         *  is received the appropriated callback function is called. If no acknowledgement was received and the
         *  waiting time is exceeded an error will be reported and the handler will be reset.
         *  \todo make AcknowledgementHandler as independent from the Medium Access Layer as possible
         *  \todo move the acknowledgement timer into the hcknowledgement handler
         *  \todo use template parameter if necessary
         *  \todo use appropriated names for variables
         */
        struct AcknowledgementHandler {
            enum ErrorCode {
                success,    /*!< Don't know.*/
                timeout,    /*!< Don't know.*/
                medium_busy /*!< Don't know.*/
            };
            volatile struct {
                bool handlerInitialized : 1; /*!< Don't know.*/
                bool handlerReady       : 1; /*!< Don't know.*/
                bool handlerWaiting     : 1; /*!< Don't know.*/
                bool receivedACK        : 1; /*!< Don't know.*/
                bool receivedTimeout    : 1; /*!< Don't know.*/
            };

            MessageFrameMAC bufferACK; //FIXME do we realy need to store another comlete message (maybe preconfigured smaler payload)
            MAC_Clock timer;
            BackoffTiming backoff_timing;

            uint8_t expectedSequenceNumber;
            uint8_t destination_id_of_last_transmitted_message;
            uint8_t destination_panid_of_last_transmitted_message;
            uint8_t timeout_counter_in_ms; //FIXME how can a counter use milli secounds

            uint8_t timeoutDuration;
            ErrorCode lastErrorCode;

            AcknowledgementHandler() {
                reset();
                handlerInitialized = false;
                lastErrorCode      = success;
            }

            void reset() {
                receivedACK     = false;
                handlerWaiting  = false;
                timeout_counter_in_ms = 0;
                receivedTimeout = false;
                handlerReady    = false;
            }


            /*! \brief  Handles received acknowledgements by calling onSendCompleted for expected acknowledgements.
             *  \note   Acknowledgements that where not expected are ignored!
             *  \param  acknowledgement received message frame
             *  \param  messageReadyFlag coresponding flag of the Medium Access Layer
             *  \param  onSendCompleted delegate that is called if the acknowledgement was expected
             *  \param  alayer Don't know.
             *  \todo   move reset_acknowledgement_timer into the AcknowledgementHandler
             *  \todo   provide the delegate for onSendComplete as template parameter
             */
            void handleACK(MessageFrameMAC& acknowledgement, volatile bool& messageReadyFlag, Delegate<>& onSendCompleted) {
                if(MAC_LAYER_VERBOSE_OUTPUT) {
                    log::emit()
                        << "ack was received, validating..." << log::endl
                        << "last message sequence number: " << (int) expectedSequenceNumber << log::endl
                        << "ack sequence number: " << (int) acknowledgement.header.sequencenumber << log::endl;
                }
                if ( expectedSequenceNumber == acknowledgement.header.sequencenumber ) {
                    resetTimeout();

                    receivedACK        = true;
                    handlerInitialized = false;
                    handlerWaiting     = false;
                    messageReadyFlag   = false;
                    lastErrorCode      = success;

                    if(MAC_VERBOSE_ACK_OUTPUT || MAC_LAYER_VERBOSE_OUTPUT) {
                        log::emit()
                            << "received ACK for message " << (int) expectedSequenceNumber << log::endl
                            << "waiting time in ms: " << (int) timeout_counter_in_ms << " current timeout duration: "
                            << (int) timeoutDuration << log::endl;
                    }

                    if ( !onSendCompleted.isEmpty() ) onSendCompleted();
                }
            }

            /*! \brief  Initializes the acknowledgement timeout mechanism for a given message.
             *
             *  \param messageObject reference to the message waiting for an acknowledgement
             *  \param aLayer Medium Access Layer used with this acknowledgement timemout.
             *  \todo move the acknowledge timeout timer into the acknowledge handler
             *  \todo remove the unnecessary return value
             */
            ErrorCode initializeAcknowledgementTimeout(MessageFrameMAC& messageObject) {
                if (MAC_LAYER_VERBOSE_OUTPUT) log::emit() << "wait for ACK..." << log::endl;

                expectedSequenceNumber   = messageObject.header.sequencenumber;
                destination_id_of_last_transmitted_message    = messageObject.header.dest_adress;
                destination_panid_of_last_transmitted_message = messageObject.header.dest_pan;
                reset();

                handlerWaiting     = true;
                handlerInitialized = true;
                timer.start(); //FIXME why is acknowledgement handling separated from the acknowledgement timer

                return success; //FIXME why does the method has a fixed return value
            }
            /*! \brief  Resets the timer for the acknowledgement timeout.*/
            void resetTimeout() { //FIXME should be moved into the AcknowledgementHandler
                timer.stop();
                timer.setCounter(0);
            }
        };
    }
}
}
