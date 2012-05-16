#define ARMAROW_DEBUG_DISABLE

#include <armarow/debug.h>

#include <config.h>
#include <string.h>
#include <debug_portmap.h>

using armarow::common::Error;


typedef Mac::MessageType MessageType;
Mac mac;

MessageType msg;

void init()
{
    strcpy(reinterpret_cast<char*>(msg.payload), "Mac layer test");

    msg.header.size            = 15;
    msg.header.destination.pan = 0;
    msg.header.destination.id  = 255;

/*    Mac::TxCompleteCallback txCompleteCallback;

    txCompleteCallback.value.bind<&txComplete>();

    Error error=mac.setAttribute(txCompleteCallback);
    if(error)
        log::emit<log::Error>() << "setting of callback failed: " << error << log::endl;
    sei();*/
}

int main()
{
    IM::init();

    UsePortmap(debug, platform::Debug);
    debug.debug0.ddr=true;
    debug.debug1.ddr=true;
    debug.debug2.ddr=true;
    SyncPortmap(debug);

    init();

    log::emit() << PROGMEMSTRING("Mac Bubbler") << log::endl;
    sei();

    while(true)
    {
        delay_ms(1000);
        debug.debug0.pin=true;
        SyncPortmap(debug);
        msg.header.size=15;
        log::emit() << "transmitting message" << log::endl;
        Error error=mac.send(msg);
        if(error)
        {
            log::emit<log::Error>() << "transmission rejected due to " << error << log::endl;
            continue;
        }
//        debug.debug1.pin=true;
//        SyncPortmap(debug);
    }
    return 0;
}

