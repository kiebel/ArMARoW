#include <radio.h>

typedef platform::config::RadioDriver<> RadioController;
RadioController::Message message;

RadioController rc;

void receive(){
    rc.receive(message);
    log::emit() << "received message: "
                << (uint16_t)(message.payload[0])
                << log::endl;
}

void init()
{
    RadioController::Attributes::Callback cb;
    cb.value.bind<&receive>();
    rc.setAttribute(cb);
}   

int main() {

    init();

    log::emit() << "Sniffer" << log::endl << log::endl;
    sei();

    Idler::idle();

    return 0;
}
