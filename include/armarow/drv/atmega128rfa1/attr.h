#pragma once

#include <armarow/common/attributeContainer.h>

namespace armarow {
namespace drv {
namespace atmega128rfa1 {
    namespace attributes {
        using common::AttributeContainer;
        namespace tags
        {
            struct Channel{};
            struct Sleep{};
        }
        namespace params
        {
            typedef spec::Channels ChannelType;
            typedef bool SleepType;
        }
        typedef AttributeContainer< tags::Channel, params::ChannelType > Channel;
        typedef AttributeContainer< tags::Sleep  , params::SleepType   > Sleep;
    }
   
    template<typename RegMap>
    struct AttributeHandler
    {
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

        common::Error getAttribute(attributes::Sleep& attr)
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

        common::Error getAttribute(attributes::Sleep& attr)
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
