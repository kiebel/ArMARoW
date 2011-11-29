UseInterrupt(SIG_OUTPUT_COMPARE3A);

namespace armarow {
namespace mac {
namespace simple802_15_4 {

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
			/** \brief current backoff exponent **/
			uint8_t currentBackoffExponent;

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
			BackoffTiming(T& obj) {
				timer.onTimerDelegate<T, F>(obj);
				reset();
			}

			/** \brief Destructor stopping the internal timer**/
			~BackoffTimer() {
				timer.stop();
			}

			/** \brief Start random backoff timer
			*   
			*   As defined in the IEEE 802.15.4 the backoff
			*   interval containing the potential backoff values
			*   grows exponentially with the number of waited
			*   backoffs.  The reset() function resets the backoff
			*   exponent to the minimal value.
			*
			**/
			void wait() {
				uint16_t currentBackoff = (uint32_t)rand() * ((1<<(backoffExponent)) -1) / RAND_MAX;
				uint16_t backoffTime = currentBackoff * backoffUnitDuration / 1000 ;

				timer.start(backoffTime);
				
				log::emit<log::Trace>()
					<< "starting backoff timer, timeout in " 
					<< backoffTime << " ms." << log::endl;
			}

			/** \brief Reset the backoff exponent to the minimal value **/
			void reset() {
				currentBackoffExponent = minBackoffExponent;
			}
        };

}
}
}
