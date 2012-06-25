#define ARMAROW_DEBUG_DISABLE

#include <armarow/debug.h>
#include <config.h>

Mac mac;
Mac::MessageType message;
RelayMessage out;

typedef Interrupt::InterruptManager<Mac::InterruptSlotList> IM;

static const char* const content = CONTENT;

void receive(Mac::MessageType& msg)
{
    log::emit() << "received something: [" <<  msg.header.source.pan 
                << "," << (uint16_t)msg.header.source.id 
                << "](" << (uint16_t)msg.header.seqNumber 
                << ") -> " << msg.payload 
                << "(" << (uint16_t)msg.header.size << ")" << log::endl;

    if(msg.header.source.id != 1)
        return;

    if(msg.header.size!=sizeof(CONTENT))
        return;

    for(uint8_t i=0;i<msg.header.size;i++)
        if(msg.payload[i] != content[i])
            return;

    log::emit() << "is ping :-)" << log::endl;

    out.header.destination.pan = 0;
    out.header.destination.id  = 20;
    out.payload.source.id      = msg.header.source.id;
    out.payload.source.pan     = msg.header.source.pan;
    out.payload.seqNr          = msg.header.seqNumber;
    out.payload.rssi           = (int8_t)msg.properties.rssi-90;
    out.payload.lqi            = msg.properties.lqi;

    Error error=mac.send(out);
    if(error)
        log::emit() << "Error relaying: " << error << log::endl;
}

void handleEvent(Mac::MessageType& msg)
{ 
    switch(msg.properties.state)
    {
        case(armarow::common::RX_DONE): receive(msg);
                                        break;
        case(armarow::common::TX_DONE): log::emit() << "TX complete" << log::endl;
        default: break;
    }
}

int main() {

    IM::init();

    {
        Mac::Attributes::Callback cb;
        cb.value.bind<&handleEvent>();
        mac.setAttribute(cb);
        
        Mac::Attributes::Channel channel;
        channel.value=Mac::Attributes::Channel::Channels::Channel11;
        mac.setAttribute(channel);
    }



    log::emit() << "Ping Relay" << log::endl << log::endl;
    sei();

    mac.receive(message);

    platform::Idler::idle();

    return 0;
}

