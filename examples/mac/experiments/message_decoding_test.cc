

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdint.h>

using namespace std;


#define DeviceAddress int
//#define uint16_t short

//#define MAX_NUMBER_OF_DATABYTES 127

//#define cout log::emit()
//#define endl log::endl


#define MAX_NUMBER_OF_DATABYTES (128-sizeof(uint8_t)-sizeof(MAC_Header))
//böser hack!!!
//#define payload.data data


//#define uint8_t char

/*from armarow start*/ 

/*
struct info{

	static int frame=100;

}
*/
                /*! \brief  definition of a layer specific message object*/
                typedef struct {
                    uint8_t size;                   /*!< number of bytes*/
                    uint8_t payload[127];   /*!< frame data     */
                } mob_t;



/*from armarow end*/



namespace MAC{

void send(char* buffer,int buffersize);

struct A{

 int a[2];

 void output(){
 	cout << "A " << a[0] << "," << a[1] << endl;
 }


} __attribute__((packed));

//sorgt dafür das compiler nix an der Speicherstruktur ändert bei Optimierung


struct B{

 int b;

 //wird nicht automatisch verwendet beim umcasten
 explicit B(A& a){

	b=a.a[1];
	
 }

 void output(){
 	cout << "B " << b << endl;
 }

} __attribute__((packed));


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

	explicit MAC_Header(mob_t physical_layer_message){
	

		//init header	


	}

} __attribute__((packed));



struct MAC_Payload{


	char data[MAX_NUMBER_OF_DATABYTES]; 

	explicit MAC_Payload(){


		
		for(int i=0;i<MAX_NUMBER_OF_DATABYTES;i++){

			data[i]='\0'; //set memory to zero
			//cout << i << endl;
		}

		

	}

	explicit MAC_Payload(char* pointer_to_buffer, uint8_t size_of_databuffer){

		MAC_Payload();

		if(MAX_NUMBER_OF_DATABYTES<size_of_databuffer){

			cout << "ERROR: MAC_Payload: number of bytes in databuffer does not fit in MAC_Payload! -> decrease size of databuffer or transmit multiple MAC_Messages!" << endl;
			return;

		}

		//payloadsize=size_of_databuffer;

		for(int i=0;i<size_of_databuffer;i++){

			data[i]=pointer_to_buffer[i];

		}


	}


	explicit MAC_Payload(mob_t& physical_layer_message) {

		//init payload


	}

} __attribute__((packed));


/**
struct RTS : public MAC_Payload{



}__attribute__((packed));


struct CTS : public MAC_Payload{



}__attribute__((packed));
**/


struct MAC_Message{
	
	uint8_t size; //just there to be there (placeholder [Platzhalter] for uint_8 in mob_t object, so that the header starts exactly at the adress of the payload of the mob_t object)
	MAC_Header header;
	MAC_Payload payload;

	explicit MAC_Message(mob_t& physical_layer_message){

		//init header	
		if(physical_layer_message.size<sizeof(header)-sizeof(uint8_t)){

			cout << "ERROR: MAC_Message constructor: physical Message to short, to contain a MAC_Header!" << endl;
			cout << "Minimal Size: " << sizeof(header)-sizeof(uint8_t) << " size of current MAC_Frame: " << physical_layer_message.size << endl;


			cout << endl;

			print();
	
			cout << endl << endl << endl;

		}

		//set the correct size value for this layer (size refers always only to the payload, the Headerlength is not included. Since the MAC_Header is part of the Payload of the physical Message, its length has to be subtracted from the size value of the physical message, so that value specifies now the Number of Bytes in the MAC_Payload)
		size = physical_layer_message.size - sizeof(MAC_Header);

		if(header.messagetype==RTS){

			cout << "RTS Message" << endl;

		}else if(header.messagetype==CTS){

			cout << "CTS Message" << endl;

		}else if(header.messagetype==DATA){

			cout << "DATA Message" << endl;

		}else if(header.messagetype==ACK){

			cout << "ACK Message" << endl;

		}else{
			cout << "FATAL ERROR: failed decoding MAC Message" << endl;
		}

	

	}

	explicit MAC_Message(MessageType msgtyp, DeviceAddress send_adress, DeviceAddress receive_adress, char* pointer_to_databuffer, uint8_t size_of_databuffer){

		if(size_of_databuffer>MAX_NUMBER_OF_DATABYTES){

			cout << "FATAL ERROR: FAILED building MAC_Message Object because the size of the databuffer is greater than the MAX_NUMBER_OF_DATABYTES" << endl;
			return;

		}

		new (&header) MAC_Header( msgtyp, send_adress, receive_adress);
		new (&payload) MAC_Payload(pointer_to_databuffer, size_of_databuffer);
		//size=sizeof(MAC_Header)+sizeof(uint8_t)+size_of_databuffer;	
		//size=sizeof(MAC_Header)+size_of_databuffer;
		size=size_of_databuffer;
		//uint8_t 
	}





	void print(){

	cout << "MAC_HEADER:" << endl
	<< "sender_adress: " << header.send_adress << endl
	<< "receiver_adress: " << header.receive_adress << endl
	<< "message_type: " << header.messagetype << endl;

	cout << "MAC_PAYLOAD:" << endl
	<< "size of Payload: " << (int) size - sizeof(MAC_Header) << endl;

	//if (size-sizeof(MAC_Header)>0) cout << "content of Payload: " << payload.databuffer << endl;
	if (size > 0) cout << "content of Payload: " << payload.data << endl;	

	}


	mob_t* getPhysical_Layer_Message(){
		size += sizeof(MAC_Header); //the size value now have to correspond to the payload of the physical layer Message, and the the MAC_Header of the MAC_Message is part of that payload 
		return (mob_t*) this;
	}

	static MAC_Message* create_MAC_Message_from_Physical_Message(mob_t& physical_layer_message){

		return (MAC_Message*) new(&physical_layer_message) MAC_Message(physical_layer_message);


	}


} __attribute__((packed));





void send(char* buffer,int buffersize){

//string received_data;

//int size=buffersize;
int offset=0;

while(buffersize>0){


	//int counterlimit=min(buffersize,MAX_NUMBER_OF_DATABYTES);

	int counterlimit;
	if(buffersize<MAX_NUMBER_OF_DATABYTES) counterlimit=buffersize; else counterlimit=MAX_NUMBER_OF_DATABYTES;


	//for(int i=0;i<counterlimit;i++){

		MAC_Message mac_message1(DATA,25,38,&buffer[offset],counterlimit);
		cout << endl << endl << "Sending MAC_Message... " << endl;
		mac_message1.print();
		

		//string s(mac_message1.payload.databuffer);
		//received_data += s;
		

	//}

	offset += counterlimit;
	buffersize -= MAX_NUMBER_OF_DATABYTES;

}

	//cout << received_data << endl;




}






};  //namespace MAC end

using namespace MAC;



int mod(int x,int y){
 
 if(x<y) return x;
 
 return mod(x/y,y);

}


void printHEX(mob_t message){

	cout << "=========HEX_OUTPUT FROM mob_t object=========" << endl << endl;
	char* buffer = (char*) &message;
	
	for(int i=0;i<sizeof(mob_t);i+=4){

		cout << (void*) buffer[i] << endl;
		//if(i%20==0) cout << endl;

	}
	cout << endl << "=======HEX_OUTPUT FROM mob_t object END========" << endl << endl;


}









void test(){


const char* buffer="hallo du da!";


MAC_Message mac_msg_data(DATA,25,38,(char*)buffer,strlen(buffer));
MAC_Message mac_msg_rts(RTS,38,25,NULL,0);

cout << endl <<  "====DATA Message====" << endl;

mac_msg_data.print();

cout << endl <<  "====RTS Message====" << endl;
mac_msg_rts.print();

cout << "init test end" << endl << endl << endl; 



mob_t physical_layer_message1 = *mac_msg_data.getPhysical_Layer_Message();
mob_t physical_layer_message2 = *mac_msg_rts.getPhysical_Layer_Message();

printHEX(physical_layer_message1 );
printHEX(physical_layer_message2 );

MAC_Message* mac_message1 = MAC_Message::create_MAC_Message_from_Physical_Message(physical_layer_message1);
MAC_Message* mac_message2 = MAC_Message::create_MAC_Message_from_Physical_Message(physical_layer_message2);

cout << endl << "mac_message1: " << endl;

mac_message1->print();

cout << endl << "mac_message2: " << endl;

mac_message2->print();


cout << "decoding test end" << endl << endl << endl; 


}






void sendingtest(){



const char* buffer="Ein langer Text erfordert bei der übertragung über ein Funkmedium eine Aufteilung in mehrere Packete, da die Nachrichtenlänge sehr begrenzt ist. nichts destoso trotz wollen wir eine große Nachricht auf der Ebene höhere Protokolle versenden, ohne ums um Details kümmern zu müssen. Es ist von daher naheliegend, sie automatisch aufzuteilen und wieder zusammenzusetzen.";

cout << strlen(buffer) << endl;

int buffersize=strlen(buffer);

 MAC::send((char*)buffer,buffersize);

}

/**
#if sizeof(MAC_Message) != sizeof(mob_t)
      #error "MAC_Message und Physical LAyer Message sind unterschiedlich groß -> der Memory Overlay Mechanismus kann nicht funktionieren!!!"
#endif
*/













int main(){

	cout << "Größe einer Mac_Message: " << sizeof(MAC_Message) << endl;
	cout << "Größe einer Physical_Layer_Message: " << sizeof(mob_t) << endl;


	test();

	sendingtest();

	/*
	PhysicalLayerMessage physicallayermessage;
	PhysicalLayerPayload physicalpayload;


	MAC_Header* h = new (&physicallayermessage) MAC_Header(physicallayermessage);
	MAC_Payload* p1 = new (&physicalpayload) MAC_Payload(physicalpayload);
	*/

	/*

	mob_t physical_layer_message;
	mob_t physical_layer_message2_write_test;

	//strcpy(physical_layer_message.payload,"this is a message");
	int* array = (int*)physical_layer_message.payload;

	array[0]=5;
	array[1]=3;
	array[2]=RTS;
	array[3]=112;
	

	MAC_Header* h = new (physical_layer_message.payload) MAC_Header(physical_layer_message);
	MAC_Payload* p1 = new (physical_layer_message.payload+sizeof(MAC_Header)) MAC_Payload(physical_layer_message);


	cout 
	<< "sender_adress: " << h->send_adress << endl
	<< "receiver_adress: " << h->receive_adress << endl
	<< "message_type: " << h->messagetype << endl;

	cout 
	<< "payload_size: " << p1->payloadsize << endl;


	h = new (physical_layer_message2_write_test.payload) MAC_Header(physical_layer_message2_write_test);
	p1 = new (physical_layer_message2_write_test.payload+sizeof(MAC_Header)) MAC_Payload(physical_layer_message2_write_test);



	 h->send_adress = 20;
	 h->receive_adress = 15;
	 h->messagetype = CTS;

	 p1->payloadsize=53;

	cout 
	<< "sender_adress: " << h->send_adress << endl
	<< "receiver_adress: " << h->receive_adress << endl
	<< "message_type: " << h->messagetype << endl;

	cout 
	<< "payload_size: " << p1->payloadsize << endl;


	int* array2 = (int*) physical_layer_message2_write_test.payload;

	for(int i=0;i<(sizeof(MAC_Header)+sizeof(MAC_Payload))/4;i++){

		cout << "Index: " << i << " content: " << array2[i] << endl;

	}




	A a;
	a.a[0]=1;
        a.a[1]=42;
	a.output();
	

	B* b1 = new(&a) B(a);
	
	b1->output();	
	a.output();
	*/

	return 0;
}


