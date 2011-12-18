#include <radio.h>
#include <avr-halib/avr/clock.h>
#include <avr-halib/share/freq.h>
#include <armarow/common/interface.h>
#include <armarow/common/message.h>
#include <armarow/common/error.h>

namespace narmarow {
    namespace common = armarow::common;
    namespace phy {
        struct Message: public common::Message< 128 >::type{};
        
        struct Phy: public common::Interface< phy::Message >
        {
            typedef platform::config::RadioDriver<> RadioController;
            RadioController::mob_t message;
            RadioController rc;

            typedef phy::Message Message;
            
            Phy()
            {
                uint8_t channel=11;
                rc.init();
                rc.setAttribute(armarow::PHY::phyCurrentChannel, &channel);
            }
            
            common::Error send(Message& msg)
            {
                for(uint8_t i = 0; i < 128; i++)
                {
                    ((uint8_t*)&message)[i]=((uint8_t*)&msg)[i];
                }
                rc.setStateTRX(armarow::PHY::tx_on);
                rc.send(message);
                return common::SUCCESS;
            }
            
/*            common::Error receive(Message& msg)
            {
                BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
                return common::FAILURE;
            }
            
            template<typename AttributeContainer>
            common::Error getAttribute(AttributeContainer& attr) const
            {
                BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
                return common::FAILURE;
            }
            
            template<typename AttributeContainer>
            common::Error setAttribute(const AttributeContainer& attr)
            {
                BOOST_STATIC_ASSERT_MSG(!sizeof(Message), "not_implemented_yet");
                return common::FAILURE;
            }
*/            
        };
    }
}

using avr_halib::config::Frequency;
using avr_halib::drivers::Clock;

struct ClockConfig : public platform::avr::clock::Clock1BaseConfig
{
    typedef Frequency<1> TargetFrequency;
    typedef uint8_t TickValueType;
};

Clock<ClockConfig> periodicTrigger;

typedef narmarow::phy::Phy Myphy;
Myphy bob;
Myphy::Message message;

void send(){
    
    bob.send(message);
    log::emit() << "Sending message "
                << ((uint32_t*)message.payload)[0]++ << ::logging::log::endl;
}




int main() {
    periodicTrigger.registerCallback<send>();

    log::emit() << "Starting bubbler (repeated send of the same message)!"
                << log::endl << log::endl;

    message.header.size=sizeof(uint32_t);
    *reinterpret_cast<uint32_t*>(message.payload)=0;

    sei();

    Idler::idle();

    return 0;
}
