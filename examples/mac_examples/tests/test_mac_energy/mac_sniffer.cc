

#include "armarow/mac/mac_csma_ca.h"

#include "armarow/mac/util.h"

//#include <avr-halib/avr/basicADC.h>


//using avr_halib::drivers::Clock;

//using namespace avr_halib::regmaps;

UseInterrupt(SIG_OUTPUT_COMPARE5A);

	struct JitterMeasurementClockConfig
	{
		typedef uint16_t TickValueType;
		typedef Frequency<1> TargetFrequency; //every second one timer interrupt
		typedef CPUClock TimerFrequency; //CPUClock
		typedef local::Timer5 Timer; //16 Bit timer
	};




struct My_MAC_Config : public armarow::MAC::MAC_Configuration{

	enum {
		channel=3,
		mac_adress_of_node=20
	};

};



armarow::MAC::MAC_CSMA_CA<My_MAC_Config,platform::config::rc_t,armarow::MAC::Enable> mac;


armarow::MAC::mob_t messageobject;

Clock<JitterMeasurementClockConfig> jitter_clock;
//Adc adc;

#undef LOGGING_DISABLE


MessageLossMeasurement<110> msg_measurement_object;

uint32_t global_sequence_number=0;

int old_timer_value=0;
int new_timer_value=0;
bool has_received_at_least_once=false;

//erst notifizieren, und dann receive aufrufen, wo man receive puffer übergibt
void callback_recv() {

	//Uhr stoppen
	jitter_clock.stop();

 	if(mac.receive(messageobject)!=0){

		new_timer_value=(int) jitter_clock.getCounter(); //read microticks

		Clock<JitterMeasurementClockConfig>::Time timeobject;	

		jitter_clock.getTime(timeobject);

		//(int) timeobject.ticks
		//(int) timeobject.microticks

		Clock<JitterMeasurementClockConfig>::config::TickValueType ticks = timeobject.ticks;
		Clock<JitterMeasurementClockConfig>::config::MicroTickValueType microTicks = timeobject.microTicks;

		::logging::log::emit() << "tick: " << ticks << "microtick:" << microTicks << ::logging::log::endl;

		int total_number_of_micro_ticks=Clock<JitterMeasurementClockConfig>::config::microTickMax*ticks+microTicks;
		::logging::log::emit() << "total microticks:" << total_number_of_micro_ticks << ::logging::log::endl;


		messageobject.get_object_from_payload(msg_measurement_object);


/*		
		while(global_sequence_number<msg_measurement_object.global_sequence_number){
			::logging::log::emit()
			<< PROGMEMSTRING("lost message: ") << (int) global_sequence_number++
        		<< ::logging::log::endl; 
		}		


		*/

		//int tmp = (new_timer_value-old_timer_value) * 0.030518;

		if(has_received_at_least_once)
		::logging::log::emit()
		//<< PROGMEMSTRING("time : ") << new_timer_value //<< (int) msg_measurement_object.global_sequence_number
		//<< PROGMEMSTRING("jitter : ") 
		<< (new_timer_value-old_timer_value) 
		//<< "," << tmp  //* 0.000030518 * 1000 // in ms
		//(new_timer_value-old_timer_value) * 0,000030518 = ((new_timer_value-old_timer_value) * 30518) / 10^9
        	<< ::logging::log::endl;

//0,000030518 sekunden je Tick
//die Timerwerte sind ticks
//timerwert * 0,000030518 = zeit in ms -> jitter ticks in ms umrechnen!
//wir haben einen 16 bit timer, der aber nur 15 bit nutzt -> 2^15 = 32768 -> einmal pro Sekunde soll der Interrupt ausgelöst werden, also: 1/32768=0,000030518 s
//ticks * 0,030518 ms = Zeit in ms

		//global_sequence_number++;
		


		old_timer_value=new_timer_value;
		has_received_at_least_once=true;

	}else{

		::logging::log::emit()
        	<< PROGMEMSTRING("Failed receiving message!") 
        	<< ::logging::log::endl << ::logging::log::endl;

	}

	//neu aufziehen und starten
	jitter_clock.setCounter(0); //reset clock
	jitter_clock.start();

}
/*! \brief  Initializes the physical layer.*/
void test_asynchron_receive() {

    mac.onMessageReceiveDelegate.bind<callback_recv>();
	/*
    while(1){

                uint8_t value=0;
		if(!adc.configure<uint8_t>(ConfiguredADCRegMap::channel0, ConfiguredADCRegMap::internal))
		{
			log::emit() << "Error: ADC still busy" << log::endl;
			continue;
		}
		adc.startConversion();
		while(!adc.isDone());
		adc.fetchValue(value);
		log::emit() << "Value: " << value << log::endl;

    }
	*/
}
/* === main ================================================================= */
int main() {

    

    sei();                              // enable interrupts
    ::logging::log::emit()
        << PROGMEMSTRING("Starting sniffer!") << ::logging::log::endl
        << "maxNumMicroTicks" << Clock<JitterMeasurementClockConfig>::config::microTickMax << ::logging::log::endl //=31250
	<< "Jitter Values:"
        << ::logging::log::endl << ::logging::log::endl;

    test_asynchron_receive();    //aus bzw. einkommentieren für aynchronen/synchronen test                       



}
