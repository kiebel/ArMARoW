


#ifndef __MAC_MESSAGE__
#define __MAC_MESSAGE__


/* === amarow includes ============================================================= */
//#include "platform-cfg.h"               // platform dependent software config

//#include "logConf.h"


//#define DeviceAddress uint32_t


#define MAX_NUMBER_OF_DATABYTES (101-sizeof(uint8_t)-sizeof(MAC_Header))


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

//typedef uint32_t DeviceAddress; 
typedef uint8_t DeviceAddress; 



//typedef rc_t::platform::config::mob_t  platform::config::mob_t;

//typedef armarow::PHY::rc_t::platform::config::mob_t platform::config::mob_t
//typedef platform::config::mob_t mob_t;

//typedef MAC_Message platform::config::mob_t;

void send(char* buffer,unsigned int buffersize);


enum MessageType{RTS,CTS,DATA,ACK};


struct MAC_Header{

	DeviceAddress send_adress;
	DeviceAddress receive_adress;
	MessageType messagetype;

	MAC_Header(){

	}

	MAC_Header(MessageType a_messagetype, DeviceAddress a_send_adress, DeviceAddress a_receive_adress ){

		send_adress=a_send_adress;
		receive_adress=a_receive_adress;
		messagetype=a_messagetype;

	}

	explicit MAC_Header(platform::config::mob_t physical_layer_message){
	

		//init header	


	}

} __attribute__((packed));


/*
struct MAC_Payload{


	char data[MAX_NUMBER_OF_DATABYTES]; 

	explicit MAC_Payload(){

	}

	explicit MAC_Payload(char* pointer_to_buffer, uint8_t size_of_databuffer){

		setPayloadNULL();

		if(MAX_NUMBER_OF_DATABYTES<size_of_databuffer){

			::logging::log::emit() << "ERROR: MAC_Payload: number of bytes in databuffer does not fit in MAC_Payload! -> decrease size of databuffer or transmit multiple MAC_Messages!" << ::logging::log::endl;
			return;

		}

		//payloadsize=size_of_databuffer;

		for(int i=0;i<size_of_databuffer;i++){

			data[i]=pointer_to_buffer[i];

		}


	}


	explicit MAC_Payload(platform::config::mob_t& physical_layer_message) {

		//init payload


	}


	void setPayloadNULL(){

		for(unsigned int i=0;i<MAX_NUMBER_OF_DATABYTES;i++){

			data[i]='\0'; //set memory to zero
			//::logging::log::emit() << i << ::logging::log::endl;
		}

	}

} __attribute__((packed));
*/


struct MAC_Message{
	
	uint8_t size; //just there to be there (placeholder [Platzhalter] for uint_8 in platform::config::mob_t object, so that the header starts exactly at the adress of the payload of the platform::config::mob_t object)
	MAC_Header header;
	char payload[MAX_NUMBER_OF_DATABYTES]; 

	//MAC_Payload payload;

  //this constructor have to be used with the palcement new operator, hence it may not called directly
  private:

	explicit MAC_Message(platform::config::mob_t& physical_layer_message){

		//init header	
		if(physical_layer_message.size<sizeof(header)-sizeof(uint8_t)){

			::logging::log::emit() << "ERROR: MAC_Message constructor: physical Message to short, to contain a MAC_Header!" << ::logging::log::endl;
			::logging::log::emit() << "Minimal Size: " << sizeof(header)-sizeof(uint8_t) << " size of current MAC_Frame: " << physical_layer_message.size << ::logging::log::endl;


			::logging::log::emit() << ::logging::log::endl;

			print();
	
			::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << ::logging::log::endl;

		}

		//set the correct size value for this layer (size refers always only to the payload, the Headerlength is not included. Since the MAC_Header is part of the Payload of the physical Message, its length has to be subtracted from the size value of the physical message, so that value specifies now the Number of Bytes in the MAC_Payload)
		size = physical_layer_message.size - sizeof(MAC_Header);

		if(header.messagetype==RTS){

			::logging::log::emit() << "RTS Message" << ::logging::log::endl;

		}else if(header.messagetype==CTS){

			::logging::log::emit() << "CTS Message" << ::logging::log::endl;

		}else if(header.messagetype==DATA){

			::logging::log::emit() << "DATA Message" << ::logging::log::endl;

		}else if(header.messagetype==ACK){

			::logging::log::emit() << "ACK Message" << ::logging::log::endl;

		}else{
			::logging::log::emit() << "FATAL ERROR: failed decoding MAC Message" << ::logging::log::endl;
		}

	

	}


	public:

	explicit MAC_Message(){

		DeviceAddress send_adress=25;
		DeviceAddress receive_adress=38;

		//MAC_Message(DATA, send_adress, receive_adress, (char*) 0, 0);

		new (&header) MAC_Header( DATA, send_adress, receive_adress);
		setPayloadNULL();
		size=0;

	}



	explicit MAC_Message(MessageType msgtyp, DeviceAddress send_adress, DeviceAddress receive_adress, char* pointer_to_databuffer, uint8_t size_of_databuffer){

		if(size_of_databuffer>MAX_NUMBER_OF_DATABYTES){

			::logging::log::emit() << "FATAL ERROR: FAILED building MAC_Message Object because the size of the databuffer is greater than the MAX_NUMBER_OF_DATABYTES" << ::logging::log::endl;
			return;

		}

		new (&header) MAC_Header( msgtyp, send_adress, receive_adress);
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
	::logging::log::emit() << "sender_adress: " <<  (int) header.send_adress << ::logging::log::endl;
	::logging::log::emit() << "receiver_adress: " << (int) header.receive_adress << ::logging::log::endl;
	::logging::log::emit() << "message_type: " << (int) header.messagetype << ::logging::log::endl;

	::logging::log::emit() << "MAC_PAYLOAD:" << ::logging::log::endl;
	::logging::log::emit() << "size of Payload: " << (int) size << ::logging::log::endl;

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



	platform::config::mob_t* getPhysical_Layer_Message(){
		size += sizeof(MAC_Header); //the size value now have to correspond to the payload of the physical layer Message, and the the MAC_Header of the MAC_Message is part of that payload 
		return (platform::config::mob_t*) this;
	}

	static MAC_Message* create_MAC_Message_from_Physical_Message(platform::config::mob_t& physical_layer_message){

		return (MAC_Message*) new(&physical_layer_message) MAC_Message(physical_layer_message);


	}


} __attribute__((packed));




void send_test(char* buffer,unsigned int buffersize){

//string received_data;

//int size=buffersize;
int offset=0;

while(buffersize>0){


	//int counterlimit=min(buffersize,MAX_NUMBER_OF_DATABYTES);

	int counterlimit;
	if(buffersize<MAX_NUMBER_OF_DATABYTES) counterlimit=buffersize; else counterlimit=MAX_NUMBER_OF_DATABYTES;


	//for(int i=0;i<counterlimit;i++){

		MAC_Message mac_message1(DATA,25,38,&buffer[offset],counterlimit);
		::logging::log::emit() << ::logging::log::endl << ::logging::log::endl << "Sending MAC_Message... " << ::logging::log::endl;
		mac_message1.print();
		

		//string s(mac_message1.payload.databuffer);
		//received_data += s;
		

	//}

	offset += counterlimit;
	buffersize -= MAX_NUMBER_OF_DATABYTES;

}

	//::logging::log::emit() << received_data << ::logging::log::endl;




}






};  //namespace MAC end

};  //namespace armarow end

#endif

