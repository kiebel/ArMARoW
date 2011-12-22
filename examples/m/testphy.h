#include <armarow/common/interface.h>
#include <armarow/common/message.h>
#include <armarow/common/error.h>

namespace test {
    namespace common = armarow::common;
    namespace meta = armarow::meta;
    namespace phy {
        struct Phy : public common::Interface< common::Message< 128 > >
        {
            typedef platform::config::RadioDriver<> RadioController;
            RadioController::mob_t message;
            RadioController rc;
            
            struct SignalParameters: public RadioController::SignalParameters {};
            
            typedef common::Message< 128 > Message;
            
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
            
            common::Error receive(Message& msg)
            {
                //hier nachricht abholen
                for(uint8_t i = 0; i < 128; i++)
                {
                    ((uint8_t*)&msg)[i]=((uint8_t*)&message )[i];
                }
                return common::SUCCESS;
            }
            struct Attribute{
                struct ClearChannelAssessment
                {
                    uint8_t value;
                };
            };
            
            common::Error getAttribute(Attribute::ClearChannelAssessment& attr)
            {
                rc.doCCA(attr.value);
                return common::SUCCESS;
            }

/*            
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
