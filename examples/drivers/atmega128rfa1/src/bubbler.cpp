//#define LOGGING_DISABLE

#include <config.h>
#include <debug_portmap.h>
#include <avr-halib/avr/portmap.h>

Radio rc;
Radio::MessageType message;

int main() {
    IM::init();
    message.header.size=127;
    for(uint8_t i=0;i<message.header.size-1;i++)
        message.payload[i]='0'+i%10;

    message.payload[message.header.size-1]='\0';

    log::emit() << "Periodic bubbler" << log::endl << log::endl;

    UsePortmap(debug, platform::Debug);
    debug.debug0.ddr=true;
    debug.debug1.ddr=true;
    SyncPortmap(debug);

    while(true)
    {
        debug.debug0.pin=true;
        SyncPortmap(debug0);
        Error error=rc.send(message);
        if(error)
        {
            log::emit() << error << log::endl;
            continue;
        }
        debug.debug1.pin=true;
        SyncPortmap(debug1);
    }
    return 0;
}
