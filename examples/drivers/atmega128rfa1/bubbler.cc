#define LOGGING_DISABLE
#include <armarow/debug.h>
#include <radio.h>

struct Config : public armarow::drv::atmega128rfa1::DefaultConfig
{
    static const bool rxOnIdle = false;
    static const bool useInterrupt = false;
};

typedef platform::config::RadioDriver<Config> RadioController;
RadioController::Message message;

RadioController rc;

int main() {
    message.header.size=127;

    log::emit() << "Periodic bubbler" << log::endl << log::endl;

    while(true)
    {
        armarow::common::Error error=rc.send(message);
        if(error)
            log::emit() << error << log::endl;
    }
    return 0;
}
