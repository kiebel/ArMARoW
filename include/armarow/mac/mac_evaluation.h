
#ifndef __MAC_EVALUATION_CA__
#define __MAC_EVALUATION_CA__

#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging


/* CLOCK */
///*

//#include "platform-cfg.h"               // platform dependent software config



#include "avr-halib/share/delay.h"      // delays and timings
#include "avr-halib/share/delegate.h"   //Delegator for user defined interrupt service routines
//TODO: include the global interruptlock when the new version of avr halib is available
#include "avr-halib/share/interruptLock.h"  //Global Lock


#include "armarow/armarow.h"            // main ArMARoW include
#include "armarow/debug.h"              // ArMARoW logging and debugging
#include "armarow/phy/phy.h"            // physical layer

/* CLOCK */

#include <avr-halib/regmaps/local.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/portmaps/icstick_portmap.h>
#include <avr-halib/ext/led.h>
#include <avr-halib/ext/button.h>

//#include <avr-halib/avr/newTimer.h>
#include <avr-halib/avr/timer.h>
#include <avr-halib/avr/regmaps.h> //ale regmaps aus non experimental Zweig

//AVR includes
#include <stdlib.h>

#include "../common.h"

using avr_halib::drivers::Clock;

using namespace avr_halib::regmaps;

//*/

namespace armarow{

	namespace MAC{



enum MAC_EVALUATION_ACTIVATION_STATE{Enable,Disable};

	/*!
	  \brief This is the configuration class for the Clock needed for the evaluation feature. The clock will cause an interrupt every Second and call a run to completion Task.
	*/
	struct EvaluationClockConfig
	{
		typedef uint16_t TickValueType;
		typedef Frequency<1> TargetFrequency; //every ms one timer interrupt
		typedef CPUClock TimerFrequency; //CPUClock
		typedef local::Timer4 Timer;
	};



/*!
  \brief This is the Interface of the Mac_Evaluation class.
*/
struct Mac_Evaluation_Interface{

/*!
  \brief This method is called in the receive function of the Mac Layer with the size parameter in the mac header. It therefore contains only payload bytes.
  It is used to correctly measure the netto bandwith.
*/
	void add_number_of_received_bytes(uint8_t number_of_received_bytes);

/*!
  \brief This method is a run to completion task, that outputs statistical information every second.
*/
	void print_and_reset_number_of_received_bytes();
/*!
  \brief This method initializes all neccessary data for the evaluation feature.
*/
	void init();

};



//if any other Template Parameter than Enable/Disable is passed as argument, it generates a well readible compile time error, so no need for a static assert
template <MAC_EVALUATION_ACTIVATION_STATE>
struct Mac_Evaluation;

/*!
  \brief This is the actual implementation of the evaluation feature for the mac protocol. It is only used, if you pass the Enable constant as third template parameter to the Mac_Layer.
  This class is mainly used to output statistical information every second and is used to meaure uptime and bandwith.
*/
template <>
struct Mac_Evaluation<Enable> : public Mac_Evaluation_Interface{


	Clock<EvaluationClockConfig> eval_clock;

	Led<Led0> led;

	uint16_t received_bytes_in_last_second;

	int uptime_in_sec;

	Mac_Evaluation(){
					
		init();
		

	}

	void init(){

		this->received_bytes_in_last_second=0;
		eval_clock.registerCallback<typeof *this, &Mac_Evaluation<Enable>::print_and_reset_number_of_received_bytes>(*this);

		this->uptime_in_sec=0;

	}

	
	//this method should be called in your receive method, if you got a data message, just at the number of bytes in the payload 
	void add_number_of_received_bytes(uint8_t number_of_received_bytes){
		this->received_bytes_in_last_second += number_of_received_bytes;
	}

	//the Idea is to call this method every second in a periodic interrupt service routine triggered by a clock 
	void print_and_reset_number_of_received_bytes(){

	  //secure ISR
	  avr_halib::locking::GlobalIntLock lock;

	  led.toggle();
	  ::logging::log::emit() 
	  //<< "received bytes:\t"  << this->received_bytes_in_last_second << "\t"//<< ::logging::log::endl 
	  //<< ((int) this->received_bytes_in_last_second << 3) << " kbit/s" //mal 8
	  //<< ::logging::log::endl
	  << "uptime:\t" << uptime_in_sec << " sec"
	  << ::logging::log::endl;

	  this->received_bytes_in_last_second=0;

	  this->uptime_in_sec++;
	}

};

/*!
  \brief This is the dummy implementation of the evaluation feature for the mac protocol with only empty methods. It is only used, if you pass the Disable constant as third template parameter to the Mac_Layer.
If you want to disable evaluation, this class is used, if used correctly, it doesn't even cause one byte overhead (MAC Protocoll has to inherit from evaluation, in that way it will be no overhead at all, because empty methods are thrown away by the compiler)
*/
template <>
struct Mac_Evaluation<Disable> : public Mac_Evaluation_Interface{

	Mac_Evaluation(){}

	void add_number_of_received_bytes(uint8_t number_of_received_bytes){}

	void print_and_reset_number_of_received_bytes(){}

	void init(){}

};







		} //end namespace mac

	} //end namespace armarow


#endif

