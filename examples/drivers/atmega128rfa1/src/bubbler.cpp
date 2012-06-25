//#define LOGGING_DISABLE
#define ARMAROW_DEBUG_DISABLE

#include <armarow/debug.h>


#include <config.h>
#include <debug_portmap.h>
#include <avr-halib/avr/portmap.h>

typedef Interrupt::InterruptManager< Radio::InterruptSlotList > IM;

Radio rc;
Radio::MessageType message;

void init()
{
    message.header.size=127;
    for(uint8_t i=0;i<message.header.size-1;i++)
        message.payload[i]='0'+i%10;

    message.payload[message.header.size-1]='\0';

    Radio::Attributes::Channel channel;
    channel.value = Radio::Attributes::Channel::Channels::Channel11;
    rc.setAttribute(channel);   

    UsePortmap(debug, platform::Debug);
    debug.debug0.ddr=true;
    debug.debug1.ddr=true;
    SyncPortmap(debug);
}

int main() {
    IM::init();

    init();

    log::emit() << "Periodic bubbler" << log::endl << log::endl;

    sei();

    while(true)
    {
        UsePortmap(debug, platform::Debug);
        debug.debug0.pin=true;
        SyncPortmap(debug0);
        Error error=rc.send(message);
        if(error)
        {
            log::emit() << error << log::endl;
            continue;
        }
        else
            log::emit() << "bubble" << log::endl;
        delay_ms(1000);
    }
    return 0;
}
