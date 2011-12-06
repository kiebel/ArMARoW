#pragma once

#include <backoffTimer.h>

namespace armarow {
namespace mac {
namespace simple801_15_4
{
    /*! \brief  Implementation of the IEEE 802.15.4 Medium Access Layer protocol using non beacon and CSMA/CA.
     *
     *  \tparam config configuration for the CSMA/CA protocol
     *  \tparam PhysicalLayer the type of physical layer to be used as base
     *  \tparam Message type of the messages to be transmitted     */
    template<class config, class PhysicalLayer, class Message>
    class NonBeaconCsmaCa{
        private:
            struct BackoffConfig
            {
                static const uint8_t minBackoffExponent=;
                static const uint8_t maxBackoffExponent=;
                static const uint16_t backoffUnitDuration=;
                static const uint32_t backoffPeriod=;
                typedef config::BackoffTimer Timer;
            };
            PhysicalLayer& phy;
            /** \brief Pointer to message buffer for transmission**/
            Message *txBuffer;
            Delegate<Message&> txComplete;

            /** \brief CSMA/CA internal state flags **/
            struct InternalFlags
            {       
                bool txBusy : 1; /**< Flag indicating an ongoing transmission. **/

                /** \brief default constructor to initialize values **/
                InternalFlags() : txBusy(false){}
            } flags;

            void nextTry()
            {
                PhysicalLayer::Attribute::ClearChannelAssessment cca;
                phy.getAttribute(cca);
                if(cca.value)
                {
                    phy.send(*txBuffer);
                    txBuffer->state=TX_DONE;
                    flags.txBusy=false;
                    backoffTimer.reset();
                    txComplete(*txBuffer);            
                }
                else
                    if (!backoffTimer.wait())
                    {
                        txBuffer->state=FAILED;
                        flags.txBusy=false;
                        backoffTimer.reset();
                        txComplete(*txBuffer);
                    }
            }

            BackoffTimer< BackoffConfig > backoffTimer;

       public:
            NonBeaconCsmaCa(PhysicalLayer& phy) : phy(phy)
            {
                reset();
                backoffTimer.register<NonBeaconCsmaCa, &NonBeaconCsmaCa::nextTry>(*this);
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
