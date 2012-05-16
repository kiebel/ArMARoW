#include <avr-halib/avr/oneShotTimer.h> 
#include <avr-halib/avr/timer.h>
#include <avr-halib/common/frequency.h>
#include <stdlib.h>

namespace armarow {
namespace mac {
namespace simple802_15_4
{

    /** \brief Timer abstraction encapsulating the IEEE 801.15.4 CSMA/CA backoff behaviour.
     *  \tparam config configuration structure for backoff timing
     **/
    template<typename config>
    class BackoffTimer
	{
        private:
			struct OneShotTimerConfig : public avr_halib::drivers::OneShotTimer::DefaultConfig
			{
                typedef platform::Timer1BaseConfig::RegMap RegMap;
                typedef platform::Timer1BaseConfig::InputFrequency InputFrequency;
				typedef avr_halib::config::Frequency<config::backoffFrequency> BaseFrequency;
			};
            /** \brief One-shot-Timer executing the registered callback after timeout duration **/
			typedef typename avr_halib::drivers::OneShotTimer::configure<OneShotTimerConfig>::type Timer;
			Timer timer;
            /** \brief number of waited backoffs
             *
             * is used to calculate the next backoff exponent and by this the
             * number of backoff Priods to wait
             *
             * see IEEE 802.15.4 7.5.1.4
             **/
            uint8_t backoffCount;

            static const uint8_t minBackoffExponent = config::minBackoffExponent;
            static const uint8_t maxBackoffExponent = config::maxBackoffExponent;
            static const uint8_t maxBackoffCount    = maxBackoffExponent - minBackoffExponent;
            static const uint32_t backoffPeriod     = config::backoffPeriod;

            /** \brief calculates random backoff time
             *
             *   \return backoff waiting time in us
             *
             *   As defined in the IEEE 802.15.4 the backoff
             *   interval containing the potential backoff values
             *   grows exponentially with the number of waited
             *   backoffs.
             **/
            uint32_t nextNrOfBackoffs()
            {
                uint16_t  backoffExponent = minBackoffExponent + backoffCount;
                if( backoffExponent > maxBackoffExponent )
                    backoffExponent = maxBackoffExponent;

                // +1 enables the compiler to optimize divison to shift
                uint16_t backoffs = ( ( (uint32_t)rand() << backoffExponent ) - 1 )
                                    / ( (uint32_t) RAND_MAX + 1); 

                return backoffs;
            }

        public:
            typedef typename Timer::CallbackType CallbackType;
            typedef typename Timer::InterruptSlotList InterruptSlotList;

            /** \brief Default constructor
             *
             *   Resets backoff exponent to minimum value
             **/
            
            BackoffTimer() {
                reset();
            }

            /** \brief Start random backoff timer
             *
             *  \return if the maximum number of backoffs was exceeded
             **/
            bool wait() {
                if(backoffCount > maxBackoffCount)
                    return false;
                backoffCount++;
                uint8_t backoffs = nextNrOfBackoffs();
                timer.template setup<Timer::Units::matchA>( backoffs );

                log::emit<log::Trace>()
                    << "started backoff timer, timeout in "
                    << (uint16_t)backoffs << log::endl;

                return true;
            }

            /** \brief Reset the backoff counter **/
            void reset() {
                backoffCount = 0;
            }

            void setCallback(const CallbackType& cb)
            {
			    timer.template setCallback<Timer::Units::matchA>(cb);
            }
    };
}
}
}
