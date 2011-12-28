#include <string.h>

#include <radio.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/share/freq.h>

using avr_halib::config::Frequency;
using avr_halib::drivers::Clock;

typedef platform::config::RadioDriver<> RadioController;
RadioController::Message message;

struct ClockConfig : public platform::avr::clock::Clock1BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint8_t TickValueType;
};

Clock<ClockConfig> periodicTrigger;
RadioController rc;

void send(){
    rc.send(message);
    log::emit() << "Sending message: "
                << (uint16_t)(message.payload[0]++)
                << log::endl;
}

void init(){

    periodicTrigger.registerCallback<send>();
    
    message.header.size=sizeof(uint8_t);
    message.payload[0]=0;
}

int main() {

    init();

    log::emit() << "Periodic bubbler" << log::endl << log::endl;
    sei();

    Idler::idle();

    return 0;
}
