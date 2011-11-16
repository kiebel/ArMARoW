namespace armarow {
namespace mac {
    namespace configuration {
        /*! \brief Default configuration for the Medium Access Layer.*/
        struct DefaultMACConfiguration {
            enum {
                channel                = 11, /*!> default radio channel*/
                nodeAddress            = 28, /*!> default address node*/
                pan_id                 = 0,  /*!> default PAN identifier*/
                ack_request            = 1,  /*!> Don't know*/
                minBackoffExponend     = 2,  /*!> Don't know*/
                maxBackoffExponend     = 7,  /*!> Don't know*/
                maxRetransmissions     = 3,  /*!> Don't know*/
                modePromiscuous        = 0,  /*!> Don't know*/
                acknowledgementTimeout = 100 /*!> Don't know*/
            };
        };
    }
}
}
