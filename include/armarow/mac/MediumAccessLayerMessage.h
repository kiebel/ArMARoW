#pragma once

#include <armarow/common.h>

namespace armarow {
namespace mac {
    typedef uint16_t DeviceAddress;
    typedef uint16_t PANAddress;

    enum FrameTypeIEEE{
        beacon         = 0,
        data           = 1,
        acknowledgment = 2,
        command        = 3
    };

    struct FrameControlFieldIEEE {
        uint8_t frametype            : 3;
        bool    securityenabled      : 1;
        bool    framepending         : 1;
        bool    ackrequest           : 1;
        bool    intraPAN             : 1;
        uint8_t                      : 2; //reserved may need zeros
        uint8_t destaddressingmode   : 2;
        uint8_t                      : 2;
        uint8_t sourceaddressingmode : 2;

        void init() {
            frametype            = data;
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

        FrameHeaderMAC() {
            controlfield.init();
            sequencenumber = 0;
            source_adress  = 0;
            dest_adress    = 0;
            dest_pan       = 0; 
            source_pan     = 0;
        }

        /*! \brief Prints out the format of the frame header using the logging framework.*/
        void printFormat() {
            log::emit()
                << "SIZE OF FrameHeaderMAC:" << sizeof(FrameHeaderMAC) << log::endl
                << "SIZE OF FrameControlFieldIEEE: " <<  sizeof(FrameControlFieldIEEE) << log::endl
                << "SIZE OF sequencenumber: " << sizeof(sequencenumber) << log::endl << log::endl
                << "SIZE OF dest_pan: " << sizeof(dest_pan) << log::endl
                << "SIZE OF dest_adress: " << sizeof(dest_adress) << log::endl
                << "SIZE OF source_pan: " << sizeof(source_pan) << log::endl
                << "SIZE OF source_adress: " <<  sizeof(source_adress) << log::endl;
        }

        /*! \brief Prints out the content of a frame header using the logging framework.*/
        void printContent() {
            log::emit()
                << "FrameHeaderMAC:" << log::endl
                << "message_type: " << (int) controlfield.frametype << log::endl << log::endl
                << "ackrequest: " << (int) controlfield.ackrequest << log::endl << log::endl
                << "sequencenumber: " << (int) sequencenumber << log::endl << log::endl
                << "dest_pan: " << (int) dest_pan << log::endl
                << "dest_adress: " << (int) dest_adress << log::endl
                << "source_pan: " << (int) source_pan << log::endl
                << "source_adress: " <<  (int) source_adress << log::endl;
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
                for (uint8_t index=0; index < sizeof(payload); index++) {
                    payload[index]=0; //clean data (need to do this?)
                }

                size                  = 0;
            }

            explicit MessageFrameMAC(FrameTypeIEEE msgtyp, DeviceAddress source, DeviceAddress destination, uint8_t* dataBuffer, uint8_t dataSize) {
                if ( dataSize > sizeof(payload)){ 
                    log::emit()
                        << "FATAL ERROR: FAILED to create MessageFrameMAC object, since payload is to small"
                        << log::endl;
                    return;
                }
                header.controlfield.frametype = msgtyp;

                header.source_adress  = source;
                header.dest_adress    = destination;

                for(uint8_t index=0; index < dataSize; index++) {
                    payload[index] = dataBuffer[index];
                }
                size = dataSize;
            }

            void print() {
                log::emit()
                    << "FrameHeaderMAC:" << log::endl
                    << "message_type: " << (int) header.controlfield.frametype << log::endl << log::endl
                    << "ackrequest: " << (int) header.controlfield.ackrequest << log::endl << log::endl
                    << "sequencenumber: " << (int) header.sequencenumber << log::endl << log::endl
                    << "dest_pan: " << (int) header.dest_pan << log::endl
                    << "dest_adress: " << (int) header.dest_adress << log::endl
                    << "source_pan: " << (int) header.source_pan << log::endl
                    << "source_adress: " <<  (int) header.source_adress << log::endl;
            }

            void hexdump() { //FIXME is this method necessary NO says Karl
                uint8_t  loops = 0; //need to insert linebrakes
                uint8_t* pointer = (uint8_t*) this;

                log::emit()
                    << log::endl << "=== BEGIN HEX DUMP ===" << log::endl << log::endl
                    << log::hex;

                for (uint8_t index=0; index < sizeof(MessageFrameMAC); index++) { //FIXME this loop is used twice, maybe it should be moved to another method
                    log::emit() << (uint16_t) pointer[index] << ",";
                    if ( loops++ >= 20 ){
                        loops = 0;
                        log::emit() << log::endl;
                    }
                }
                log::emit() << log::endl << "=== END HEX DUMP ===" << log::endl << log::endl;
            }

#define MAC_LAYER_VERBOSE_OUTPUT true
            bool isValid() {
                if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                    log::emit() << "Validate MAC Frame..." << log::endl;
                }
                if ( size > sizeof(payload) ) {
                    log::emit() 
                        << "FATAL ERROR: Size of Payload to large! MAX Value: " << sizeof(payload)
                        << " Value of Frame: " << (int) size << log::endl;
                    return false;
                }
                //FIXME unnecessary functionality, should be moved to a method
                if ( header.controlfield.frametype == beacon ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT) {
                        log::emit() << "Beacon Message" << log::endl;
                    }
                } else if(header.controlfield.frametype == command) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        log::emit() << "MAC_command Message" << log::endl;
                    }
                } else if(header.controlfield.frametype == data) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        log::emit() << "Data Message" << log::endl;
                    }
                } else if ( header.controlfield.frametype == acknowledgment ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT ) {
                        log::emit() << "Acknowledgment Message" << log::endl;
                    }
                } else {
                    log::emit() << "FATAL ERROR: failed decoding MAC Message" << log::endl;
                    print();
                    return false;
                }
                if ( MAC_LAYER_VERBOSE_OUTPUT ) { //FIXME do we need this output at all?
                    log::emit() << "Success..." << log::endl;
                }
                return true;
            }
#undef MAC_LAYER_VERBOSE_OUTPUT
            /*! \brief Stores an object of arbitrary type in a message of the Medium Access Layer (MAC).
             *  \note The size of the object type has to be smaler or equal the available payload size!
             */
            template <class T>
            int storeObject(T& object) {
                static const bool k = ( sizeof(payload) >= sizeof(T) );
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
                static const bool k = ( sizeof(payload) >= sizeof(T) );
                ARMAROW_STATIC_ASSERT_ERROR(k,TYPE_TO_LARGE__DOES_NOT_FIT_IN_MESSAGE_PAYLOAD,(T));
                uint8_t* objectData = (uint8_t*) &object;
                for (uint8_t index = 0; index < sizeof(T); index++)
                    objectData[index] = payload[index];
                return 0; //FIXME what is a return value needed for?
            }

            platform::config::mob_t* getPhysicalLayerMessage() {
                size += sizeof(FrameHeaderMAC);
                return (platform::config::mob_t*) this; //FIXME use appropriated cast
            }

            /*! \brief Converts a message from the Physical Lyaer into a message of the Medium Access Layer (MAC).
             *
             *  \param message Physical Layer message
             *  \return a valid MessageFrameMAC upon successful decoding, otherwise NULL.
             */
            static MessageFrameMAC* transformPhysicalLayerMessageIntoMediumAccessLayerMessage( platform::config::mob_t& message) {
                bool decodingSuccessful = true;
                MessageFrameMAC* messageMAC = (MessageFrameMAC*) new(&message) MessageFrameMAC(message, decodingSuccessful);
                return ( ( decodingSuccessful ) ? messageMAC : (MessageFrameMAC*)0 ); //FIXME use reference or NULL
            }
        private:
            explicit MessageFrameMAC(platform::config::mob_t& physical_layer_message, bool& decoding_was_successful) {
                if ( physical_layer_message.size < sizeof(header) ) {
                    decoding_was_successful = false;
                    log::emit()
                        << "ERROR: MessageFrameMAC constructor: physical Message to short, to contain a FrameHeaderMAC!"
                        << log::endl << "Minimal Size: " << sizeof(header) << " size of current MAC_Frame: "
                        << (int) physical_layer_message.size << log::endl << log::endl;
                    print();
                    log::emit() << log::endl << log::endl << log::endl; //FIXME is this needed???
                    return;
                }

                size      = physical_layer_message.size - sizeof(FrameHeaderMAC);
                minfo.ed  = physical_layer_message.minfo.ed;
                minfo.lqi = physical_layer_message.minfo.lqi;

                decoding_was_successful = (decoding_was_successful) ? isValid() : decoding_was_successful;
                static const bool k = ( sizeof(platform::config::mob_t) == sizeof(MessageFrameMAC) );
                ARMAROW_STATIC_ASSERT_ERROR(k,MessageFrameMAC_AND_PHY_MESSAGE_HAVE_DIFFERENT_SIZES__MESSAGEDECODING_WILL_NOT_WORK,(int));
            }
    } __attribute__((packed));
};
};
