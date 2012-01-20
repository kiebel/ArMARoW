#pragma once

#include <avr-halib/share/delegate.h>

#include <armarow/common/attributeContainer.h>
#include <armarow/common/error.h>

namespace armarow {
namespace drv {
namespace atmega128rfa1 {
    namespace attributes {
        namespace tags
        {
            struct Channel{};
            struct Sleep{};
            struct Callback{};
            struct CCA{};
            struct CCAParams{};
        }
        namespace params
        {
            using specification::ChannelType;
            using specification::RSSIType;
            struct CCAParamType
            {
                typedef specification::CCAModeType CCAModeType;
                typedef specification::CCAThresholdType CCAThresholdType;

                CCAModeType     mode;
                CCAThresholdType threshold;
            };
            using specification::CCAType;
            using specification::SleepType;
        }
        namespace values
        {
            using specification::Channels;
            struct CCAParams : public specification::CCAModes
            {
                static const specification::CCAThresholdType minThreshold = specification::Constants::minCCAThreshold;
                static const specification::CCAThresholdType maxThreshold = specification::Constants::maxCCAThreshold;
            };
        };
    }
   
    template<typename Driver>
    struct AttributeHandler : public Driver
    {
        public:
            struct Attributes
            {
                typedef common::AttributeContainer< attributes::tags::Channel, 
                                                    attributes::params::ChannelType,
                                                    attributes::values::Channels     > Channel;
                typedef common::AttributeContainer< attributes::tags::Sleep, 
                                                    attributes::params::SleepType    > Sleep;
                typedef common::AttributeContainer< attributes::tags::CCA, 
                                                    attributes::params::CCAType      > CCA;
                typedef common::AttributeContainer< attributes::tags::CCA, 
                                                    attributes::params::CCAParamType,
                                                    attributes::values::CCAParams    > CCAParams;
                typedef common::AttributeContainer< attributes::tags::Callback,
                                                    typename Driver::CallbackType >    Callback;
            };

            common::Error setAttribute(typename Attributes::Callback& cb)
            {
                this->callUpper=cb.value;
                return common::SUCCESS;
            }

            common::Error getAttribute(typename Attributes::Callback& cb) const
            {
                cb.value=this->callUpper;
                return common::SUCCESS;
            }

            common::Error setAttribute(typename Attributes::Sleep& attr)
            {
                bool result;
                if(attr.value)
                    result=this->sleep();
                else
                    result=this->wakeup();

                if(result)
                    return common::SUCCESS;
                else
                    return common::BUSY;
            }

            common::Error getAttribute(typename Attributes::Sleep& attr) const
            {
                attr.value=this->isSleeping();
                return common::SUCCESS;
            }

            common::Error setAttribute(typename Attributes::Channel& attr)
            {
                this->setChannel(attr.value);
                return common::SUCCESS;
            }

            common::Error getAttribute(typename Attributes::Channel& attr) const
            {
                attr.value=this->getChannel();
                return common::SUCCESS;
            }

            common::Error getAttribute(typename Attributes::CCA& cca)
            {
                if(!this->startCCA())
                    return common::BUSY;
                while(!this->ccaDone());
                cca.value=this->getCCAValue();
                return common::SUCCESS;
            }

            common::Error setAttribute(typename Attributes::CCAParams& param)
            {
                if(!this->setCCAParams(param.value.mode, param.value.threshold))
                    return common::OUT_OF_RANGE;
                else
                    return common::SUCCESS;
            }
             
            common::Error getAttribute(typename Attributes::CCAParams& param) const
            {
                this->getCCAParams(param.value.mode, param.value.threshold);
                return common::SUCCESS;
            }
    };
}
}
}    
