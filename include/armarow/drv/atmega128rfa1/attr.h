#pragma once

#include "core.h"

#include <armarow/common/attributeContainer.h>
#include <armarow/common/error.h>
#include <avr-halib/avr/interruptLock.h>

namespace armarow {
namespace drv {
namespace atmega128rfa1
{
    using common::AttributeContainer;
    using avr_halib::locking::GlobalIntLock;

    class AttributeHandler
    {
        private:
            struct Tags
            {
                struct Channel{};
                struct Sleep{};
                struct Callback{};
                struct ClearChannelAssessment{};
                struct ClearChannelParameters{};
            };

            struct Parameters
            {
                typedef specification::ChannelType ChannelType;
                typedef specification::RSSIType    RSSIType;
                typedef specification::ClearChannelAssessmentType     ClearChannelAssessmentType;
                typedef specification::SleepType   SleepType;

                struct ClearChannelParametersType
                {
                    typedef specification::ClearChannelModeType      ClearChannelModeType;
                    typedef specification::ClearChannelThresholdType ClearChannelThresholdType;

                    ClearChannelModeType      mode;
                    ClearChannelThresholdType threshold;
                };
            };

            struct Values
            {
                struct ClearChannelParameters
                {
                    typedef specification::ClearChannelModes ClearChannelModes;
                    static const specification::ClearChannelThresholdType minThreshold = specification::Constants::minCCAThreshold;
                    static const specification::ClearChannelThresholdType maxThreshold = specification::Constants::maxCCAThreshold;
                };

                typedef specification::Channels Channels;
            };

        public:
            template<typename Config>
            class configure
            {
                private:
                    typedef typename Core::configure< Config >::type Base;

                public:
                    struct type : public Base
                    {
                        public:
                            struct Attributes
                            {
                                typedef AttributeContainer< Tags      ::Channel, 
                                                            Parameters::ChannelType,
                                                            Values    ::Channels                    > Channel;
                                typedef AttributeContainer< Tags      ::Sleep, 
                                                            Parameters::SleepType                   > Sleep;
                                typedef AttributeContainer< Tags      ::ClearChannelAssessment, 
                                                            Parameters::ClearChannelAssessmentType  > ClearChannelAssessment;
                                typedef AttributeContainer< Tags      ::ClearChannelParameters, 
                                                            Parameters::ClearChannelParametersType,
                                                            Values    ::ClearChannelParameters      > ClearChannelParameters;
                                typedef AttributeContainer< Tags      ::Callback,
                                                            typename Base::CallbackType           > Callback;
                            };
                        private:
                            typedef typename Base::RegMap RegMap;
                            typedef typename Base::StateType StateType;
                            typedef typename Base::States States;

                            static const StateType idleState = Base::idleState;

                        public:
                            typedef typename Base::Constants Constants;
                            common::Error setAttribute(const typename Attributes::Callback& cb)
                            {
                                this->setCallback(cb.value);
                                return common::SUCCESS;
                            }

                            common::Error getAttribute(typename Attributes::Callback& cb) const
                            {
                                cb.value=this->getCallback();
                                return common::SUCCESS;
                            }

                            common::Error setAttribute(const typename Attributes::Sleep& attr)
                            {
                                UseRegMap(rm, RegMap);

                                if(attr.value)
                                {
                                    if( !setState(States::trx_off) )
                                        return common::BUSY;
                                    rm.sleep=true;
                                    SyncRegMap(rm);
                                    return common::SUCCESS;
                                }
                                else
                                {
                                    rm.sleep=false;
                                    do
                                    {
                                        SyncRegMap(rm);
                                    }while( !rm.irqStatus.pllLock );

                                    rm.irqStatus.pllLock=true;
                                    SyncRegMap(rm);
                                    if(!setState(idleState))
                                        return common::BUSY;
                                }
                            }

                            common::Error getAttribute(typename Attributes::Sleep& attr) const
                            {
                                UseRegMap(rm ,RegMap);

                                SyncRegMap(rm);
                                attr.value = (rm.sleep && this->getState() == States::trx_off);
                                
                                return common::SUCCESS;
                            }

                            common::Error setAttribute(const typename Attributes::Channel& attr)
                            {
                                UseRegMap(rm, RegMap);

                               rm.channel=attr.value;
                                
                                SyncRegMap(rm);

                                if(rm.irqStatus.pllLock)
                                {
                                    GlobalIntLock lock;

                                    rm.irqStatus.pllLock=true;
                                    SyncRegMap(rm);

                                    while(!rm.irqStatus.pllLock)
                                    {
                                        log::emit<log::Trace>() << "waiting for pll to lock" << log::endl;
                                        SyncRegMap(rm);
                                    }
                                    
                                    rm.irqStatus.pllLock=true;
                                    SyncRegMap(rm);
                                }
                                
                                return common::SUCCESS;
                            }

                            common::Error getAttribute(typename Attributes::Channel& attr) const
                            {
                                UseRegMap(rm, RegMap);

                                SyncRegMap(rm);
                                *((uint8_t*)&attr.value) = rm.channel;

                                return common::SUCCESS;
                            }

                            common::Error getAttribute(typename Attributes::ClearChannelAssessment& cca)
                            {
                                UseRegMap(rm, RegMap);

                                if( !setState(States::rx_on) )
                                    return common::BUSY;

                                rm.cca_request=true;
                                SyncRegMap(rm);

                                do
                                {
                                    SyncRegMap(rm);
                                }
                                while(!rm.cca_done);

                                setState(idleState);

                                SyncRegMap(rm);
                                cca.value=rm.cca_status;

                                return common::SUCCESS;
                            }

                            common::Error setAttribute(const typename Attributes::ClearChannelParameters& param)
                            {
                                UseRegMap(rm, RegMap);

                                if( param.value.threshold < Attributes::ClearChannelParameters::minThreshold ||
                                    param.value.threshold > Attributes::ClearChannelParameters::maxThreshold    )
                                    return common::OUT_OF_RANGE;

                                rm.cca_mode      = param.value.mode;
                                rm.cca_threshold = param.value.threshold / specification::Constants::ccaThresholdModifier;
                                SyncRegMap(rm);

                                return common::SUCCESS;
                            }
                             
                            common::Error getAttribute(typename Attributes::ClearChannelParameters& param) const
                            {
                                UseRegMap(rm, RegMap);

                                SyncRegMap(rm);
                                param.value.mode      = rm.cca_mode;
                                param.value.threshold = rm.cca_threshold;

                                return common::SUCCESS;
                            }
                    };
            };
    };
}
}
}    
