UseInterrupt(SIG_OUTPUT_COMPARE3A);

namespace armarow {
namespace mac {
namespace simple802_15_4
{

    /** \brief Timer abstraction encapsulating the IEEE 801.15.4 CSMA/CA backoff behaviour.
     *  \tparam minBackoffExponent minimum Backoff exponent
     *  \tparam maxBackoffExponent maximum Backoff exponent
     *  \tparam backoffUnitDuration the duration of one backoff unit in us
     */
    template<uint8_t minBackoffExponent, uint8_t maxBackoffExponent, uint16_t backoffUnitDuration>
    class BackoffTimer {
    private:
        /** \brief One-shot-Timer executing the registered callback after timeout duration **/
        ExactEggTimer<Timer3> timer;
        /** \brief number of sequent backoffs
         * is used to calculate the next backoff exponent and by this the number of backoff Periods to wait
         * see IEEE 802.15.4 7.5.1.4
         **/
        uint8_t backoffCount;

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
        template<typedef T, void (*T::F)(void)>
        BackoffTimer(T& obj) {
            timer.onTimerDelegate<T, F>(obj);
            reset();
        }

        /** \brief Destructor stopping the internal timer**/
        ~BackoffTimer() {
            timer.stop();
        }

        /** \brief calculates random backoff time
         *   As defined in the IEEE 802.15.4 the backoff
         *   interval containing the potential backoff values
         *   grows exponentially with the number of waited
         *   backoffs.
         **/
        static uint16_t getBackoffTime(uint8_t backoffCount)
        {
            uint8_t  backoffExponent = min( (minBackoffExponent + backoffCount) , maxBackoffExponent );
            uint16_t backoffPeriods = (uint32_t)rand() * ((1 << (backoffExponent)) - 1) / (RAND_MAX + 1);
                // +1 enables the compiler to optimize divison to shift
            return backoff * backoffUnitDuration / 1000;
        }

        /** \brief Start random backoff timer
         **/
        void wait() {
            uint16_t backoffTime = getBackoffTime(backoffCount);
            timer.start(backoffTime);
            backoffCount++;
            log::emit<log::Trace>()
                << "started backoff timer, timeout in "
                << backoffTime << " ms." << log::endl;
        }
        void shortwait()
        {
            uint16_t backoffTime = getBackoffTime(0);
            timer.start(backoffTime);
            log::emit<log::Trace>()
                << "started backoff timer, timeout in "
                << backoffTime << " ms." << log::endl;
        }

        /** \brief Reset the backoff exponent to the minimal value **/
        void reset() {
            backoffCount = 0;
        }
    };
}
}
}
