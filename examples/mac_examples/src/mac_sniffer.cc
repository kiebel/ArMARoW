#define ARMAROW_DEBUG_DISABLE

#include <armarow/debug.h>
#include <config.h>
#include <idler.h>
#include <debug_portmap.h>

Mac mac;
Mac::MessageType message;

void receive(Mac::MessageType& msg){
    
    if(msg.properties.state!=armarow::common::RX_DONE)
        return;

    log::emit<log::Trace>() << "received something" << log::endl;

    UsePortmap(debug, platform::Debug);
    debug.debug0.pin=true;
    debug.debug1.pin=true;
    SyncPortmap(debug);
    
    
    log::emit() << "["    << message.header.source.pan 
                << ", "   << message.header.source.id 
                << "]: "  << (uint16_t)message.header.seqNumber 
                << " -> " << (const char*)(message.payload)
                << log::endl;


}

void init()
{
    Mac::Attributes::Callback cb;
    cb.value.bind<&receive>();
    mac.setAttribute(cb);

    UsePortmap(debug, platform::Debug);
    debug.debug0.ddr=true;
    debug.debug1.ddr=true;
    SyncPortmap(debug);
}   

int main() {

    IM::init();

    init();

    log::emit() << "Sniffer" << log::endl << log::endl;
    sei();

    mac.receive(message);
    platform::Idler::idle();

    return 0;
}

