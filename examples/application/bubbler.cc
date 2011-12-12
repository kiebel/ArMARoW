#include <radio.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/share/freq.h>

using avr_halib::config::Frequency;
using avr_halib::drivers::Clock;

typedef platform::config::RadioDriver<> RadioController;
RadioController::mob_t message;

struct ClockConfig : public platform::avr::clock::Clock1BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint8_t TickValueType;
};

Clock<ClockConfig> periodicTrigger;
RadioController rc;

void send(){
    rc.setStateTRX(armarow::PHY::tx_on);
    rc.send(message);
    log::emit() << "Sending message "
                << ((uint32_t*)message.payload)[0]++ << ::logging::log::endl;
}

int main() {
    uint8_t channel=11;
    periodicTrigger.registerCallback<send>();

    log::emit() << "Starting bubbler (repeated send of the same message)!"
                << log::endl << log::endl;
    
    message.size=sizeof(uint32_t);
    *reinterpret_cast<uint32_t*>(message.payload)=0;

    rc.init();
    rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);

    sei();

    Idler::idle();

    return 0;
}
