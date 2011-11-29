#include <string.h>

#include <armarow/common/interface.h>
#include <armarow/common/attributeContainer.h>

#include <avr-halib/share/interruptLock.h>

using armarow::common::Error;

struct Message
{
    typedef uint16_t Address;

    uint8_t size;
    Address dstAddress;
    uint8_t payload[126];
    Error state;
};

using armarow::common::AttributeContainer;
using armarow::common::Interface;

class TestMAC : public Interface<Message>
{
    public:
        static const Message::Address BROADCAST_ADDRESS=255;

        struct TxCompleteAttribute{};

        typedef AttributeContainer< TxCompleteAttribute, 
                                    Delegate<Message&> 
                                  > 
                    TxCompleteCallback;

    private:
        Delegate<Message&> txCompleteDelegate;

    public:
        Error send(Message& msg)
        {
            txCompleteDelegate(msg);
            return armarow::common::SUCCESS;
        }

        Error setAttribute(const TxCompleteCallback& attr)
        {
            txCompleteDelegate = attr.param;
            return armarow::common::SUCCESS;
        }
};

typedef TestMAC MacLayer;
MacLayer mac;

Message msg;

const char* const content="MAC LAYER TEST";

void txComplete(Message& msg){
    avr_halib::locking::GlobalIntLock lock;

    log::emit() << "transmission finished with " << msg.state << log::endl;
}

void init()
{
    strcpy(reinterpret_cast<char*>(msg.payload), content);

    msg.size        = strlen(content);
    msg.dstAddress = MacLayer::BROADCAST_ADDRESS;

    MacLayer::TxCompleteCallback txCompleteCallback;

    txCompleteCallback.param.bind<&txComplete>();

    Error error=mac.setAttribute(txCompleteCallback);
    if(error)
        log::emit<log::Error>() << "setting of callback failed: " << error << log::endl;
    sei();
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

