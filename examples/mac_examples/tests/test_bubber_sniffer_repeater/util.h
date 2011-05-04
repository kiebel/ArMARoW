#include "armarow/mac/mac_message.h"

struct My_Information{

	//char data[100];

	int messwert1;
	//int messwert2;
	//int messwert3;

	int global_sequence_number;

 	uint8_t lqi;
	uint8_t ed;

	armarow::MAC::MAC_Header header;

	My_Information(){

	    messwert1=10;
	    //messwert2=20;
	    //messwert3=30;

	    global_sequence_number=0;

	}

	void print(){

	    ::logging::log::emit() << "messwert1: " << messwert1 << ::logging::log::endl;
	    //::logging::log::emit() << "messwert2: " << messwert2 << ::logging::log::endl;
	    //::logging::log::emit() << "messwert3: " << messwert3 << ::logging::log::endl;

	//header.print();

	}
	
} ;
