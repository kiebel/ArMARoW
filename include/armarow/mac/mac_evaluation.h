
#ifndef __MAC_EVALUATION_CA__
#define __MAC_EVALUATION_CA__

#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging


namespace armarow{

	namespace MAC{



enum MAC_EVALUATION_ACTIVATION_STATE{Enable,Disable};


struct Mac_Evaluation_Interface{

	void add_number_of_received_bytes(uint8_t number_of_received_bytes);

	void print_and_reset_number_of_received_bytes();


};



//if any other Template Parameter than Enable/Disable is passed as argument, it generates a well readible compile time error, so no need for a static assert
template <MAC_EVALUATION_ACTIVATION_STATE>
struct Mac_Evaluation;

//here is the actual implementation of the evaluation for a mac protocoll
template <>
struct Mac_Evaluation<Enable> : public Mac_Evaluation_Interface{

	uint16_t received_bytes_in_last_second;

	Mac_Evaluation(){
					
		this->received_bytes_in_last_second=0;

	}
	
	//this method should be called in your receive method, if you got a data message, just at the number of bytes in the payload 
	void add_number_of_received_bytes(uint8_t number_of_received_bytes){
		this->received_bytes_in_last_second += number_of_received_bytes;
	}

	//the Idea is to call this method every second in a periodic interrupt service routine triggered by a clock 
	void print_and_reset_number_of_received_bytes(){

	  ::logging::log::emit() << "received bytes in last second: "  << this->received_bytes_in_last_second << ::logging::log::endl;

	  this->received_bytes_in_last_second=0;

	}

};

//if you want to disable evaluation, this class is used, if used correctly, it doesn't even cause one byte overhead (MAC Protocoll has to inherit from evaluation, in that way it will be no overhead at all, because empty methods are thrown away by the compiler)
template <>
struct Mac_Evaluation<Disable> : public Mac_Evaluation_Interface{

	void add_number_of_received_bytes(uint8_t number_of_received_bytes){}

	void print_and_reset_number_of_received_bytes(){}


};







		} //end namespace mac

	} //end namespace armarow


#endif

