//#define LOGGING_DISABLE

#define ARMAROW_DEBUG_DISABLE
#include <armarow/debug.h>


#include <config.h>
#include <idler.h>
#include <debug_portmap.h>

typedef Interrupt::InterruptManager< Radio::InterruptSlotList > IM;

Radio radio;
Radio::MessageType message;

void printReceivedMessage()
{
    UsePortmap(debug, platform::Debug);
    debug.debug0.pin=true;
    SyncPortmap(debug);

    debug.debug1.pin=true;
    SyncPortmap(debug);

    log::emit() << "->[" << (uint16_t)message.header.size
                << "] ";
    for(uint8_t i=0;i<message.header.size;i++)
        log::emit() << log::hex << (uint16_t)message.payload[i] << " ";
    log::emit() << log::dec << log::endl;
}

void handleEvents(Radio::MessageType& msg)
{
    switch(msg.properties.state)
    {
        case(armarow::common::RX_DONE): printReceivedMessage();
        default: break;
    }
}

void init()
{
    Radio::Attributes::Callback cb;
    cb.value.bind<&handleEvents>();
    radio.setAttribute(cb);

    Radio::Attributes::Channel channel;
    channel.value = Radio::Attributes::Channel::Channels::Channel11;
    radio.setAttribute(channel);

    UsePortmap(debug, platform::Debug);
    debug.debug0.ddr=true;
    debug.debug1.ddr=true;
    SyncPortmap(debug);
}   

int main()
{

    IM::init();

    init();

    log::emit() << "Sniffer" << log::endl << log::endl;
    sei();

    radio.receive(message);

    platform::Idler::idle();

    return 0;
}
