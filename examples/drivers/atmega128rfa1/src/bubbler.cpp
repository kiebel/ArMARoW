#define LOGGING_DISABLE

#include <config.h>

Radio rc;
Radio::MessageType message;

int main() {
    IM::init();
    message.header.size=127;

    log::emit() << "Periodic bubbler" << log::endl << log::endl;

    while(true)
    {
        Error error=rc.send(message);
        if(error)
            log::emit() << error << log::endl;
    }
    return 0;
}
