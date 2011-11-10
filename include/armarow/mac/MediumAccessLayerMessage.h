#pragma once

//FIXME
#define MAX_NUMBER_OF_DATABYTES (platform::config::rc_t::info::frame - sizeof(MAC_Header))

namespace armarow {
namespace mac {
    typedef uint16_t DeviceAddress; 
    typedef uint16_t PANAddress; 

    enum IEEE_Frametype {
        Beacon         = 0,
        Data           = 1,
        Acknowledgment = 2,
        MAC_command    = 3
    };

    struct IEEE_Frame_Control_Field{
        unsigned int frametype            : 3;
        unsigned int securityenabled      : 1;
        unsigned int framepending         : 1;
        unsigned int ackrequest           : 1;
        unsigned int intraPAN             : 1;
        unsigned int reserved             : 2;
        unsigned int destaddressingmode   : 2;
        unsigned int reserved2            : 2;
        unsigned int sourceaddressingmode : 2;

        void init() {
            frametype            = Data;i //FIXME
            securityenabled      = 0;
            framepending         = 0;
            ackrequest           = 0;
            intraPAN             = 0;
            reserved             = 0;
            destaddressingmode   = 2;
            reserved2            = 0;
            sourceaddressingmode = 2;
        }
    } __attribute__((packed));

    void send(char* buffer,unsigned int buffersize); //FIXME

    enum MessageType{ RTS, CTS, DATA, ACK }; 
    
    /*!  \brief This class encapsulates all Mac Header concerning data in exactly the order specified by IEEE 802.15.4 . It is an important part of the message decoding mechanism.*/
    struct MAC_Header {
        IEEE_Frame_Control_Field controlfield;
        uint8_t sequencenumber;
        PANAddress dest_pan;
        DeviceAddress dest_adress;
        PANAddress source_pan;
        DeviceAddress source_adress;

        /*! \brief This is a convinience constructor, but since we initialize the whole Mac Header on a per variable basis, it is not used anymore.
         *
         *  \param a_messagetype an IEEE message type
         *  \param a_source_adress
         *  \param a_dest_adress
         */
        MAC_Header(IEEE_Frametype a_messagetype, DeviceAddress a_source_adress, DeviceAddress a_dest_adress) {
            controlfield.init();
            controlfield.frametype=a_messagetype;
            source_adress = a_source_adress;
            dest_adress   = a_dest_adress;
            sequencenumber = 0;
            dest_pan       = 0; 
            source_pan     = 0;
        }

        /*! \brief This method is used for debug purposes. You can identify message decoding problems with it.*/
        void printFrameFormat(){
            ::logging::log::emit() << "SIZE OF MAC_HEADER:" << sizeof(MAC_Header) << ::logging::log::endl;
            ::logging::log::emit() << "SIZE OF IEEE_Frame_Control_Field: " <<  sizeof(IEEE_Frame_Control_Field) << ::logging::log::endl;
            ::logging::log::emit() << "SIZE OF sequencenumber: " << sizeof(sequencenumber) << ::logging::log::endl << ::logging::log::endl;
            ::logging::log::emit() << "SIZE OF dest_pan: " << sizeof(dest_pan) << ::logging::log::endl;
            ::logging::log::emit() << "SIZE OF dest_adress: " << sizeof(dest_adress) << ::logging::log::endl;
            ::logging::log::emit() << "SIZE OF source_pan: " << sizeof(source_pan) << ::logging::log::endl;
            ::logging::log::emit() << "SIZE OF source_adress: " <<  sizeof(source_adress) << ::logging::log::endl;
        }

        /*! \brief This method is used for debug purposes. You can identify message decoding problems with it.*/
        void print(){
            ::logging::log::emit() << "MAC_HEADER:" << ::logging::log::endl;
            ::logging::log::emit() << "message_type: " << (int) controlfield.frametype << ::logging::log::endl << ::logging::log::endl;
            ::logging::log::emit() << "ackrequest: " << (int) controlfield.ackrequest << ::logging::log::endl << ::logging::log::endl;
            ::logging::log::emit() << "sequencenumber: " << (int) sequencenumber << ::logging::log::endl << ::logging::log::endl;
            ::logging::log::emit() << "dest_pan: " << (int) dest_pan << ::logging::log::endl;
            ::logging::log::emit() << "dest_adress: " << (int) dest_adress << ::logging::log::endl;
            ::logging::log::emit() << "source_pan: " << (int) source_pan << ::logging::log::endl;
            ::logging::log::emit() << "source_adress: " <<  (int) source_adress << ::logging::log::endl;
        }

        /*! \brief This method is used with the placement new operator, to lay over the Mac Header over the Physical Layer message payload to decode the message.*/
        explicit MAC_Header(platform::config::mob_t physical_layer_message) {}
    } __attribute__((packed));


    /*! \brief This class is the abstraction of a MAC_Message. It is used to decode Physical Layer Messages into Mac Layer Messages and the other way around. It uses the placement new operator to implement its functionality.*/
    struct MAC_Message {
        uint8_t size;
        MAC_Header header;
        char payload[MAX_NUMBER_OF_DATABYTES]; 
        struct {
            uint8_t lqi;  /*!< last measured LQI value*/
            uint8_t ed;
            bool    crc  : 1;
        } minfo;

        private:

            /*! \brief This constructor is responsible for the decoding of the Physical Layer message into a Mac message. This constructor have to be used with the placement new operator, hence it may not be called directly.*/
            explicit MAC_Message(platform::config::mob_t& physical_layer_message, bool& decoding_was_successful) {
                if ( physical_layer_message.size < sizeof(header) ) {
                    ::logging::log::emit() << "ERROR: MAC_Message constructor: physical Message to short, to contain a MAC_Header!" << ::logging::log::endl;
                    ::logging::log::emit() << "Minimal Size: " << sizeof(header) << " size of current MAC_Frame: " << (int) physical_layer_message.size << ::logging::log::endl;
                    decoding_was_successful = false;
                    ::logging::log::emit() << ::logging::log::endl;
                    print();
                    ::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << ::logging::log::endl;
                    return;
                }

                size = physical_layer_message.size - sizeof(MAC_Header);
                this->minfo.ed=physical_layer_message.minfo.ed;
                this->minfo.lqi=physical_layer_message.minfo.lqi;

                if ( decoding_was_successful ) decoding_was_successful = isValid();
                static const bool k=((sizeof(platform::config::mob_t) == sizeof(MAC_Message))?true:false);
                ARMAROW_STATIC_ASSERT_ERROR(k,MAC_MESSAGE_AND_PHY_MESSAGE_HAVE_DIFFERENT_SIZES__MESSAGEDECODING_WILL_NOT_WORK,(int));
            }
        public:
            /*! \brief This is the default constructor. It sets default values to all variables of the Mac Header. We use the principle: Everything works with the default configuration. Hence you only have to init variables yourself, that need non default values. This makes your code more readable and simplifies developement. (And it avoids errors as well)*/
            explicit MAC_Message() {
                DeviceAddress source_adress = 25;
                DeviceAddress dest_adress   = 38;
	
                new (&header) MAC_Header( Data, source_adress, dest_adress); //FIXME

                header.sequencenumber = 0;
                header.dest_pan       = 0; 
                header.source_pan     = 0;
                header.controlfield.init();
                setPayloadNULL();
                size                  = 0;
	        }

            /*! \brief This is a convinience constructor, but since we initialize the whole Mac Header on a per variable basis, it is not used anymore.
             *
             *  \param msgtyp a message type defined in IEEE
             *  \param source_adress local node adress
             *  \param dest_adress destination mac adress
             *  \param pointer_to_databuffer points to a buffer that contains the data to transmit
             *  \param size_of_databuffer 
             */
            explicit MAC_Message(IEEE_Frametype msgtyp, DeviceAddress source_adress, DeviceAddress dest_adress, char* pointer_to_databuffer, uint8_t size_of_databuffer) {
                if ( size_of_databuffer > MAX_NUMBER_OF_DATABYTES ) {
                    ::logging::log::emit() << "FATAL ERROR: FAILED building MAC_Message Object because the size of the databuffer is greater than the MAX_NUMBER_OF_DATABYTES" << ::logging::log::endl;
                    return;
                }
                new (&header) MAC_Header( msgtyp, source_adress, dest_adress); //FIXME
                setPayloadNULL();
                if ( MAX_NUMBER_OF_DATABYTES < size_of_databuffer ) {
                    ::logging::log::emit() << "ERROR: MAC_Payload: number of bytes in databuffer does not fit in MAC_Payload! -> decrease size of databuffer or transmit multiple MAC_Messages!" << ::logging::log::endl;
                    return;
                }

                for(int i=0; i < size_of_databuffer; i++) {
                    payload[i]=pointer_to_databuffer[i];
                }
                size = size_of_databuffer;
            }

            /*! \brief This methods fill the Payload area of the mac message with zero bytes and sets the size to 0.*/
            void setPayloadNULL() {
                for (unsigned int i=0; i < MAX_NUMBER_OF_DATABYTES; i++) {
                    payload[i]='\0'; //FIXME
                }
            }

            /*! \brief Prints the content of the Mac Message. Helpful for Debuggging.*/
            void print() {
                ::logging::log::emit() << "MAC_HEADER:" << ::logging::log::endl;
                ::logging::log::emit() << "message_type: " << (int) header.controlfield.frametype << ::logging::log::endl << ::logging::log::endl;
                ::logging::log::emit() << "ackrequest: " << (int) header.controlfield.ackrequest << ::logging::log::endl << ::logging::log::endl;
                ::logging::log::emit() << "sequencenumber: " << (int) header.sequencenumber << ::logging::log::endl << ::logging::log::endl;
                ::logging::log::emit() << "dest_pan: " << (int) header.dest_pan << ::logging::log::endl;
                ::logging::log::emit() << "dest_adress: " << (int) header.dest_adress << ::logging::log::endl;
                ::logging::log::emit() << "source_pan: " << (int) header.source_pan << ::logging::log::endl;
                ::logging::log::emit() << "source_adress: " <<  (int) header.source_adress << ::logging::log::endl;
            }

            /*! \brief Prints the content of the Mac Message in Hexcode. Helpful for Debuggging.*/
            void hexdump() {
                ::logging::log::emit() << ::logging::log::endl << "=== BEGIN HEX DUMP ===" << ::logging::log::endl << ::logging::log::endl;	
                ::logging::log::emit() << ::logging::log::hex;

                unsigned char* pointer = (unsigned char*) this;
                unsigned int loopcounter = 0;

                for (unsigned int i=0; i < sizeof(MAC_Message); i++) {
                    ::logging::log::emit() << (uint16_t) pointer[i] << ",";
                    if ( loopcounter >= 20 ) {
                        loopcounter=0;
                        ::logging::log::emit() << ::logging::log::endl;
                    }	
                }
                ::logging::log::emit() << ::logging::log::endl;
                ::logging::log::emit() << ::logging::log::endl;
                ::logging::log::emit() << ::logging::log::dec;


                loopcounter = 0;
                for (unsigned int i=0; i < sizeof(MAC_Message); i++) {
                    ::logging::log::emit() << (uint16_t) pointer[i] << ",";
                    if ( loopcounter >= 20 ) {
                        loopcounter = 0;
                        ::logging::log::emit() << ::logging::log::endl;
                    }	
                }
                ::logging::log::emit() << ::logging::log::endl << "=== END HEX DUMP ===" << ::logging::log::endl << ::logging::log::endl;
            }

            /*! \brief This Method implements the validating mechanism.*/
            bool isValid() {
                if ( MAC_LAYER_VERBOSE_OUTPUT )
                    ::logging::log::emit() << "Validate MAC Frame..." << ::logging::log::endl;
                if ( size > MAX_NUMBER_OF_DATABYTES ) {
                    ::logging::log::emit() << "FATAL ERROR: Size of Payload to large! MAX Value: " << MAX_NUMBER_OF_DATABYTES << " Value of Frame: " << (int) size << ::logging::log::endl;
                    return false;
                }
                if ( header.controlfield.frametype == Beacon ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT) 
                        ::logging::log::emit() << "Beacon Message" << ::logging::log::endl;
                } else if(header.controlfield.frametype == MAC_command) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT )
                        ::logging::log::emit() << "MAC_command Message" << ::logging::log::endl;
                } else if(header.controlfield.frametype == Data) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT )
                        ::logging::log::emit() << "Data Message" << ::logging::log::endl;
                } else if ( header.controlfield.frametype == Acknowledgment ) {
                    if ( MAC_LAYER_VERBOSE_OUTPUT )
                        ::logging::log::emit() << "Acknowledgment Message" << ::logging::log::endl;
                } else {
                    ::logging::log::emit() << "FATAL ERROR: failed decoding MAC Message" << ::logging::log::endl;
                    print();
                    return false;
                }
                if ( MAC_LAYER_VERBOSE_OUTPUT )
                    ::logging::log::emit() << "Success..." << ::logging::log::endl;
                return true;
            }

	
            //FIXME
            /*! If the Type of the passed object is to large to fit into the payload, then the compiler will terminate with a compile time static assertion error.  \brief Stores an arbitrary type with size less than the maximal payload size in the message payload. */
            template <class T>
            int store_object_in_payload(T& obj) {
                static const bool k = ( MAX_NUMBER_OF_DATABYTES >= sizeof(T) ) ? true : false;
                ARMAROW_STATIC_ASSERT_ERROR(k,TYPE_TO_LARGE__DOES_NOT_FIT_IN_MESSAGE_PAYLOAD,(T));
                char* tmp = (char*) &obj;
                for (unsigned int i=0; i < sizeof(T); i++)
                    payload[i]=tmp[i];
                size=sizeof(T);
                return 0;
            }

            /*! \brief Loads an object of arbitrary type from the message payload.*/
            template <class T>
            int get_object_from_payload(T& obj) {
                static const bool k = ( MAX_NUMBER_OF_DATABYTES >= sizeof(T) ) ? true : false;
                ARMAROW_STATIC_ASSERT_ERROR(k,TYPE_TO_LARGE__DOES_NOT_FIT_IN_MESSAGE_PAYLOAD,(T));
                char* tmp = (char*) &obj;
                for (unsigned int i=0; i < sizeof(T); i++)
                    tmp[i] = payload[i];
                return 0;
            }
	

            platform::config::mob_t* getPhysical_Layer_Message() {
                size += sizeof(MAC_Header);
                return (platform::config::mob_t*) this;
            }

            /*! \brief This method converts a Physical Layer message to a Mac Layer message.
             * 
             *  This Method is intended to return a pointer to a decoded MAC_Message. Therefore we need a Physical Layer message as parameter. If we couldn't decode the Physical Layer message, a zero pointer is returned.
             *  \param physical_layer_message the message shall be decoded
             *  \return by successful decoding, it points to a valid MAC_Message object, otherwise it is a zero pointer
             */
            static MAC_Message* create_MAC_Message_from_Physical_Message(platform::config::mob_t& physical_layer_message) {
                bool decoding_was_successful = true; //FIXME
                MAC_Message* tmp = (MAC_Message*) new(&physical_layer_message) MAC_Message(physical_layer_message,decoding_was_successful); //FIXME

                return ( ( decoding_was_successful ) ? tmp : (MAC_Message*)0 ); //FIXME
            }
        }
    } __attribute__((packed));
};
};
