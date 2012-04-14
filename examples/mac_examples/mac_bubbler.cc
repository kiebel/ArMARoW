#include <platform.h>
#include <radio.h>
#include <armarow/mac/simple802.15.4.h>
#include <string.h>

using armarow::common::Error;
using armarow::mac::Simple802_15_4;

typedef platform::config::RadioDriver<> Phy;

struct MacConfig
{
    typedef Clock1 BackoffTimer;
    static const uint16_t pan     = 0;
    static const uint16_t address = 1;
};

typedef Simple802_15_4<MacConfig, Phy> Mac;
typedef Mac::Message Message;
Mac mac;

Message msg;

const char* const content="MAC LAYER TEST";

void init()
{
    strcpy(reinterpret_cast<char*>(msg.payload), content);

    msg.header.size            = strlen(content);
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
    init();

    log::emit() << PROGMEMSTRING("Mac Bubbler") << log::endl;

    while(true)
    {
        Error error=mac.send(msg);
        if(error)
            log::emit<log::Error>() << "transmission rejected due to " << error << log::endl;
        delay_ms(500);
    }
    return 0;
}

