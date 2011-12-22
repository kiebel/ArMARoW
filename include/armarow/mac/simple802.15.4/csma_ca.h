#pragma once

#include "backoffTimer.h"

namespace armarow {
namespace mac {
namespace simple802_15_4
{
    /** \brief  Implementation of the IEEE 802.15.4 Medium Access Layer protocol using non beacon and CSMA/CA.
     *
     *  \tparam config configuration for the CSMA/CA protocol
     *  \tparam PhysicalLayer the type of physical layer to be used as base
     *  \tparam Message type of the messages to be transmitted
     */
    template<class config, class PhysicalLayer, class Message>
    class NonBeaconCsmaCa{
        private:
            struct BackoffConfig
            {
                static const uint8_t minBackoffExponent = 3;
                static const uint8_t maxBackoffExponent = 8;
                static const uint32_t ticksperSecond    = 1000UL * 1000UL;
                static const uint32_t ticksperSymbol    = ticksperSecond / PhysicalLayer::SignalParameters::symbolRate;
                static const uint32_t backoffPeriod     = 20 * ticksperSymbol;
                typedef typename config::BackoffTimer Timer;
            };
            PhysicalLayer& phy;
            /** \brief Pointer to message buffer for transmission**/
            Message *txBuffer;
            Delegate<Message&> txCallback;
            BackoffTimer< BackoffConfig > backoffTimer;

            /** \brief CSMA/CA internal state flags **/
            struct InternalFlags
            {
                bool txBusy : 1; /**< Flag indicating an ongoing transmission. **/

                /** \brief default constructor to initialize values **/
                InternalFlags() : txBusy(false){}
            } flags;

            void nextTry()
            {
                typename PhysicalLayer::Attribute::ClearChannelAssessment cca;
                phy.getAttribute(cca);
                if(cca.value)
                {
                    phy.send( txBuffer->down(*txBuffer) );
                    txBuffer->properties.state=common::TX_DONE;
                    flags.txBusy=false;
                    backoffTimer.reset();
                    txCallback(*txBuffer);
                }
                else
                {
                    bool backoffsexceeded = !backoffTimer.wait();
                    if( backoffsexceeded )
                    {
                        txBuffer->properties.state=common::TX_FAILED;
                        flags.txBusy=false;
                        backoffTimer.reset();
                        txCallback(*txBuffer);
                    }
                }
            }


       public:
            NonBeaconCsmaCa(PhysicalLayer& phy) : phy(phy)
            {
                reset();
                backoffTimer.template register_callback<NonBeaconCsmaCa, &NonBeaconCsmaCa::nextTry>(*this);
            }

            void sendMessage(Message& msg) {
                txBuffer=&msg;
                flags.txBusy=true;
                backoffTimer.wait();
            }

            bool isReady() const
            {
                return flags.txBusy;
            }

            void reset()
            {
                txBuffer = 0;
                flags.txBusy=false;
            }

            void setTxDelegate(const Delegate<Message&>& d)
            {
                txCallback=d;
            }

    };
}
}
}