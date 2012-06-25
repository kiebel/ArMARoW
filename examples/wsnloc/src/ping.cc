#define ARMAROW_DEBUG_DISABLE
//#define LOGGING_DISABLE
#include <armarow/debug.h>

#include <config.h>
#include <string.h>

using armarow::common::Error;

typedef boost::mpl::joint_view< Mac::InterruptSlotList, Trigger::InterruptSlotList >::type InterruptList;
typedef Interrupt::InterruptManager< InterruptList > IM;


static const char* const content = CONTENT;
static const uint16_t sendPeriod = SEND_PERIOD;

MessageType msg;
Mac mac;
Trigger sendTrigger;

void send()
{
    msg.header.size=sizeof(CONTENT);
    sendTrigger.setup<Trigger::Units::matchA>(sendPeriod);

    Error error=mac.send(msg);
    if(error)
        log::emit() << "transmission rejected due to " << error << log::endl;
    else
        log::emit() << "transmitted message" << log::endl;
}

int main()
{
    IM::init();

    {

        Mac::Attributes::Channel channel;
        channel.value=Mac::Attributes::Channel::Channels::Channel11;
        mac.setAttribute(channel);

        Trigger::CallbackType cb;
        cb.bind<&send>();
        sendTrigger.setCallback<Trigger::Units::matchA>(cb);

        msg.header.size=sizeof(CONTENT);

        for(uint8_t i=0;i<sizeof(CONTENT);i++)
            msg.payload[i]=(uint8_t)content[i];

        msg.header.destination.pan = 0;
        msg.header.destination.id  = 255;

    }

    log::emit() << PROGMEMSTRING("Ping Node") << log::endl;
    sei();

    sendTrigger.setup<Trigger::Units::matchA>(sendPeriod);

    platform::Idler::idle();

    return 0;
}

