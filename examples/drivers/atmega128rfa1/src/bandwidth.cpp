#define LOGGING_DISABLE //#define ARMAROW_DEBUG_DISABLE

#include <config.h>
#include <idler.h>

Radio::MessageType message;
Radio rc;
//Trigger periodicTrigger;

uint16_t counter;

void eval()
{
    log::emit() << "Messages: " << counter << log::endl;
    counter=0;
}

void count(Radio::EventType event)
{
    switch(event)
    {
        case(Radio::Events::rxEnd): PINE|=1<<4;//counter++;
        default:                    break;
    }
}

void init(){

    Radio::Attributes::Callback radioCB;
    radioCB.value.bind<count>();
    rc.setAttribute(radioCB);
//    Delegate<void> triggerCB;
//    triggerCB.bind<&eval>();
//    periodicTrigger.setCallback(triggerCB);
}

int main() {
    IM::init();

    init();
    counter=0;

    log::emit() << "Bandwidth measure" << log::endl << log::endl;
    DDRE|=1<<4;
    sei();

    platform::Idler::idle();

    return 0;
}
