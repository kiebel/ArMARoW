#pragma once

#include <avr-halib/share/delegate.h>

#include <armarow/common/attributeContainer.h>
#include <armarow/common/error.h>

namespace armarow {
namespace drv {
namespace atmega128rfa1 {
    namespace attributes {
        using common::AttributeContainer;
        namespace tags
        {
            struct Channel{};
            struct Sleep{};
            struct Callback{};
        }
        namespace params
        {
            typedef specification::Channels ChannelType;
            typedef bool SleepType;
            typedef Delegate<void> CallbackType;
        }
        typedef AttributeContainer< tags::Channel , params::ChannelType  > Channel;
        typedef AttributeContainer< tags::Sleep   , params::SleepType    > Sleep;
        typedef AttributeContainer< tags::Callback, params::CallbackType > Callback;
    }
   
    template<typename RegMap>
    struct AttributeHandler
    {
        private:
            void defaultCallback(){}
        protected:
            attributes::params::CallbackType callUpper;
        public:
            AttributeHandler()
            {
                callUpper.template bind<AttributeHandler, &AttributeHandler::defaultCallback>(this);
            }

            struct Attributes
            {
                typedef attributes::Channel  Channel;
                typedef attributes::Sleep    Sleep;
                typedef attributes::Callback Callback;
            };

            common::Error setAttribute(attributes::Callback& cb)
            {
                callUpper=cb.value;
                return common::SUCCESS;
            }

            common::Error getAttribute(attributes::Callback& cb) const
            {
                cb.value=callUpper;
                return common::SUCCESS;
            }

            common::Error setAttribute(attributes::Sleep& attr)
            {
                UseRegMap(rm, RegMap);
                rm.sleep=attr.value;
                SyncRegMap(rm);
                while(!rm.irqStatus.pllLock)
                    SyncRegMap(rm);
                rm.irqStatus.pllLock=true;
                SyncRegMap(rm);
                return common::SUCCESS;
            }

            common::Error getAttribute(attributes::Sleep& attr) const
            {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                attr.value=rm.sleep;
                return common::SUCCESS;
            }

            common::Error setAttribute(attributes::Channel& attr)
            {
                UseRegMap(rm, RegMap);
                rm.channel=attr.value;
                SyncRegMap(rm);
                while(!rm.irqStatus.pllLock)
                    SyncRegMap(rm);
                rm.irqStatus.pllLock=true;
                SyncRegMap(rm);
                return common::SUCCESS;
            }

            common::Error getAttribute(attributes::Channel& attr) const
            {
                UseRegMap(rm, RegMap);
                SyncRegMap(rm);
                attr.value=rm.channel;
                return common::SUCCESS;
            }
    };
}
}
}    
