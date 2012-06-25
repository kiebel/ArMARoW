#define LOGGING_DISABLE

#include <armarow/debug.h>
#include <config.h>
#include <idler.h>
#include <debug_portmap.h>
#include <string.h>

#include "avr-halib/avr/uart.h"
#include "avr-halib/common/cdeviceframe.h"
#include "avr-halib/common/delay.h"

using avr_halib::drivers::Uart;
using avr_halib::regmaps::local::Uart1;

struct Config : public Uart::DefaultConfig
{
    typedef Uart1 RegMap;
    static const Uart::BaudRateType baudRate = 57600;
};

struct CFrameConfig : public CFrameModifierBase
{
	enum {esc = 'e', sofr = 'a', eofr = 'b', escmod = 0x7f};
};

typedef avr_halib::drivers::Uart::configure<Config>::type Uart;

typedef CFrame< CFrameConfig > Framing;
typedef CDeviceFrameNoInt< Uart, uint8_t, 255, Framing> FrameDevice;
typedef FrameDevice::mob_t SerialMessage;

Mac mac;
Mac::MessageType in;
SerialMessage out;
FrameDevice serial;

typedef Interrupt::InterruptManager<Mac::InterruptSlotList> IM;

void receive(Mac::MessageType& msg){

    out.size                 = 0;
    memcpy(out.payload + out.size, &msg.header.source, sizeof(msg.header.source));
    out.size                += sizeof(msg.header.source);
    out.payload[out.size++]  = 1;
    memcpy(out.payload + out.size, &msg.payload, msg.header.size);
    out.size                += msg.header.size;

    serial.send(out);
}

void handleEvent(Mac::MessageType& msg)
{ 
    switch(msg.properties.state)
    {
        case(armarow::common::RX_DONE): receive(msg);
                                        break;
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
    sei();

    mac.receive(in);
    platform::Idler::idle();

    return 0;
}

