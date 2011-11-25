namespace armarow {
namespace mac {
    namespace ieee802_15_4 {

        /*! \brief  Encapsulates variables and program logic of the IEEE backoff timing computation.
         *  \todo add template parameter for Min/MaxRetransmissionCount and Min/MaxbackoffExponent
         */
        struct BackoffTiming {
            uint8_t BackoffExponend;
            uint8_t MaxRetransmissionCount; //FIXME is not used here
            uint8_t RetransmissionCount;     //FIXME is not used here

            BackoffTiming() {
                reset();
                MaxRetransmissionCount = MACCFG::MaxRetransmissionCount; //FIXME use template parameter
            }

            /*! \brief  Indicates that the number of maximal backoffs has been reached.*/
            bool exceededBackofCount() {
                return ( BackoffExponend > MACCFG::MaxBackoffExponend ); //FIXME use template parameter
            }

            /*! \brief  Returns a random backoff time in milli secound.
             *  \note   As defined in the IEEE 802.15.4 the backoff time will grow exponentially with
             *          the number of backoffs.
             */
            uint16_t getBackoffTimeMS() {
                uint32_t backoffTime = ( (uint32_t)rand() * 2^(BackoffExponend) ) / (0x8000);
                BackoffExponend++;

                if(MAC_LAYER_VERBOSE_OUTPUT) {
                    log::emit()
                        << "random waiting time in ms: " << (int) backoffTime << log::endl;
                }

                return (uint16_t)backoffTime;
            }

            /*! \brief Resets the IEEE backoff timing computation.*/
            void reset() {
                BackoffExponend    = MACCFG::MinBackoffExponend; //FIXME use template parameter
                RetransmissionCount = 0;
            }
        };

    }
}
}
