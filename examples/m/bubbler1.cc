#include <radio.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/share/freq.h>

#include "testphy.h"

using avr_halib::config::Frequency;
using avr_halib::drivers::Clock;

struct ClockConfig : public platform::avr::clock::Clock1BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint8_t TickValueType;
};

Clock<ClockConfig> periodicTrigger;

typedef test::phy::Phy Myphy;
Myphy bob;
Myphy::Message message;

void send(){
    
    bob.send(message);
    log::emit() << "Sending message "
                << ((uint32_t*)message.payload)[0]++ << ::logging::log::endl;
}




int main() {
    periodicTrigger.registerCallback<send>();

    log::emit() << "Starting bubbler (repeated send of the same message)!"
                << log::endl << log::endl;

    message.header.size=sizeof(uint32_t);
    *reinterpret_cast<uint32_t*>(message.payload)=0;

    sei();

    Idler::idle();

    return 0;
}
