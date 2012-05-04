#include <config.h>

Radio rc;
Clock clock;
Clock::Time time;

typedef Radio::Attributes::ClearChannelParameters CCAParams;
typedef Radio::Attributes::ClearChannelAssessment CCA;

int main() {

    IM::init();

    log::emit() << "CCA-Test" << log::endl << log::endl;

    sei();

    while(true)
    {
        armarow::common::Error error;
        CCAParams ccaParams;
        CCA cca;

        ccaParams.value.threshold = CCAParams::maxThreshold;
        ccaParams.value.mode      = CCAParams::ClearChannelModes::CarrierSense;

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

