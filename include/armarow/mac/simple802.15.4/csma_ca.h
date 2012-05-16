#pragma once

#include "backoffTimer.h"
#include <boost/mpl/joint_view.hpp>
#include <avr-halib/avr/portmap.h>
#include <debug_portmap.h>

namespace armarow {
namespace mac {
namespace simple802_15_4
{
    /** \brief  Implementation of the IEEE 802.15.4 Medium Access Layer protocol using non beacon mode and CSMA/CA.
     *
     *  \tparam config configuration for the CSMA/CA protocol
     *  \tparam PhysicalLayer the type of physical layer to be used as base
     *  \tparam Message type of the messages to be transmitted
     */
    template<class config, class PhysicalLayer, class Message>
    class NonBeaconCsmaCa{
        private:
            struct BackoffConfig : public config
            {
                static const uint8_t minBackoffExponent = 3;
                static const uint8_t maxBackoffExponent = 8;
                static const uint8_t symbolsPerBackoffPeriod = 20;
                static const uint32_t backoffFrequency  = PhysicalLayer::Constants::symbolRate / symbolsPerBackoffPeriod;
            };
            
            typedef BackoffTimer< BackoffConfig > Timer;

            PhysicalLayer& phy;
            /** \brief Pointer to message buffer for transmission**/
            Message *txBuffer;
            Timer backoffTimer;

        protected:
            void nextTry()
            {
                typename PhysicalLayer::Attributes::ClearChannelAssessment cca;

                UsePortmap(debug, ::platform::Debug);
                debug.debug2.pin=true;
                SyncPortmap(debug);

                if(!txBuffer)
                {
                    log::emit<log::Error>() << "no message to transmit" << log::endl;
                    return;
                }
                
                
                if(!phy.getAttribute(cca) && cca.value)
                {
                    typename Message::BaseMessage& phyMsg = Message::down(*txBuffer);
                    common::Error error = phy.send( phyMsg );
                    if(error)
                    {
                        log::emit<log::Error>() << "error transmitting message " << log::hex << (void*)txBuffer << " : " << error << log::endl;
                        txBuffer->properties.state = common::TX_FAILED;
                        Message::up( phyMsg );
                    }

                    txBuffer=NULL;
                    backoffTimer.reset();

                    return;
                }
                
                if( !backoffTimer.wait() )
                {
                    txBuffer->properties.state=common::TX_FAILED;
                    txBuffer=NULL;
                    backoffTimer.reset();
                }
            }

            typedef typename Timer::InterruptSlotList BackOffInterrupts;
            typedef typename PhysicalLayer::InterruptSlotList PhyInterrupts;


       public:
            typedef typename boost::mpl::joint_view< BackOffInterrupts,
                                                     PhyInterrupts 
                                                   >::type InterruptSlotList;

            NonBeaconCsmaCa(PhysicalLayer& phy) : phy(phy), txBuffer(NULL)
            {
                typename Timer::CallbackType cb;

                cb.template bind<NonBeaconCsmaCa, &NonBeaconCsmaCa::nextTry>(this);

                reset();
                                
                backoffTimer.setCallback(cb);
            }

            void sendMessage(Message& msg)
            {
                txBuffer=&msg;

                if(!backoffTimer.wait())
                    log::emit<log::Error>() << "failed  to start backoff timer!" << log::endl;
            }

            bool isReady() const
            {
                return !txBuffer;
            }

            void txDone()
            {
                txBuffer = NULL;
            }

            void reset()
            {
                txDone();
                backoffTimer.reset();
            }
    };
}
}
}
