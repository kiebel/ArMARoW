#include <config.h>
#include <idler.h>

Radio radio;
Radio::MessageType message;

void receive(Radio::EventType event){
    switch(event)
    {
        case(Radio::Events::rxEnd):
            radio.receive(message);
            log::emit() << "received message: "
                        << (uint16_t)(message.payload[0])
                        << log::endl;
        default:break;
    }
}

void init()
{
    Radio::Attributes::Callback cb;
    cb.value.bind<&receive>();
    radio.setAttribute(cb);
}   

int main() {

    IM::init();

    init();

    log::emit() << "Sniffer" << log::endl << log::endl;
    sei();

    platform::Idler::idle();

    return 0;
}
