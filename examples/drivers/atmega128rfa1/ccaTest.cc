#include <radio.h>

#include <avr-halib/avr/clock.h>
#include <avr-halib/share/freq.h>

using avr_halib::config::Frequency;
using avr_halib::drivers::Clock;

struct ClockConfig : public platform::avr::clock::Clock1BaseConfig
{
    typedef Frequency<1000> TargetFrequency;
    typedef uint32_t TickValueType;
};

Clock<ClockConfig> clock;
Clock<ClockConfig>::Time time;

struct Config : public armarow::drv::atmega128rfa1::DefaultConfig
{
    static const bool rxOnIdle = false;
};

typedef platform::config::RadioDriver<Config> RadioController;
RadioController rc;

int main() {

    log::emit() << "CCA-Test" << log::endl << log::endl;

    sei();

    while(true)
    {
        armarow::common::Error error;
        RadioController::Attributes::CCAParams ccaParams;
        RadioController::Attributes::CCA cca;

        ccaParams.value.threshold = RadioController::Attributes::CCAParams::maxThreshold;
        ccaParams.value.mode      = RadioController::Attributes::CCAParams::CarrierSense;

        error=rc.setAttribute(ccaParams);
        if(error)
        {
            log::emit< log::Error >() << "configuring CCA - " << error << log::endl;
            continue;
        }
        
        error=rc.getAttribute(cca);
        if(error)
        {
            log::emit< log::Error >() << "could not start CCA - " << error << log::endl;
            continue;
        }
        

        if(!cca.value)
        {
            clock.getTime(time);
            log::emit() << "[" << time.ticks << ", " << time.microTicks << "] Busy" << log::endl;
        }
    }

    return 0;
}

