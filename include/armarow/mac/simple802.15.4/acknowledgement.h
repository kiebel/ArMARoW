namespace armarow {
namespace mac {
namespace simple802_15_4
{

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

        MAC_Clock AcknowledgementTimeoutTimer; //FIXME should be moved into the AcknowledgementHandler
        MessageFrameMAC bufferACK; //FIXME do we realy need to store another comlete message (maybe preconfigured smaler payload)
        uint8_t expectedSequenceNumber;
        uint8_t destination_id_of_last_transmitted_message;
        uint8_t destination_panid_of_last_transmitted_message;
        uint8_t timeout_counter_in_ms; //FIXME how can a counter use milli secounds

        uint8_t timeoutDuration;
        ack_error_code lastErrorCode;

        AcknowledgementHandler() {
            reset();
            timeoutDuration    = MACCFG::timeoutDurationACK;
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

        void print() {
            log::emit()
                << "receivedACK: " << (int)receivedACK << log::endl
                << "handlerWaiting: " << (int)handlerWaiting << log::endl
                << "RetransmissonCount: " << (int)BackoffTiming.RetransmissonCount << log::endl
                << "timeout_counter_in_ms: " << (int)timeout_counter_in_ms << log::endl
                << "timeout_counter_in_ms: " << (int)receivedTimeout << log::endl;
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
        void receivedACK(MessageFrameMAC& acknowledgement, volatile bool& messageReadyFlag, Delegate<>& onSendCompleted, layer& alayer) {
            log::emit<log::Trace>()
                << "ack was received, validating..." << log::endl
                << "last message sequence number: " << (int) expectedSequenceNumber << log::endl
                << "ack sequence number: " << (int) acknowledgement.header.sequencenumber << log::endl;

            if ( expectedSequenceNumber == acknowledgement.header.sequencenumber ) {
                alayer.reset_acknowlegement_timer(); //FIXME why is acknowledgement handling separated from the acknowledgement timer

                receivedACK        = true;
                handlerInitialized = false;
                handlerWaiting     = false;
                messageReadyFlag   = false;
                lastErrorCode      = success;

                log::emit<log::Trace>()
                    << "received ACK for message " << (int) expectedSequenceNumber << log::endl
                    << "waiting time in ms: " << (int) timeout_counter_in_ms << " current timeout duration: "
                    << (int) timeout_duration_in_ms << log::endl;

                onSendCompleted();
            }
        }

        /*! \brief  Initializes the acknowledgement timeout mechanism for a given message.
            *
            *  \param messageObject reference to the message waiting for an acknowledgement
            *  \param aLayer Medium Access Layer used with this acknowledgement timemout.
            *  \todo move the acknowledge timeout timer into the acknowledge handler
            *  \todo remove the unnecessary return value
            */
        ack_error_code initializeAcknowledgementTimeout(MessageFrameMAC& messageObject, layer& aLayer) {
            log::emit<log::Trace>() << "wait for ACK..." << log::endl;

            expectedSequenceNumber   = messageObject.header.sequencenumber;
            destination_id_of_last_transmitted_message    = messageObject.header.dest_adress;
            destination_panid_of_last_transmitted_message = messageObject.header.dest_pan;
            reset();

            handlerWaiting     = true;
            handlerInitialized = true;
            aLayer.acknowlegement_timeout_timer.start(); //FIXME why is acknowledgement handling separated from the acknowledgement timer

            return success; //FIXME why does the method has a fixed return value
        }

                /*! \brief  Acknowledges a given message.
            *
            *  \param messageObjec a valid message (IEEE data message)
            *  \return an errorcode (\see AcknowledgementHandler::ErrorCode)
            */
        int acknowledgeMessage(MessageFrameMAC& messageObject) {
            if ( messageObject.header.controlfield.ackrequest == 0 ) {
                return handlerACK::success; //FIXME is it realy a success if we do nothing
            }

            handlerACK.bufferACK.header.sequencenumber = messageObject.header.sequencenumber;
            handlerACK.bufferACK.header.dest_pan       = messageObject.header.source_pan;
            handlerACK.bufferACK.header.dest_adress    = messageObject.header.source_adress;
            handlerACK.bufferACK.header.source_adress  = MACCFG::nodeAddress;
            handlerACK.bufferACK.header.source_pan     = MACCFG::PANId;
            handlerACK.bufferACK.header.controlfield.frametype = Acknowledgment;//FIXME scope mit angeben
            handlerACK.bufferACK.size = 0;
            handlerACK.bufferACK.header.controlfield.ackrequest = 0;

            log::emit<log::Trace>()
                << "sending ACK..." << log::endl
                << "====> start ACK msg..." << log::endl << log::endl;
            handlerACK.bufferACK.print();
            log::emit<log::Trace>()
                << "====> end ACK msg..." << log::endl << log::endl;

            handlerACK.handlerReady = true;
            sendMessage();

            return handlerACK::success;
        }

        /*! \brief  Resets the timer for the acknowledgement timeout.*/
        void resetAcknowledgementTimeout() { //FIXME should be moved into the AcknowledgementHandler
            acknowledgementTimeoutTimer.stop();
            acknowledgementTimeoutTimer.setCounter(0);
        }

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

                if ( handlerACK.BackoffTiming.RetransmissionsCount <= handlerACK.BackoffTiming.MaxRetransmissionCount ) {
                    log::emit<log::Trace>()
                        << "retry transmitting... attempt number "
                        << (int) handlerACK.BackoffTiming.RetransmissionCount
                        << log::endl;
                    handlerACK.BackoffTiming.BackoffExponend = MACCFG::MinBackoffExponend;
                    sendMessage();
                } else {
                    handlerACK.lastErrorCode = handlerACK::timeout;
                    messageReadyFlag = false;
                    log::emit<log::Trace>()
                        << "TIMEOUT..." << log::endl
                        << "number of retries has exeeded..." << log::endl;
                    onSendCompleted();
                }
            }
        }
    };
}
}
}
