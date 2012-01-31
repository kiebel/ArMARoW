#define ARMAROW_DEBUG_DISABLE
#include <armarow/debug.h>
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

uint16_t counter;

void eval()
{
    log::emit() << "Messages: " << counter << log::endl;
    counter=0;
}

void count(){
    counter++;
}

void init(){

    RadioController::Attributes::Callback cb;
    cb.value.bind<count>();
    rc.setAttribute(cb);
    periodicTrigger.registerCallback<eval>();
}

int main() {
    init();
    counter=0;

    log::emit() << "Bandwidth measure" << log::endl << log::endl;
    sei();

    Idler::idle();

    return 0;
}
