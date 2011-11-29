
#include "armarow/mac/mediumAccessLayerInterface.h"
#include <avr-halib/share/interruptLock.h>

using armarow::common::Error;

struct TestMAC : public armarow::mac::MacInterface
{
	typedef uint16_t Address;

	static const Address BROADCAST_ADDRESS=255;

	struct Message
	{
		uint8_t size;
		Address dstAddress;
		uint8_t payload[126];
		Error state;
	};

	Delegate<Message&> txCompleteDelegate;

	template<void (*f)(Message& msg)>
	struct TxCompleteCallback{};

	Error send(Message& msg)
	{
		txCompleteDelegate(msg);
		return armarow::common::SUCCESS;
	}

	template<void (*f)(Message&)>
	Error setAttribute(const TxCompleteCallback<f>& attr)
	{
		txCompleteDelegate.bind<f>();
		return armarow::common::SUCCESS;
	}
};

typedef TestMAC MacLayer;
MacLayer mac;

typedef MacLayer::Message Message;
Message msg;

PGM_P content="MAC LAYER TEST";

void txComplete(Message& msg){
    avr_halib::locking::GlobalIntLock lock;

    log::emit() << "transmission finished with " << msg.state << log::endl;
}

void init()
{
    strcpy_P(reinterpret_cast<char*>(msg.payload), content);

    msg.size        = strlen_P(content);
    msg.dstAddress = MacLayer::BROADCAST_ADDRESS;

	MacLayer::TxCompleteCallback<&txComplete> txCompleteCallback;

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

