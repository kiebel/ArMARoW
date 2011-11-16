#pragma once

namespace armarow {
namespace mac {
    typedef uint16_t DeviceAddress;
    typedef uint16_t PANAddress;

    enum FrameTypeIEEE {
        beacon         = 0,
        data           = 1,
        acknowledgment = 2,
        command        = 3
    };
    enum MessageType{ rts, cts, data, ack };
    struct FrameControlFieldIEEE {
        uint16_t frametype            : 3;
        uint16_t securityenabled      : 1;
        uint16_t framepending         : 1;
        uint16_t ackrequest           : 1;
        uint16_t intraPAN             : 1;
        uint16_t                      : 2;
        uint16_t destaddressingmode   : 2;
        uint16_t                      : 2;
        uint16_t sourceaddressingmode : 2;

        void init() {
            frametype            = armarow::mac::MessageType::data;
            securityenabled      = 0;
            framepending         = 0;
            ackrequest           = 0;
            intraPAN             = 0;
            destaddressingmode   = 2;
            sourceaddressingmode = 2;
        }
    } __attribute__((packed));

    /*! \brief Data structure encapsulating the header of the Medium Access Layer (MAC).*/
    struct FrameHeaderMAC {
        FrameControlFieldIEEE controlfield;
        uint8_t sequencenumber;
        PANAddress dest_pan;
        DeviceAddress dest_adress;
        PANAddress source_pan;
        DeviceAddress source_adress;

        FrameHeaderMAC(FrameTypeIEEE messageType, DeviceAddress source, DeviceAddress destination) {
            controlfield.init();
            controlfield.frametype = messageType;
            source_adress = source;
            dest_adress   = destination;
            sequencenumber = 0;
            dest_pan       = 0; 
            source_pan     = 0;
        }

        /*! \brief Prints out the format of the frame header using the logging framework.*/
        void printFormat() {
            ::logging::log::emit()
                << "SIZE OF FrameHeaderMAC:" << sizeof(FrameHeaderMAC) << ::logging::log::endl
                << "SIZE OF FrameControlFieldIEEE: " <<  sizeof(FrameControlFieldIEEE) << ::logging::log::endl
                << "SIZE OF sequencenumber: " << sizeof(sequencenumber) << ::logging::log::endl << ::logging::log::endl
                << "SIZE OF dest_pan: " << sizeof(dest_pan) << ::logging::log::endl
                << "SIZE OF dest_adress: " << sizeof(dest_adress) << ::logging::log::endl
                << "SIZE OF source_pan: " << sizeof(source_pan) << ::logging::log::endl
                << "SIZE OF source_adress: " <<  sizeof(source_adress) << ::logging::log::endl;
        }

        /*! \brief Prints out the content of a frame header using the logging framework.*/
        void printContent() {
            ::logging::log::emit()
                << "FrameHeaderMAC:" << ::logging::log::endl
                << "message_type: " << (int) controlfield.frametype << ::logging::log::endl << ::logging::log::endl
                << "ackrequest: " << (int) controlfield.ackrequest << ::logging::log::endl << ::logging::log::endl
                << "sequencenumber: " << (int) sequencenumber << ::logging::log::endl << ::logging::log::endl
                << "dest_pan: " << (int) dest_pan << ::logging::log::endl
                << "dest_adress: " << (int) dest_adress << ::logging::log::endl
                << "source_pan: " << (int) source_pan << ::logging::log::endl
                << "source_adress: " <<  (int) source_adress << ::logging::log::endl;
        }

        explicit FrameHeaderMAC(platform::config::mob_t physical_layer_message) {} //FIXME
    } __attribute__((packed));

    /*! \brief Data structure encapsulating the message frame of the Medium Access Layer (MAC).*/
    struct MessageFrameMAC {
        public:
            uint8_t size;
            FrameHeaderMAC header;
            char payload[(platform::config::rc_t::info::frame - sizeof(FrameHeaderMAC))]; //FIXME move to appropriate enum payloadSize
            struct {
                uint8_t lqi;  /*!< last measured LQI value*/
                uint8_t ed;
                bool    crc  : 1;
            } minfo; //FIXME create a typedef and move

            explicit MessageFrameMAC() {
                DeviceAddress source      = 25; //FIXME what does 25 mean?
                DeviceAddress destination = 38; //FIXME what does 38 mean?
                new (&header) FrameHeaderMAC( MessageType::data, source, destination); //FIXME what is happening here?

                header.sequencenumber = 0;
                header.dest_pan       = 0;
                header.source_pan     = 0;
                header.controlfield.init();
                setDefaultPayload();
                size                  = 0;
            }
            explicit MessageFrameMAC(FrameTypeIEEE msgtyp, DeviceAddress source, DeviceAddress destination, uint8_t* dataBuffer, uint8_t dataSize) {
                if ( dataSize > (platform::config::rc_t::info::frame - sizeof(FrameHeaderMAC)) ) { //FIXME move to appropriate enum payloadSize
                    ::logging::log::emit()
                        << "FATAL ERROR: FAILED to create MessageFrameMAC object, since payload is to small"
                        << ::logging::log::endl;
                    return;
                }
                new (&header) FrameHeaderMAC( msgtyp, source, destination);
                if ( (platform::config::rc_t::info::frame - sizeof(FrameHeaderMAC)) < dataSize ) { //FIXME seems unnecessary since already checked
                    ::logging::log::emit()
                        << "ERROR: MAC_Payload: number of bytes in databuffer does not fit in MAC_Payload! -> decrease size of databuffer or transmit multiple MessageFrameMACs!"
                        << ::logging::log::endl;
                    return;
                }

                for(int index=0; index < dataSize; index++) {
                    payload[index]=dataBuffer[index];
                }
                size = dataSize;
            }
            void setDefaultPayload() {
                for (uint8_t index=0; index < (platform::config::rc_t::info::frame - sizeof(FrameHeaderMAC)); index++) { //FIXME move to appropriate enum payloadSize
                    payload[index]=0;
                }
            }
            void print() {
                ::logging::log::emit() 
                    << "FrameHeaderMAC:" << ::logging::log::endl
                    << "message_type: " << (int) header.controlfield.frametype << ::logging::log::endl << ::logging::log::endl
                    << "ackrequest: " << (int) header.controlfield.ackrequest << ::logging::log::endl << ::logging::log::endl
                    << "sequencenumber: " << (int) header.sequencenumber << ::logging::log::endl << ::logging::log::endl
                    << "dest_pan: " << (int) header.dest_pan << ::logging::log::endl
                    << "dest_adress: " << (int) header.dest_adress << ::logging::log::endl
                    << "source_pan: " << (int) header.source_pan << ::logging::log::endl
                    << "source_adress: " <<  (int) header.source_adress << ::logging::log::endl;
            }
            void hexdump() { //FIXME is this method necessary
                uint8_t loopcounter = 0;
                uint8_t* pointer = (uint8_t*) this;

                ::logging::log::emit() 
                    << ::logging::log::endl << "=== BEGIN HEX DUMP ===" << ::logging::log::endl << ::logging::log::endl
                    << ::logging::log::hex;
                for (uint8_t index=0; index < sizeof(MessageFrameMAC); index++) { //FIXME this loop is used twice, maybe it should be moved to another method
                    ::logging::log::emit() << (uint16_t) pointer[index] << ","; //FIXME use correct cast
                    if ( loopcounter >= 20 ) {
                        loopcounter=0;
                        ::logging::log::emit() << ::logging::log::endl;
                    }	
                }
                ::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << ::logging::log::dec;
                loopcounter = 0;
                for (uint8_t index=0; index < sizeof(MessageFrameMAC); index++) { //FIXME do we need both hex and dec printout?
                    ::logging::log::emit() << (uint16_t) pointer[i] << ","; //FIXME use correct cast
                    if ( loopcounter >= 20 ) {
                        loopcounter = 0;
                        ::logging::log::emit() << ::logging::log::endl;
                    }	
                }
                ::logging::log::emit() << ::logging::log::endl << "=== END HEX DUMP ===" << ::logging::log::endl 
                    << ::logging::log::endl;
            }
            bool isValid() {
                if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                    ::logging::log::emit() << "Validate MAC Frame..." << ::logging::log::endl;
                }
                if ( size > MAX_NUMBER_OF_DATABYTES ) {
                    ::logging::log::emit() 
                        << "FATAL ERROR: Size of Payload to large! MAX Value: " << MAX_NUMBER_OF_DATABYTES
                        << " Value of Frame: " << (int) size << ::logging::log::endl;
                    return false;
                }
                //FIXME unnecessary functionality, should be moved to a method
                if ( header.controlfield.frametype == FrameTypeIEEE::beacon ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT) {
                        ::logging::log::emit() << "Beacon Message" << ::logging::log::endl;
                    }
                } else if(header.controlfield.frametype == FrameTypeIEEE::command) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        ::logging::log::emit() << "MAC_command Message" << ::logging::log::endl;
                    }
                } else if(header.controlfield.frametype == FrameType::data) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        ::logging::log::emit() << "Data Message" << ::logging::log::endl;
                    }
                } else if ( header.controlfield.frametype == FrameType::acknowledgment ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        ::logging::log::emit() << "Acknowledgment Message" << ::logging::log::endl;
                    }
                } else {
                    ::logging::log::emit() << "FATAL ERROR: failed decoding MAC Message" << ::logging::log::endl;
                    print();
                    return false;
                }
                if ( MAC_LAYER_VERBOSE_OUTPUT ) { //FIXME do we need this output at all?
                    ::logging::log::emit() << "Success..." << ::logging::log::endl;
                }
                return true;
            }
            /*! \brief Stores an object of arbitrary type in a message of the Medium Access Layer (MAC).
             *  \note The size of the object type has to be smaler or equal the available payload size!
             */
            template <class T>
            int storeObject(T& object) {
                static const bool k = ( MAX_NUMBER_OF_DATABYTES >= sizeof(T) ) ? true : false;
                ARMAROW_STATIC_ASSERT_ERROR(k,TYPE_TO_LARGE__DOES_NOT_FIT_IN_MESSAGE_PAYLOAD,(T));
                uint8_t* objectData = (uint8_t*) &object; //FIXME use appropriated cast
                for (uint8_t index = 0; index < sizeof(T); index++)
                    payload[index]=objectData[index];
                    size = sizeof(T);
                return 0; //FIXME what is a return value needed for?
            }

            /*! \brief Loads an object of arbitrary type from the message payload.*/
            template <class T>
            int readObject(T& object) {
                static const bool k = ( MAX_NUMBER_OF_DATABYTES >= sizeof(T) ) ? true : false;
                ARMAROW_STATIC_ASSERT_ERROR(k,TYPE_TO_LARGE__DOES_NOT_FIT_IN_MESSAGE_PAYLOAD,(T));
                uint8_t* objectData = (uint8_t*) &object;
                for (uint8_t index = 0; index < sizeof(T); index++)
                    objectData[index] = payload[index];
                return 0; //FIXME what is a return value needed for?
            }

            platform::config::mob_t* getPhysicalLayerMessage() {
                size += sizeof(FrameHeaderMAC);
                return (platform::config::mob_t*)this; //FIXME use appropriated cast
            }

            /*! \brief Converts a message from the Physical Lyaer into a message of the Medium Access Layer (MAC).
             *
             *  \param message Physical Layer message
             *  \return a valid MessageFrameMAC upon successful decoding, otherwise NULL.
             */
            static MessageFrameMAC* transformPhysicalLayerMessageIntoMediumAccessLayerMessage( platform::config::mob_t& message) {
                bool decodingSuccessful = true;
                MessageFrameMAC* messageMAC = (MessageFrameMAC*) new(&message) MessageFrameMAC(message, decodingSuccessful);
                return ( ( decodingSuccessful ) ? messsageMAC : (MessageFrameMAC*)0 ); //FIXME use reference or NULL
            }
        private:
            explicit MessageFrameMAC(platform::config::mob_t& physical_layer_message, bool& decoding_was_successful) {
                if ( physical_layer_message.size < sizeof(header) ) {
                    decoding_was_successful = false;
                    ::logging::log::emit()
                        << "ERROR: MessageFrameMAC constructor: physical Message to short, to contain a FrameHeaderMAC!"
                        << ::logging::log::endl; << "Minimal Size: " << sizeof(header) << " size of current MAC_Frame: "
                        << (int) physical_layer_message.size << ::logging::log::endl << ::logging::log::endl;
                    print();
                    ::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << ::logging::log::endl; //FIXME is this needed???
                    return;
                }

                size      = physical_layer_message.size - sizeof(FrameHeaderMAC);
                minfo.ed  = physical_layer_message.minfo.ed;
                minfo.lqi = physical_layer_message.minfo.lqi;

                decoding_was_successful = (decoding_was_successful) ? isValid() : decoding_was_successful;
                static const bool k = (sizeof(platform::config::mob_t) == sizeof(MessageFrameMAC) ) ? true : false;
                ARMAROW_STATIC_ASSERT_ERROR(k,MessageFrameMAC_AND_PHY_MESSAGE_HAVE_DIFFERENT_SIZES__MESSAGEDECODING_WILL_NOT_WORK,(int));
            }
    } __attribute__((packed));
};
};
