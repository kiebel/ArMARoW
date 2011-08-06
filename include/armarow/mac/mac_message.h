


#ifndef __MAC_MESSAGE__
#define __MAC_MESSAGE__

#define LOGGING_DISABLE

//TODO: REPLACE 101 with the a value directly coming from the physical layer
//#define MAX_NUMBER_OF_DATABYTES (101-sizeof(uint8_t)-sizeof(MAC_Header))

//armarow::PHY::aMaxPHYPacketSize is the brutto payload. Since we can activate CRC, we could get additional overhead, that is considered in rc_t::info::payload
//rc_t::info::payload = available payload considering frame size and overhead
#define MAX_NUMBER_OF_DATABYTES (platform::config::rc_t::info::payload - sizeof(MAC_Header))



/*from armarow start*/ 

/*
struct info{

	static int frame=100;

}
*/
                /*! \brief  definition of a layer specific message object*/
//                typedef struct {
//                    uint8_t size;                   /*!< number of bytes*/
//                    uint8_t payload[127];   /*!< frame data     */
//                } platform::config::mob_t;



/*from armarow end*/

namespace armarow{

namespace MAC{


typedef uint16_t DeviceAddress; 
typedef uint16_t PANAddress; 


/*see IEEE 802.15.4 page 112 for more details*/


enum IEEE_Frametype{Beacon=0,Data=1,Acknowledgment=2,MAC_command=3};

struct IEEE_Frame_Control_Field{


   void init(){

	//TODO: aus Standard "richtige" Werte raussuchen

	frametype = Data;
        securityenabled = 0; //Bit 3
        framepending = 0; //Bit 4   //ist das eine fragmentierte Nachricht
        ackrequest = 0; //Bit 5     //wollen wir Aknolegment haben?
        intraPAN = 0; //Bit 6
        reserved = 0; //Bits 7-9
        destaddressingmode = 2; //Bits 10-11 
        reserved2 = 0; //Bits 12-13
        sourceaddressingmode = 2; //Bits 14-15


   }

	IEEE_Frame_Control_Field(){init();}

   //Bits: 0–2 3 4 5 6 7–9 10–11 12–13 14–15

   unsigned int frametype : 3; //Bits: 0–2
   unsigned int securityenabled : 1; //Bit 3
   unsigned int framepending : 1; //Bit 4
   unsigned int ackrequest : 1; //Bit 5
   unsigned int intraPAN : 1; //Bit 6
   unsigned int reserved : 2; //Bits 7-9
   unsigned int destaddressingmode : 2; //Bits 10-11 
   unsigned int reserved2 : 2; //Bits 12-13
   unsigned int sourceaddressingmode : 2; //Bits 14-15

} __attribute__((packed));





void send(char* buffer,unsigned int buffersize);

enum MessageType{RTS,CTS,DATA,ACK}; 
//enum MessageType{RTS=256,CTS=257,DATA=258,ACK=259}; //we have the compiler option short enums enabled, thats we we only get 1 byte size instead of 2 byte -> but the IEEE standard wants it to be 16 Bit, so we enter values that doesn't fit into one byte, so that the compiler have to keep 16 bit values 


struct MAC_Header{

	//MessageType messagetype;   //16 Bit
	IEEE_Frame_Control_Field controlfield;  //16 Bit
	uint8_t sequencenumber;    //8 Bit
	PANAddress dest_pan;       //16 Bit
	DeviceAddress dest_adress; //16 Bit
	PANAddress source_pan;     //16 Bit
	DeviceAddress source_adress; //16 Bit

	MAC_Header(){



	}

	MAC_Header(IEEE_Frametype a_messagetype, DeviceAddress a_source_adress, DeviceAddress a_dest_adress){

		controlfield.init();

		source_adress=a_source_adress;
		dest_adress=a_dest_adress;
		//messagetype=a_messagetype;

		controlfield.frametype=a_messagetype;

		sequencenumber=0; //get_global_sequence_number();

		//we doesn't support this yet, so we just set this part of the header null
		dest_pan=0; 
		source_pan=0;

	}



	void printFrameFormat(){

	::logging::log::emit() << "SIZE OF MAC_HEADER:" << sizeof(MAC_Header) << ::logging::log::endl;
	::logging::log::emit() << "SIZE OF IEEE_Frame_Control_Field: " <<  sizeof(IEEE_Frame_Control_Field) << ::logging::log::endl;
	//::logging::log::emit() << "SIZE OF message_type: " << sizeof(messagetype) << ::logging::log::endl << ::logging::log::endl;
	::logging::log::emit() << "SIZE OF sequencenumber: " << sizeof(sequencenumber) << ::logging::log::endl << ::logging::log::endl;
	::logging::log::emit() << "SIZE OF dest_pan: " << sizeof(dest_pan) << ::logging::log::endl;
	::logging::log::emit() << "SIZE OF dest_adress: " << sizeof(dest_adress) << ::logging::log::endl;
	::logging::log::emit() << "SIZE OF source_pan: " << sizeof(source_pan) << ::logging::log::endl;
	::logging::log::emit() << "SIZE OF source_adress: " <<  sizeof(source_adress) << ::logging::log::endl;

	//::logging::log::emit() << "===== DEBUG: size of message components =====" << ::logging::log::endl;
		
	//::logging::log::emit() << "SIZE OF IEEE_Frame_Control_Field: " <<  sizeof(IEEE_Frame_Control_Field) << ::logging::log::endl;


	}


	explicit MAC_Header(platform::config::mob_t physical_layer_message){
	

		//init header	


	}

} __attribute__((packed));



struct MAC_Message{
	
	uint8_t size; //just there to be there (placeholder [Platzhalter] for uint_8 in platform::config::mob_t object, so that the header starts exactly at the adress of the payload of the platform::config::mob_t object)
	MAC_Header header;
	char payload[MAX_NUMBER_OF_DATABYTES]; 

	//MAC_Payload payload;

  //this constructor have to be used with the placement new operator, hence it may not called directly
  private:

	explicit MAC_Message(platform::config::mob_t& physical_layer_message, bool& decoding_was_successful){

		//init header	
		if(physical_layer_message.size<sizeof(header)){

			::logging::log::emit() << "ERROR: MAC_Message constructor: physical Message to short, to contain a MAC_Header!" << ::logging::log::endl;
			::logging::log::emit() << "Minimal Size: " << sizeof(header) << " size of current MAC_Frame: " << physical_layer_message.size << ::logging::log::endl;
			decoding_was_successful=false;

			::logging::log::emit() << ::logging::log::endl;

			print();
	
			::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << ::logging::log::endl;

			return;

		}

		//set the correct size value for this layer (size refers always only to the payload, the Headerlength is not included. Since the MAC_Header is part of the Payload of the physical Message, its length has to be subtracted from the size value of the physical message, so that value specifies now the Number of Bytes in the MAC_Payload)
		size = physical_layer_message.size - sizeof(MAC_Header);


		//if we already know, that the decoding has failed, we have no further need for confirmation
		if(decoding_was_successful) decoding_was_successful = isValid();




	}


	public:



	explicit MAC_Message(){

		//set default values
		DeviceAddress source_adress=25;
		DeviceAddress dest_adress=38;

	
		new (&header) MAC_Header( Data, source_adress, dest_adress);

		header.sequencenumber=0; //get_global_sequence_number();

		//we doesn't support this yet, so we just set this part of the header null
		header.dest_pan=0; 
		header.source_pan=0;

		header.controlfield.init();

		setPayloadNULL();
		size=0;

	}



	explicit MAC_Message(IEEE_Frametype msgtyp, DeviceAddress source_adress, DeviceAddress dest_adress, char* pointer_to_databuffer, uint8_t size_of_databuffer){

		if(size_of_databuffer>MAX_NUMBER_OF_DATABYTES){

			::logging::log::emit() << "FATAL ERROR: FAILED building MAC_Message Object because the size of the databuffer is greater than the MAX_NUMBER_OF_DATABYTES" << ::logging::log::endl;
			return;

		}

		new (&header) MAC_Header( msgtyp, source_adress, dest_adress);
		//new (&payload) MAC_Payload(pointer_to_databuffer, size_of_databuffer);

		setPayloadNULL();

		if(MAX_NUMBER_OF_DATABYTES<size_of_databuffer){

			::logging::log::emit() << "ERROR: MAC_Payload: number of bytes in databuffer does not fit in MAC_Payload! -> decrease size of databuffer or transmit multiple MAC_Messages!" << ::logging::log::endl;
			return;

		}

		//payloadsize=size_of_databuffer;

		for(int i=0;i<size_of_databuffer;i++){

			payload[i]=pointer_to_databuffer[i];

		}

		//size=sizeof(MAC_Header)+sizeof(uint8_t)+size_of_databuffer;	
		//size=sizeof(MAC_Header)+size_of_databuffer;
		size=size_of_databuffer;
		//uint8_t 
	}


	void setPayloadNULL(){

		for(unsigned int i=0;i<MAX_NUMBER_OF_DATABYTES;i++){

			payload[i]='\0'; //set memory to zero
			//::logging::log::emit() << i << ::logging::log::endl;
		}

	}


	void print(){


	::logging::log::emit() << "MAC_HEADER:" << ::logging::log::endl;
	::logging::log::emit() << "message_type: " << (int) header.controlfield.frametype << ::logging::log::endl << ::logging::log::endl;
	::logging::log::emit() << "ackrequest: " << (int) header.controlfield.ackrequest << ::logging::log::endl << ::logging::log::endl;
	::logging::log::emit() << "sequencenumber: " << (int) header.sequencenumber << ::logging::log::endl << ::logging::log::endl;
	::logging::log::emit() << "dest_pan: " << (int) header.dest_pan << ::logging::log::endl;
	::logging::log::emit() << "dest_adress: " << (int) header.dest_adress << ::logging::log::endl;
	::logging::log::emit() << "source_pan: " << (int) header.source_pan << ::logging::log::endl;
	::logging::log::emit() << "source_adress: " <<  (int) header.source_adress << ::logging::log::endl;


		if (size > 0){
			::logging::log::emit() << "content of Payload: ";
			for(uint8_t i=0;i<size;i++)
			  ::logging::log::emit() << payload[i];
			::logging::log::emit() << ::logging::log::endl;	

		}

	}


	void hexdump(){


		::logging::log::emit() << ::logging::log::endl << "=== BEGIN HEX DUMP ===" << ::logging::log::endl << ::logging::log::endl;	


		::logging::log::emit() << ::logging::log::hex;

		unsigned char* pointer = (unsigned char*) this;

		unsigned int loopcounter = 0;

		for(unsigned int i=0;i<sizeof(MAC_Message);i++){
						
			::logging::log::emit() << (uint16_t) pointer[i] << ",";
			if(loopcounter>=20){

				loopcounter=0;
				::logging::log::emit() << ::logging::log::endl;

			}	

		}


		::logging::log::emit() << ::logging::log::endl;

		::logging::log::emit() << ::logging::log::endl;

		::logging::log::emit() << ::logging::log::dec;


		//===== test =====
		loopcounter=0;
		for(unsigned int i=0;i<sizeof(MAC_Message);i++){
						
			::logging::log::emit() << (uint16_t) pointer[i] << ",";
			if(loopcounter>=20){

				loopcounter=0;
				::logging::log::emit() << ::logging::log::endl;

			}	

		}


		::logging::log::emit() << ::logging::log::endl << "=== END HEX DUMP ===" << ::logging::log::endl << ::logging::log::endl;


	}




	//validating mechanism
	bool isValid(){

		if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "Validate MAC Frame..." << ::logging::log::endl;

		if(size>MAX_NUMBER_OF_DATABYTES){

			::logging::log::emit() << "FATAL ERROR: Size of Payload to large! MAX Value: " << MAX_NUMBER_OF_DATABYTES << " Value of Frame: " << (int) size << ::logging::log::endl;
			return false;

		}
                //valid values are: Beacon,Data,Acknowledgment,MAC_command

		if(header.controlfield.frametype==Beacon){

			if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "Beacon Message" << ::logging::log::endl;

		}else if(header.controlfield.frametype==MAC_command){

			if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "MAC_command Message" << ::logging::log::endl;

		}else if(header.controlfield.frametype==Data){

			if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "Data Message" << ::logging::log::endl;

		}else if(header.controlfield.frametype==Acknowledgment){

			if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "Acknowledgment Message" << ::logging::log::endl;

		}else{
			::logging::log::emit() << "FATAL ERROR: failed decoding MAC Message" << ::logging::log::endl;
			return false;
		}

		if(MAC_LAYER_VERBOSE_OUTPUT) ::logging::log::emit() << "Success..." << ::logging::log::endl;

		return true;
	}







	platform::config::mob_t* getPhysical_Layer_Message(){
		size += sizeof(MAC_Header); //the size value now have to correspond to the payload of the physical layer Message, and the the MAC_Header of the MAC_Message is part of that payload 
		return (platform::config::mob_t*) this;
	}

	static MAC_Message* create_MAC_Message_from_Physical_Message(platform::config::mob_t& physical_layer_message){

		bool decoding_was_successful=true;

		MAC_Message* tmp = (MAC_Message*) new(&physical_layer_message) MAC_Message(physical_layer_message,decoding_was_successful);

		if(decoding_was_successful){
			return tmp;
		}else{
			return (MAC_Message*) 0;
		}

	}


} __attribute__((packed));




void send_test(char* buffer,unsigned int buffersize){

int offset=0;

while(buffersize>0){

	int counterlimit;
	if(buffersize<MAX_NUMBER_OF_DATABYTES) counterlimit=buffersize; else counterlimit=MAX_NUMBER_OF_DATABYTES;



		MAC_Message mac_message1(Data,25,38,&buffer[offset],counterlimit);
		::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << "Sending MAC_Message... " << ::logging::log::endl;
		mac_message1.print();


	offset += counterlimit;
	buffersize -= MAX_NUMBER_OF_DATABYTES;

}



}






};  //namespace MAC end

};  //namespace armarow end

#endif

