#include <armarow/common/message.h>

#include <avr-halib/avr/InterruptManager/InterruptManager.h>

typedef armarow::common::Message<128> Message;

struct MyHeader
{
    uint32_t id;
};

struct MyAttributes
{
    uint32_t attr;
};

typedef Message::extend<MyHeader, MyAttributes>::type NewMessage;
NewMessage newMsg;
Message oldMsg;

int main()
{
    InterruptManager<>::init();

    newMsg.id=0;
    newMsg.state=armarow::common::BUSY;
    newMsg.attr=10;
    NewMessage& test=oldMsg;
    return 0;
}
