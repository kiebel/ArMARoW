#include <avr-halib/avr/timer.h> //eggTimer

namespace armarow {
namespace mac {
namespace simple802_15_4
{

    /** \brief Timer abstraction encapsulating the IEEE 801.15.4 CSMA/CA backoff behaviour.
     *  \tparam config configuration structure for backoff timing
     **/
    template<typename config>
    class BackoffTimer{
        private:
            /** \brief One-shot-Timer executing the registered callback after timeout duration **/
            ExactEggTimer<typename config::Timer> timer;
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
            uint32_t backoffTime()
            {
                uint16_t  backoffExponent = minBackoffExponent + backoffCount;
                if( backoffExponent > maxBackoffExponent )
                    backoffExponent = maxBackoffExponent;

                // +1 enables the compiler to optimize divison to shift
                uint16_t backoffPeriods =   (uint32_t)rand()
                                          * ((1 << (backoffExponent)) - 1)
                                          / ((uint32_t)RAND_MAX + 1); 

                return  (uint32_t) backoffPeriods * backoffPeriod;
            }

        public:
            /** \brief Default constructor
             *  \tparam T type of the object on which the callback will be called
             *  \tparam F member function of T acting as callback
             *  \param obj instance of T used to call F
             *
             *   Resets backoff exponent to minimum value, also
             *   registers supplied callback function with timer
             *   delegate
             **/
            
            BackoffTimer() {
                reset();
            }

            /** \brief Destructor stopping the internal timer**/
            ~BackoffTimer() {
                timer.stop();
            }

            /** \brief Start random backoff timer
             *
             *  \return if the maximum number of backoffs was exceeded
             **/
            bool wait() {
                if(backoffCount > maxBackoffCount)
                    return false;
                uint16_t backoffTime_ms = (uint16_t)(backoffTime() / 1000);
                timer.start( backoffTime_ms );

                log::emit<log::Trace>()
                    << "started backoff timer, timeout in "
                    << backoffTime_ms << " ms." << log::endl;

                return true;
            }

            /** \brief Reset the backoff counter **/
            void reset() {
                backoffCount = 0;
            }

            template<typename T, void (T::*f)(void)>
            void register_callback(T& obj)
            {
                timer.onTimerDelegate.template bind<T, f>(&obj);
            }
    };
}
}
}
